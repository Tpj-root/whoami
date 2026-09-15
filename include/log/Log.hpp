#pragma once

#include <fmt/format.h>

#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace CppLogging { class Logger; }

namespace app::log {

enum class Level { Trace, Debug, Info, Warn, Error, Fatal };

const char* LevelName(Level level) noexcept;

using Fanout = std::function<void(Level, std::string_view, std::string_view)>;

/// A log destination. Implementations must be thread safe.
class IBackend
{
public:
    virtual ~IBackend() = default;
    virtual void Write(Level level, std::string_view logger, std::string_view message) = 0;
    virtual void Flush() {}
};

using BackendPtr = std::shared_ptr<IBackend>;

/// Process wide logging facade.  Every record is fanned out to all backends.
class Manager
{
public:
    static Manager& Instance();

    void AddBackend(BackendPtr backend);

    void Write(Level level, std::string_view logger, std::string_view message);
    void Flush();

    template <typename... Args>
    void Log(Level level,
             std::string_view logger,
             fmt::format_string<Args...> format,
             Args&&... args)
    {
        Write(level, logger, fmt::format(format, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void Debug(std::string_view logger, fmt::format_string<Args...> f, Args&&... a)
    { Log(Level::Debug, logger, f, std::forward<Args>(a)...); }

    template <typename... Args>
    void Info(std::string_view logger, fmt::format_string<Args...> f, Args&&... a)
    { Log(Level::Info, logger, f, std::forward<Args>(a)...); }

    template <typename... Args>
    void Warn(std::string_view logger, fmt::format_string<Args...> f, Args&&... a)
    { Log(Level::Warn, logger, f, std::forward<Args>(a)...); }

    template <typename... Args>
    void Error(std::string_view logger, fmt::format_string<Args...> f, Args&&... a)
    { Log(Level::Error, logger, f, std::forward<Args>(a)...); }

private:
    Manager() = default;

    std::mutex              _mutex;
    std::vector<BackendPtr> _backends;
};

// --------------------------------------------------------------------------
//  Backends
// --------------------------------------------------------------------------

/// Forwards records to a callback (used to push them into the UI).
class PanelBackend final : public IBackend
{
public:
    explicit PanelBackend(Fanout fanout);
    void Write(Level level, std::string_view logger, std::string_view message) override;

private:
    Fanout _fanout;
};

/// Plain text file backend.
class FileBackend final : public IBackend
{
public:
    explicit FileBackend(const std::string& path);
    ~FileBackend() override;

    void Write(Level level, std::string_view logger, std::string_view message) override;
    void Flush() override;

private:
    std::mutex    _mutex;
    std::ofstream _stream;
};

#if defined(APP_WITH_CPPLOGGING)
/// CppLogging powered backend: writes to a file *and* forwards to the panel.
class CppLoggingBackend final : public IBackend
{
public:
    CppLoggingBackend(const std::string& path, Fanout fanout);
    ~CppLoggingBackend() override;

    void Write(Level level, std::string_view logger, std::string_view message) override;
    void Flush() override;

private:
    std::shared_ptr<CppLogging::Logger> _logger;
};
#endif

} // namespace app::log