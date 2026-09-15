#include "app/Shell.hpp"
#include "io/SerialPort.hpp"

#include <iostream>

namespace app {

Shell::Shell(CommandRegistry&   registry,
             CommandContext&   context,
             ui::PanelRenderer& renderer,
             AsyncBus&         bus,
             std::string       prompt)
    : _registry(registry)
    , _context(context)
    , _renderer(renderer)
    , _bus(bus)
    , _prompt(std::move(prompt))
{
}

void Shell::DrainAsync()
{
    // Unsolicited bytes coming from the Arduino become panel messages.
    if (_context.serial.IsOpen())
    {
        for (auto& line : _context.serial.PollLines())
            _bus.Push(AsyncMessage{ "[arduino]", std::move(line),
                                    ui::Color::BrightYellow });
    }

    for (const auto& message : _bus.Drain())
        _renderer.RenderAsync(message.tag, message.text, message.color);
}

CommandResult Shell::Dispatch(const std::vector<std::string>& tokens)
{
    if (tokens.empty())
        return CommandResult();

    ICommand* command = _registry.Find(tokens[0]);

    if (command == nullptr)
    {
        CommandResult result;
        result.Left("[dispatch] unknown token", ui::Color::BrightRed);
        result.Right("ERR: UNKNOWN_CMD: " + tokens[0], ui::Color::BrightRed);
        result.Right("try: help", ui::Color::BrightBlack);
        return result;
    }

    const std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    return command->Execute(args, _context);
}

int Shell::Run()
{
    _renderer.Banner("W H O   S H E L L",
                     "Arduino Nano 4-LED Serial Controller  |  type 'help' for commands");

    DrainAsync();

    std::string line;

    while (_running)
    {
        DrainAsync();

        std::cout << ui::Terminal::Paint(_prompt, ui::Color::BrightCyan) << std::flush;

        if (!std::getline(std::cin, line))
        {
            std::cout << '\n';
            break;                                  // EOF / Ctrl-D
        }

        const std::string trimmed = CommandParser::Trim(line);
        if (trimmed.empty())
        {
            std::cout << '\n';
            continue;
        }

        const std::string echo = _prompt + line;

        _context.state.AddHistory(trimmed);

        const auto tokens = CommandParser::Tokenize(trimmed);
        CommandResult result = Dispatch(tokens);

        if (result.ClearRequested())
        {
            _renderer.Clear();
            _renderer.Banner("W H O   S H E L L", "screen cleared");
        }
        else
        {
            _renderer.Render(echo, result);
        }

        if (result.ExitRequested())
            break;
    }

    return 0;
}

} // namespace app