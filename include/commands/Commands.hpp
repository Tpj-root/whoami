#pragma once

#include <memory>
#include <string>
#include <vector>

#include "app/Command.hpp"

namespace app::commands {

// ---------------------------------------------------------------- basic ---
class HelpCommand final : public ICommand
{
public:
    explicit HelpCommand(const CommandRegistry& registry) : _registry(registry) {}

    std::string Name()        const override { return "HELP"; }
    std::string Usage()       const override { return "help"; }
    std::string Description() const override { return "List every registered command"; }

    CommandResult Execute(const std::vector<std::string>& args,
                          CommandContext& context) override;

private:
    const CommandRegistry& _registry;
};

class WhoAmICommand final : public ICommand
{
public:
    std::string Name()        const override { return "WHOAMI"; }
    std::string Usage()       const override { return "whoami"; }
    std::string Description() const override { return "Print the current user identity"; }

    CommandResult Execute(const std::vector<std::string>& args,
                          CommandContext& context) override;
};

class EchoCommand final : public ICommand
{
public:
    std::string Name()        const override { return "ECHO"; }
    std::string Usage()       const override { return "echo \"text\""; }
    std::string Description() const override { return "Write text to the right panel"; }

    CommandResult Execute(const std::vector<std::string>& args,
                          CommandContext& context) override;
};

class HistoryCommand final : public ICommand
{
public:
    std::string Name()        const override { return "HISTORY"; }
    std::string Usage()       const override { return "history"; }
    std::string Description() const override { return "Show the command history"; }

    CommandResult Execute(const std::vector<std::string>& args,
                          CommandContext& context) override;
};

class ClearCommand final : public ICommand
{
public:
    std::string Name()        const override { return "CLEAR"; }
    std::string Usage()       const override { return "clear"; }
    std::string Description() const override { return "Clear the terminal"; }

    CommandResult Execute(const std::vector<std::string>& args,
                          CommandContext& context) override;
};

class ExitCommand final : public ICommand
{
public:
    std::string Name()        const override { return "EXIT"; }
    std::vector<std::string> Aliases() const override { return { "QUIT" }; }
    std::string Usage()       const override { return "exit | quit"; }
    std::string Description() const override { return "Leave the shell"; }

    CommandResult Execute(const std::vector<std::string>& args,
                          CommandContext& context) override;
};

// -------------------------------------------------------- serial / LED ---
/// Generic "send one wire command to the Arduino" command.
class LedCommand final : public ICommand
{
public:
    LedCommand(std::string              name,
               std::vector<std::string> aliases,
               std::string              wire,
               std::string              description,
               AppState::Mode           mode);

    std::string              Name()        const override { return _name; }
    std::vector<std::string> Aliases()     const override { return _aliases; }
    std::string              Usage()       const override { return _name; }
    std::string              Description() const override { return _description; }

    CommandResult Execute(const std::vector<std::string>& args,
                          CommandContext& context) override;

private:
    std::string              _name;
    std::vector<std::string> _aliases;
    std::string              _wire;
    std::string              _description;
    AppState::Mode           _mode;
};

class StatusCommand final : public ICommand
{
public:
    std::string Name()        const override { return "STATUS"; }
    std::string Usage()       const override { return "status"; }
    std::string Description() const override { return "Ask the Arduino for its current state"; }

    CommandResult Execute(const std::vector<std::string>& args,
                          CommandContext& context) override;
};

class SendCommand final : public ICommand
{
public:
    std::string Name()        const override { return "SEND"; }
    std::string Usage()       const override { return "send <raw text>"; }
    std::string Description() const override { return "Send a raw line to the Arduino"; }

    CommandResult Execute(const std::vector<std::string>& args,
                          CommandContext& context) override;
};

class SerialCommand final : public ICommand
{
public:
    std::string Name()        const override { return "SERIAL"; }
    std::string Usage()       const override
    { return "serial [info|open|close|port <dev>|baud <n>]"; }
    std::string Description() const override { return "Inspect and control the serial link"; }

    CommandResult Execute(const std::vector<std::string>& args,
                          CommandContext& context) override;
};

/// Registers every command above into the registry.
void RegisterAll(CommandRegistry& registry);

} // namespace app::commands