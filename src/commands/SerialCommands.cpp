#include "commands/Commands.hpp"
#include "io/SerialPort.hpp"

#include <fmt/format.h>

#include <string>

namespace app::commands {

namespace {

constexpr auto kMuted = app::ui::Color::BrightBlack;
constexpr auto kInfo  = app::ui::Color::BrightCyan;
constexpr auto kOk    = app::ui::Color::BrightGreen;
constexpr auto kErr   = app::ui::Color::BrightRed;

} // namespace

// ------------------------------------------------------------ SendCommand --
CommandResult SendCommand::Execute(const std::vector<std::string>& args,
                                   CommandContext& context)
{
    CommandResult result;

    const std::string payload = CommandParser::Join(args, 0);
    if (payload.empty())
    {
        result.Left("[send] missing payload", kErr);
        result.Right("ERR: USAGE: send <raw text>", kErr);
        return result;
    }

    if (!context.serial.IsOpen())
    {
        result.Left("[send] link down", kErr);
        result.Right("ERR: SERIAL_NOT_OPEN", kErr);
        return result;
    }

    context.serial.PollLines();
    result.Left("[tx] raw -> " + payload, kMuted);

    if (!context.serial.WriteLine(payload))
    {
        result.Right("ERR: WRITE_FAILED", kErr);
        return result;
    }

    std::string reply;
    if (context.serial.WaitForLine(reply, std::chrono::milliseconds(1500)))
        result.Right(reply, kOk);
    else
        result.Right("WARN: no response", app::ui::Color::BrightYellow);

    return result;
}

// ---------------------------------------------------------- SerialCommand --
CommandResult SerialCommand::Execute(const std::vector<std::string>& args,
                                     CommandContext& context)
{
    CommandResult result;

    const std::string sub = args.empty() ? "INFO" : CommandParser::ToUpper(args[0]);

    // ---------------------------------------------------------------- info
    if (sub == "INFO")
    {
        result.Left("[serial] state", kMuted);
        result.Left("[serial] configured device / baud", kMuted);

        result.Right(fmt::format("port      : {}", context.state.Device()), kInfo);
        result.Right(fmt::format("baud      : {}", context.state.BaudRate()), kInfo);
        result.Right(fmt::format("link      : {}",
                                 context.serial.IsOpen() ? "OPEN" : "CLOSED"),
                     context.serial.IsOpen() ? kOk : kErr);
        return result;
    }

    // ---------------------------------------------------------------- open
    if (sub == "OPEN")
    {
        result.Left("[serial] opening link", kMuted);

        if (context.serial.IsOpen())
        {
            result.Right("already open", kInfo);
            return result;
        }

        const std::string device = args.size() > 1 ? args[1] : context.state.Device();
        const int         baud   = context.state.BaudRate();

        if (context.serial.Open(device, baud))
        {
            context.state.ConfigurePort(device, baud);
            context.state.SetPortOpen(true);
            result.Right(fmt::format("OK: opened {} @ {} baud", device, baud), kOk);
        }
        else
        {
            context.state.SetPortOpen(false);
            result.Right(fmt::format("ERR: cannot open {} @ {} baud", device, baud), kErr);
        }
        return result;
    }

    // --------------------------------------------------------------- close
    if (sub == "CLOSE")
    {
        result.Left("[serial] closing link", kMuted);
        context.serial.Close();
        context.state.SetPortOpen(false);
        result.Right("OK: link closed", kOk);
        return result;
    }

    // ---------------------------------------------------------------- port
    if (sub == "PORT")
    {
        if (args.size() < 2)
        {
            result.Left("[serial] missing device", kErr);
            result.Right("ERR: USAGE: serial port <device>", kErr);
            return result;
        }

        result.Left("[serial] reconfiguring", kMuted);
        context.state.ConfigurePort(args[1], context.state.BaudRate());
        result.Right("OK: device set to " + args[1] + " (run 'serial open')", kOk);
        return result;
    }

    // ---------------------------------------------------------------- baud
    if (sub == "BAUD")
    {
        if (args.size() < 2)
        {
            result.Left("[serial] missing baud rate", kErr);
            result.Right("ERR: USAGE: serial baud <n>", kErr);
            return result;
        }

        const int baud = std::stoi(args[1]);
        result.Left("[serial] reconfiguring", kMuted);
        context.state.ConfigurePort(context.state.Device(), baud);
        result.Right(fmt::format("OK: baud set to {} (run 'serial open')", baud), kOk);
        return result;
    }

    result.Left("[serial] unknown sub-command", kErr);
    result.Right("ERR: UNKNOWN_SUBCOMMAND: " + sub, kErr);
    result.Right("usage: serial [info|open|close|port <dev>|baud <n>]", kMuted);
    return result;
}

} // namespace app::commands