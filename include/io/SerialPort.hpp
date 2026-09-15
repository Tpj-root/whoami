#pragma once

#include <atomic>
#include <chrono>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace app::io {

/// POSIX (termios) serial port wrapper, configured for the Arduino Nano.
class SerialPort
{
public:
    SerialPort() = default;
    ~SerialPort();

    SerialPort(const SerialPort&)            = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    bool Open(const std::string& device, int baudRate);
    void Close();

    bool        IsOpen()   const noexcept { return _open.load(); }
    std::string Device()   const { return _device; }
    int         BaudRate() const noexcept { return _baud; }

    bool Write(const std::string& data);
    bool WriteLine(const std::string& line);

    /// Non blocking. Returns every complete line received so far.
    std::vector<std::string> PollLines();

    /// Blocking with timeout. Waits for one complete line.
    bool WaitForLine(std::string& line, std::chrono::milliseconds timeout);

    void FlushInput();

private:
    void CloseLocked();
    void PumpLocked(std::chrono::milliseconds timeout);
    void SplitLocked();

    mutable std::mutex        _mutex;
    int                       _fd     = -1;
    std::atomic<bool>         _open{false};
    std::string               _device;
    int                       _baud   = 9600;
    std::string               _buffer;
    std::deque<std::string>   _lines;
};

} // namespace app::io