#include "commands/Commands.hpp"
#include "io/SerialPort.hpp"
#include <memory>

namespace app::commands {

void RegisterAll(CommandRegistry& registry)
{
    // ------------------------------------------------------------- basics --
    registry.Register(std::make_shared<HelpCommand>(registry));
    registry.Register(std::make_shared<WhoAmICommand>());
    registry.Register(std::make_shared<EchoCommand>());
    registry.Register(std::make_shared<HistoryCommand>());
    registry.Register(std::make_shared<ClearCommand>());
    registry.Register(std::make_shared<ExitCommand>());

    // ---------------------------------------------------------- LED modes --
    registry.Register(std::make_shared<LedCommand>(
        "BUY", std::vector<std::string>{ "GREEN_ON" },
        "BUY",
        "Green solid (D9) + green fast blink (D10)",
        AppState::Mode::Buy));

    registry.Register(std::make_shared<LedCommand>(
        "SELL", std::vector<std::string>{ "RED_ON" },
        "SELL",
        "Red solid (D8) + red fast blink (D7)",
        AppState::Mode::Sell));

    registry.Register(std::make_shared<LedCommand>(
        "OFF", std::vector<std::string>{ "ALL_OFF" },
        "OFF",
        "All four LEDs off",
        AppState::Mode::Off));

    registry.Register(std::make_shared<LedCommand>(
        "ALL_ON", std::vector<std::string>{},
        "ALL_ON",
        "All four LEDs solid on",
        AppState::Mode::AllOn));

    // --------------------------------------------------------- diagnostics --
    registry.Register(std::make_shared<StatusCommand>());
    registry.Register(std::make_shared<SendCommand>());
    registry.Register(std::make_shared<SerialCommand>());
}

} // namespace app::commands