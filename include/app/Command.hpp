#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "app/AppState.hpp"
#include "app/CommandResult.hpp"

namespace app {

namespace io { class SerialPort; }

/// Everything a command implementation is allowed to touch.
struct CommandContext
{
    io::SerialPort& serial;
    AppState&       state;
};

/// Abstract command.
class ICommand
{
public:
    virtual ~ICommand() = default;

    virtual std::string              Name()        const = 0;
    virtual std::vector<std::string> Aliases()     const { return {}; }
    virtual std::string              Usage()       const = 0;
    virtual std::string              Description() const = 0;

    virtual CommandResult Execute(const std::vector<std::string>& args,
                                  CommandContext& context) = 0;
};

using CommandPtr = std::shared_ptr<ICommand>;

/// Name/alias -> command lookup table.
class CommandRegistry
{
public:
    void Register(CommandPtr command);

    ICommand* Find(const std::string& name) const;
    std::vector<CommandPtr> All() const { return _commands; }

private:
    std::vector<CommandPtr>                    _commands;
    std::unordered_map<std::string, ICommand*> _index;
};

/// Small helpers used by the shell and the commands.
class CommandParser
{
public:
    static std::vector<std::string> Tokenize(const std::string& line);
    static std::string Join(const std::vector<std::string>& tokens, std::size_t from);
    static std::string Trim(const std::string& value);
    static std::string ToUpper(const std::string& value);
};

} // namespace app