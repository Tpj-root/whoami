#include "app/Command.hpp"

#include <algorithm>
#include <cctype>

namespace app {

void CommandRegistry::Register(CommandPtr command)
{
    if (!command)
        return;

    _index[CommandParser::ToUpper(command->Name())] = command.get();

    for (const auto& alias : command->Aliases())
        _index[CommandParser::ToUpper(alias)] = command.get();

    _commands.push_back(std::move(command));

    std::sort(_commands.begin(), _commands.end(),
              [](const CommandPtr& a, const CommandPtr& b)
              { return a->Name() < b->Name(); });
}

ICommand* CommandRegistry::Find(const std::string& name) const
{
    const auto it = _index.find(CommandParser::ToUpper(name));
    return it == _index.end() ? nullptr : it->second;
}

std::vector<std::string> CommandParser::Tokenize(const std::string& line)
{
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;
    bool escaped  = false;

    for (const char ch : line)
    {
        if (escaped)
        {
            current.push_back(ch);
            escaped = false;
            continue;
        }

        if (ch == '\\')
        {
            escaped = true;
            continue;
        }

        if (ch == '"')
        {
            inQuotes = !inQuotes;
            continue;
        }

        if (!inQuotes && std::isspace(static_cast<unsigned char>(ch)))
        {
            if (!current.empty())
            {
                tokens.push_back(current);
                current.clear();
            }
            continue;
        }

        current.push_back(ch);
    }

    if (!current.empty())
        tokens.push_back(current);

    return tokens;
}

std::string CommandParser::Join(const std::vector<std::string>& tokens, std::size_t from)
{
    std::string out;
    for (std::size_t i = from; i < tokens.size(); ++i)
    {
        if (!out.empty())
            out += ' ';
        out += tokens[i];
    }
    return out;
}

std::string CommandParser::Trim(const std::string& value)
{
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos)
        return {};

    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

std::string CommandParser::ToUpper(const std::string& value)
{
    std::string out = value;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return out;
}

} // namespace app