#include "io/SerialPort.hpp"

#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>

namespace app::io {

namespace {

speed_t ToSpeed(int baud) noexcept
{
    switch (baud)
    {
    case 1200:   return B1200;
    case 2400:   return B2400;
    case 4800:   return B4800;
    case 9600:   return B9600;
    case 19200:  return B19200;
    case 38400:  return B38400;
    case 57600:  return B57600;
    case 115200: return B115200;
    case 230400: return B230400;
    default:     return B9600;
    }
}

} // namespace

SerialPort::~SerialPort()
{
    Close();
}

bool SerialPort::Open(const std::string& device, int baudRate)
{
    std::lock_guard<std::mutex> lock(_mutex);
    CloseLocked();

    const int fd = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
        return false;

    termios tty {};
    if (::tcgetattr(fd, &tty) != 0)
    {
        ::close(fd);
        return false;
    }

    const speed_t speed = ToSpeed(baudRate);
    ::cfsetospeed(&tty, speed);
    ::cfsetispeed(&tty, speed);

    tty.c_cflag  = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
#ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS;
#endif

    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
                     INLCR | IGNCR | ICRNL | IXON);
    tty.c_oflag &= ~OPOST;
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 1;                 // 100 ms read timeout

    if (::tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        ::close(fd);
        return false;
    }

    ::tcflush(fd, TCIOFLUSH);

    _fd     = fd;
    _device = device;
    _baud   = baudRate;
    _buffer.clear();
    _lines.clear();
    _open.store(true);

    return true;
}

void SerialPort::CloseLocked()
{
    if (_fd >= 0)
    {
        ::close(_fd);
        _fd = -1;
    }

    _buffer.clear();
    _lines.clear();
    _open.store(false);
}

void SerialPort::Close()
{
    std::lock_guard<std::mutex> lock(_mutex);
    CloseLocked();
}

void SerialPort::FlushInput()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_fd >= 0)
        ::tcflush(_fd, TCIFLUSH);
    _buffer.clear();
    _lines.clear();
}

bool SerialPort::Write(const std::string& data)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_fd < 0)
        return false;

    std::size_t sent = 0;
    while (sent < data.size())
    {
        const ssize_t written = ::write(_fd, data.data() + sent, data.size() - sent);
        if (written < 0)
        {
            if (errno == EINTR)
                continue;
            return false;
        }
        sent += static_cast<std::size_t>(written);
    }

    ::tcdrain(_fd);
    return true;
}

bool SerialPort::WriteLine(const std::string& line)
{
    return Write(line + "\n");
}

void SerialPort::PumpLocked(std::chrono::milliseconds timeout)
{
    if (_fd < 0)
        return;

    char chunk[256];

    for (;;)
    {
        pollfd descriptor {};
        descriptor.fd     = _fd;
        descriptor.events = POLLIN;

        const int ready = ::poll(&descriptor, 1, static_cast<int>(timeout.count()));
        if (ready <= 0)
            return;

        const ssize_t got = ::read(_fd, chunk, sizeof(chunk));
        if (got > 0)
        {
            _buffer.append(chunk, static_cast<std::size_t>(got));
            timeout = std::chrono::milliseconds(0);   // keep draining
            continue;
        }

        if (got < 0 && errno == EINTR)
            continue;

        if (got < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return;

        return;
    }
}

void SerialPort::SplitLocked()
{
    std::size_t position;

    while ((position = _buffer.find('\n')) != std::string::npos)
    {
        std::string line = _buffer.substr(0, position);
        _buffer.erase(0, position + 1);

        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        _lines.push_back(std::move(line));
    }
}

std::vector<std::string> SerialPort::PollLines()
{
    std::lock_guard<std::mutex> lock(_mutex);

    PumpLocked(std::chrono::milliseconds(0));
    SplitLocked();

    std::vector<std::string> out(_lines.begin(), _lines.end());
    _lines.clear();
    return out;
}

bool SerialPort::WaitForLine(std::string& line, std::chrono::milliseconds timeout)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;

    std::lock_guard<std::mutex> lock(_mutex);
    if (_fd < 0)
        return false;

    for (;;)
    {
        SplitLocked();

        if (!_lines.empty())
        {
            line = _lines.front();
            _lines.pop_front();
            return true;
        }

        if (std::chrono::steady_clock::now() >= deadline)
            return false;

        PumpLocked(std::chrono::milliseconds(50));
    }
}

} // namespace app::io