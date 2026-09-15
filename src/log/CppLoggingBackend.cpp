#include "log/Log.hpp"

#if defined(APP_WITH_CPPLOGGING)

#include "logging/logging.h"

#include <filesystem>
#include <string>

namespace app::log {

namespace {

CppLogging::Level ToCppLevel(Level level) noexcept
{
    switch (level)
    {
    case Level::Trace:
    case Level::Debug: return CppLogging::Level::Debug;
    case Level::Info:  return CppLogging::Level::Info;
    case Level::Warn:  return CppLogging::Level::Warn;
    case Level::Error: return CppLogging::Level::Error;
    case Level::Fatal: return CppLogging::Level::Fatal;
    }
    return CppLogging::Level::Info;
}

Level FromCppLevel(CppLogging::Level level) noexcept
{
    switch (level)
    {
    case CppLogging::Level::Debug: return Level::Debug;
    case CppLogging::Level::Info:  return Level::Info;
    case CppLogging::Level::Warn:  return Level::Warn;
    case CppLogging::Level::Error: return Level::Error;
    case CppLogging::Level::Fatal: return Level::Fatal;
    }
    return Level::Info;
}

/// Custom CppLogging sink that forwards every record into the UI panel.
///
/// NOTE: CppLogging exposes the formatted payload through `record.message`
///       (and, when a layout has run, through `record.buffer`).  We only use
///       `message` so the sink works with every layout configuration.
class PanelSink final : public CppLogging::Sink
{
public:
    explicit PanelSink(Fanout fanout)
        : _fanout(std::move(fanout))
    {
    }

    bool ProcessRecord(CppLogging::Record& record) override
    {
        if (_fanout)
        {
            const std::string_view message(record.message);
            const std::string_view logger(record.logger);
            _fanout(FromCppLevel(record.level), logger, message);
        }
        return true;
    }

private:
    Fanout _fanout;
};

} // namespace

CppLoggingBackend::CppLoggingBackend(const std::string& path, Fanout fanout)
{
    const std::filesystem::path file(path);
    if (file.has_parent_path())
        std::filesystem::create_directories(file.parent_path());

    _logger = std::make_shared<CppLogging::Logger>();

    // --- persistent file sink -------------------------------------------
    auto fileSink = std::make_shared<CppLogging::FileSink>(path);
    fileSink->layouts().push_back(std::make_shared<CppLogging::TextLayout>());
    _logger->sinks().push_back(fileSink);

    // --- UI panel sink ---------------------------------------------------
    if (fanout)
    {
        auto panelSink = std::make_shared<PanelSink>(std::move(fanout));
        _logger->sinks().push_back(panelSink);
    }
}

CppLoggingBackend::~CppLoggingBackend() = default;

void CppLoggingBackend::Write(Level level,
                              std::string_view logger,
                              std::string_view message)
{
    if (!_logger)
        return;

    // "{0}" is understood both by fmt and by CppCommon's formatter.
    _logger->Log(ToCppLevel(level), "{0}", std::string(message));
}

void CppLoggingBackend::Flush()
{
}

} // namespace app::log

#endif // APP_WITH_CPPLOGGING