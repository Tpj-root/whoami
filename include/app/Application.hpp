#pragma once

#include <memory>
#include <string>

#include "app/AppState.hpp"
#include "app/AsyncBus.hpp"
#include "app/Command.hpp"
#include "app/Shell.hpp"
#include "io/SerialPort.hpp"
#include "ui/PanelRenderer.hpp"

namespace app {

struct Options
{
    std::string port      = "/dev/ttyUSB0";
    int         baud      = 9600;
    int         leftWidth = 34;
    std::string logFile   = "logs/whoshell.log";
    std::string prompt    = "who> ";
    bool        useSerial = true;
};

/// Wires every object together and owns the application lifetime.
class Application
{
public:
    explicit Application(Options options);

    int Run();

    static Options ParseCommandLine(int argc, char** argv);

private:
    void SetupLogging();
    void RegisterCommands();

    Options                _options;
    AppState               _state;
    io::SerialPort         _serial;
    AsyncBus               _bus;
    ui::PanelRenderer      _renderer;
    CommandRegistry        _registry;
    CommandContext         _context;
    std::unique_ptr<Shell> _shell;
};

} // namespace app