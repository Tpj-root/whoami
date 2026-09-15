#pragma once

#include <mutex>
#include <string>
#include <vector>

namespace app {

/// Shared, mutable state that every command may inspect / change.
class AppState
{
public:
    enum class Mode { Unknown, Buy, Sell, Off, AllOn };

    static const char* ModeName(Mode mode) noexcept;

    Mode CurrentMode() const noexcept { return _mode; }
    void SetMode(Mode mode) noexcept { _mode = mode; }

    void AddHistory(const std::string& entry);
    std::vector<std::string> History() const;

    void ConfigurePort(const std::string& device, int baud)
    {
        _device = device;
        _baud   = baud;
    }

    void SetPortOpen(bool open) noexcept { _portOpen = open; }

    std::string Device()   const { return _device; }
    int         BaudRate() const noexcept { return _baud; }
    bool        PortOpen() const noexcept { return _portOpen; }

private:
    Mode _mode = Mode::Unknown;

    mutable std::mutex       _historyMutex;
    std::vector<std::string> _history;

    std::string _device   = "/dev/ttyUSB0";
    int         _baud     = 9600;
    bool        _portOpen = false;
};

} // namespace app