#include "log/Log.hpp"

#include <chrono>
#include <cstring>
#include <ctime>
#include <filesystem>

namespace app::log {

const char* LevelName(Level level) noexcept
{
    switch (level)
    {
    case Level::Trace: return "TRACE";
    case Level::Debug: return "DEBUG";
    case Level::Info:  return "INFO ";
    case Level::Warn:  return "WARN ";
    case Level::Error: return "ERROR";
    case Level::Fatal: return "FATAL";
    }
    return "?????";
}

// --------------------------------------------------------------------------
//  Manager
// --------------------------------------------------------------------------

Manager& Manager::Instance()
{
    static Manager instance;
    return instance;
}

void Manager::AddBackend(BackendPtr backend)
{
    if (!backend)
        return;

    std::lock_guard<std::mutex> lock(_mutex);
    _backends.push_back(std::move(backend));
}

void Manager::Write(Level level, std::string_view logger, std::string_view message)
{
    std::vector<BackendPtr> backends;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        backends = _backends;                 // snapshot to avoid holding the lock
    }

    for (auto& backend : backends)
        backend->Write(level, logger, message);
}

void Manager::Flush()
{
    std::vector<BackendPtr> backends;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        backends = _backends;
    }

    for (auto& backend : backends)
        backend->Flush();
}

// --------------------------------------------------------------------------
//  PanelBackend
// --------------------------------------------------------------------------

PanelBackend::PanelBackend(Fanout fanout)
    : _fanout(std::move(fanout))
{
}

void PanelBackend::Write(Level level, std::string_view logger, std::string_view message)
{
    if (_fanout)
        _fanout(level, logger, message);
}

// --------------------------------------------------------------------------
//  FileBackend
// --------------------------------------------------------------------------

FileBackend::FileBackend(const std::string& path)
{
    const std::filesystem::path file(path);
    if (file.has_parent_path())
        std::filesystem::create_directories(file.parent_path());

    _stream.open(path, std::ios::app);
}

FileBackend::~FileBackend()
{
    Flush();
}

void FileBackend::Write(Level level, std::string_view logger, std::string_view message)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_stream)
        return;

    const auto now       = std::chrono::system_clock::now();
    const std::time_t t  = std::chrono::system_clock::to_time_t(now);

    std::tm local {};
    localtime_r(&t, &local);

    char stamp[32] {};
    std::strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", &local);

    _stream << stamp << " [" << LevelName(level) << "] ["
            << logger << "] " << message << '\n';
}

void FileBackend::Flush()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_stream)
        _stream.flush();
}

} // namespace app::log