#include "app/Application.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>

#include "commands/Commands.hpp"
#include "log/Log.hpp"

namespace app {

namespace {
constexpr const char* kVersion = "1.0.0";
}

Application::Application(Options options)
    : _options(std::move(options))
    , _renderer(_options.leftWidth)
    , _context{ _serial, _state }
{
    RegisterCommands();

    _shell = std::make_unique<Shell>(_registry, _context, _renderer, _bus, _options.prompt);
}

Options Application::ParseCommandLine(int argc, char** argv)
{
    Options options;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];

        auto take = [&](std::string& out)
        {
            if (i + 1 < argc)
                out = argv[++i];
        };

        if (arg == "--port" || arg == "-p")
        {
            take(options.port);
        }
        else if (arg == "--baud" || arg == "-b")
        {
            std::string value;
            take(value);
            if (!value.empty())
                options.baud = std::stoi(value);
        }
        else if (arg == "--left-width" || arg == "-w")
        {
            std::string value;
            take(value);
            if (!value.empty())
                options.leftWidth = std::stoi(value);
        }
        else if (arg == "--log")
        {
            take(options.logFile);
        }
        else if (arg == "--prompt")
        {
            take(options.prompt);
        }
        else if (arg == "--no-serial")
        {
            options.useSerial = false;
        }
        else if (arg == "--help" || arg == "-h")
        {
            std::cout
                << "who shell " << kVersion << "\n\n"
                << "  -p, --port <device>    serial device      (default /dev/ttyUSB0)\n"
                << "  -b, --baud <rate>      baud rate          (default 9600)\n"
                << "  -w, --left-width <n>   left panel width   (default 34)\n"
                << "      --log <path>       log file           (default logs/whoshell.log)\n"
                << "      --prompt <text>    prompt string      (default \"who> \")\n"
                << "      --no-serial        do not open the serial port at startup\n"
                << "  -h, --help             this text\n";
            std::exit(0);
        }
    }

    return options;
}

void Application::RegisterCommands()
{
    commands::RegisterAll(_registry);
}

void Application::SetupLogging()
{
    auto& manager = log::Manager::Instance();

    // Fan-out used by every "panel" sink: pushes the record into the AsyncBus.
    auto fanout = [this](log::Level level,
                         std::string_view logger,
                         std::string_view message)
    {
        ui::Color color = ui::Color::BrightBlack;

        switch (level)
        {
        case log::Level::Trace: color = ui::Color::BrightBlack;  break;
        case log::Level::Debug: color = ui::Color::BrightBlack;  break;
        case log::Level::Info:  color = ui::Color::BrightCyan;   break;
        case log::Level::Warn:  color = ui::Color::BrightYellow; break;
        case log::Level::Error: color = ui::Color::BrightRed;    break;
        case log::Level::Fatal: color = ui::Color::BrightRed;    break;
        }

        std::string tag = "[";
        tag += logger;
        tag += "]";

        _bus.Push(AsyncMessage{ std::move(tag), std::string(message), color });
    };

#if defined(APP_WITH_CPPLOGGING)
    manager.AddBackend(std::make_shared<log::CppLoggingBackend>(_options.logFile, fanout));
#else
    manager.AddBackend(std::make_shared<log::FileBackend>(_options.logFile));
    manager.AddBackend(std::make_shared<log::PanelBackend>(fanout));
#endif
}

int Application::Run()
{
    SetupLogging();

    auto& manager = log::Manager::Instance();
    manager.Info("app", "who shell {} starting", kVersion);

    _state.ConfigurePort(_options.port, _options.baud);

    if (_options.useSerial)
    {
        if (_serial.Open(_options.port, _options.baud))
        {
            _state.SetPortOpen(true);
            manager.Info("serial", "opened {} @ {} baud", _options.port, _options.baud);
        }
        else
        {
            _state.SetPortOpen(false);
            manager.Warn("serial", "cannot open {} @ {} baud - use 'serial open' later",
                         _options.port, _options.baud);
        }
    }
    else
    {
        manager.Warn("serial", "serial disabled (--no-serial)");
    }

    const int exitCode = _shell->Run();

    if (_serial.IsOpen())
        _serial.Close();

    manager.Info("app", "shutdown (exit code {})", exitCode);
    manager.Flush();

    return exitCode;
}

} // namespace app