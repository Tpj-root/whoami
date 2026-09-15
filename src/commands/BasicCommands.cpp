#include "commands/Commands.hpp"
#include "io/SerialPort.hpp"

#include <pwd.h>
#include <unistd.h>

#include <cstdlib>
#include <string>

namespace app::commands {

namespace {
constexpr auto kMuted  = app::ui::Color::BrightBlack;
constexpr auto kInfo   = app::ui::Color::BrightCyan;
constexpr auto kOk     = app::ui::Color::BrightGreen;
constexpr auto kErr    = app::ui::Color::BrightRed;
}

// ------------------------------------------------------------------- help --
CommandResult HelpCommand::Execute(const std::vector<std::string>&,
                                   CommandContext&)
{
    CommandResult result;
    result.Left("[help] command index", kMuted);
    result.Right("Available commands", app::ui::Color::BrightWhite);

    for (const auto& command : _registry.All())
    {
        std::string line = "  " + command->Name();

        const auto aliases = command->Aliases();
        if (!aliases.empty())
        {
            line += "  (";
            for (std::size_t i = 0; i < aliases.size(); ++i)
            {
                if (i != 0)
                    line += ", ";
                line += aliases[i];
            }
            line += ")";
        }

        line += "   " + command->Description();
        result.Right(line, kInfo);
    }

    result.Right("usage: <command> [args]      example: echo \"IAMSHADOW\"", kMuted);
    return result;
}

// ----------------------------------------------------------------- whoami --
CommandResult WhoAmICommand::Execute(const std::vector<std::string>&,
                                     CommandContext& context)
{
    CommandResult result;
    result.Left("[auth] resolving identity", kMuted);

    std::string name = "unknown";

    if (const passwd* entry = ::getpwuid(::getuid()); entry && entry->pw_name)
        name = entry->pw_name;
    else if (const char* env = std::getenv("USER"); env && *env)
        name = env;
    else if (const char* env = std::getenv("LOGNAME"); env && *env)
        name = env;

    result.Left("[auth] uid=" + std::to_string(::getuid()) +
                "  port=" + (context.state.PortOpen() ? "OPEN" : "CLOSED"), kMuted);
    result.Right(name, kOk);

    return result;
}

// ------------------------------------------------------------------- echo --
CommandResult EchoCommand::Execute(const std::vector<std::string>& args,
                                   CommandContext&)
{
    CommandResult result;
    result.Left("[echo]", kMuted);
    result.Right(CommandParser::Join(args, 0), app::ui::Color::BrightWhite);
    return result;
}

// ---------------------------------------------------------------- history --
CommandResult HistoryCommand::Execute(const std::vector<std::string>&,
                                      CommandContext& context)
{
    CommandResult result;
    result.Left("[history] replaying buffer", kMuted);

    const auto entries = context.state.History();
    if (entries.empty())
    {
        result.Right("(history is empty)", kMuted);
        return result;
    }

    result.Right("Command history", app::ui::Color::BrightWhite);
    for (std::size_t i = 0; i < entries.size(); ++i)
        result.Right("  " + std::to_string(i + 1) + "  " + entries[i], kInfo);

    return result;
}

// ------------------------------------------------------------------ clear --
CommandResult ClearCommand::Execute(const std::vector<std::string>&,
                                    CommandContext&)
{
    CommandResult result;
    result.Clear();
    return result;
}

// ------------------------------------------------------------------- exit --
CommandResult ExitCommand::Execute(const std::vector<std::string>&,
                                   CommandContext&)
{
    CommandResult result;
    result.Left("[shell] shutting down", kMuted);
    result.Right("bye", kOk);
    result.Exit();
    return result;
}

} // namespace app::commands