#include "commands/Commands.hpp"
#include "io/SerialPort.hpp"

#include <chrono>
#include <utility>

namespace app::commands {

namespace {

constexpr auto kMuted = app::ui::Color::BrightBlack;
constexpr auto kInfo  = app::ui::Color::BrightCyan;
constexpr auto kOk    = app::ui::Color::BrightGreen;
constexpr auto kWarn  = app::ui::Color::BrightYellow;
constexpr auto kErr   = app::ui::Color::BrightRed;

constexpr auto kDeviceTimeout = std::chrono::milliseconds(1200);

/// Sends one line to the Arduino and waits for the reply.
CommandResult TalkToDevice(const std::string& wire, CommandContext& context)
{
    CommandResult result;

    if (!context.serial.IsOpen())
    {
        result.Left("[serial] link down", kErr);
        result.Right("ERR: SERIAL_NOT_OPEN", kErr);
        result.Right("hint: run 'serial open' or restart with --port <device>", kMuted);
        return result;
    }

    context.serial.PollLines();          // discard stale bytes

    if (!context.serial.WriteLine(wire))
    {
        result.Left("[serial] write failed", kErr);
        result.Right("ERR: WRITE_FAILED", kErr);
        return result;
    }

    result.Left("[tx] -> " + wire, kMuted);

    std::string reply;
    if (context.serial.WaitForLine(reply, kDeviceTimeout))
        result.Right(reply, kOk);
    else
        result.Right("WARN: no response within 1200 ms", kWarn);

    return result;
}

} // namespace

// ------------------------------------------------------------- LedCommand --
LedCommand::LedCommand(std::string              name,
                       std::vector<std::string> aliases,
                       std::string              wire,
                       std::string              description,
                       AppState::Mode           mode)
    : _name(std::move(name))
    , _aliases(std::move(aliases))
    , _wire(std::move(wire))
    , _description(std::move(description))
    , _mode(mode)
{
}

CommandResult LedCommand::Execute(const std::vector<std::string>&,
                                  CommandContext& context)
{
    CommandResult result;
    result.Left("[signal] " + _name, kMuted);

    const bool wasOpen = context.serial.IsOpen();

    CommandResult device = TalkToDevice(_wire, context);
    result.Left("[mode] " + std::string(AppState::ModeName(context.state.CurrentMode())) +
                " -> " + std::string(AppState::ModeName(_mode)), kMuted);

    if (wasOpen)
        context.state.SetMode(_mode);

    for (const auto& line : device.LeftLines())
        result.Left(line.text, line.color);
    for (const auto& line : device.RightLines())
        result.Right(line.text, line.color);

    return result;
}

// ---------------------------------------------------------- StatusCommand --
CommandResult StatusCommand::Execute(const std::vector<std::string>&,
                                     CommandContext& context)
{
    CommandResult result;
    result.Left("[status] querying device", kMuted);
    result.Left("[status] local mode = " +
                std::string(AppState::ModeName(context.state.CurrentMode())), kMuted);

    CommandResult device = TalkToDevice("STATUS", context);

    for (const auto& line : device.RightLines())
        result.Right(line.text,
                     line.color == kOk ? kInfo : line.color);

    return result;
}

} // namespace app::commands