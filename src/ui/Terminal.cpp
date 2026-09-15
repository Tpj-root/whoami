#include "ui/Terminal.hpp"

#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdlib>

namespace app::ui {

const char* Terminal::Code(Color color) noexcept
{
    switch (color)
    {
    case Color::Default:       return RESET;
    case Color::Black:         return BLACK;
    case Color::Red:           return RED;
    case Color::Green:         return GREEN;
    case Color::Yellow:        return YELLOW;
    case Color::Blue:          return BLUE;
    case Color::Magenta:       return MAGENTA;
    case Color::Cyan:          return CYAN;
    case Color::White:         return WHITE;
    case Color::BrightBlack:   return BRIGHT_BLACK;
    case Color::BrightRed:     return BRIGHT_RED;
    case Color::BrightGreen:   return BRIGHT_GREEN;
    case Color::BrightYellow:  return BRIGHT_YELLOW;
    case Color::BrightBlue:    return BRIGHT_BLUE;
    case Color::BrightMagenta: return BRIGHT_MAGENTA;
    case Color::BrightCyan:    return BRIGHT_CYAN;
    case Color::BrightWhite:   return BRIGHT_WHITE;
    }
    return RESET;
}

std::string Terminal::Paint(std::string_view text, Color color)
{
    if (color == Color::Default)
        return std::string(text);

    std::string out;
    out.reserve(text.size() + 16);
    out += Code(color);
    out += text;
    out += RESET;
    return out;
}

std::size_t Terminal::VisibleLength(std::string_view text) noexcept
{
    std::size_t length = 0;

    for (std::size_t i = 0; i < text.size(); )
    {
        const unsigned char ch = static_cast<unsigned char>(text[i]);

        if (ch == 0x1B)                       // ESC -> skip the whole CSI sequence
        {
            ++i;
            if (i < text.size() && text[i] == '[')
            {
                ++i;
                while (i < text.size() && !(text[i] >= '@' && text[i] <= '~'))
                    ++i;
                if (i < text.size())
                    ++i;
            }
            continue;
        }

        if ((ch & 0xC0) != 0x80)              // count only UTF-8 lead bytes
            ++length;

        ++i;
    }

    return length;
}

int Terminal::Width() noexcept
{
    struct winsize ws {};
    if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        return static_cast<int>(ws.ws_col);

    if (const char* columns = std::getenv("COLUMNS"))
    {
        const int value = std::atoi(columns);
        if (value > 0)
            return value;
    }

    return 120;
}

int Terminal::Height() noexcept
{
    struct winsize ws {};
    if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_row > 0)
        return static_cast<int>(ws.ws_row);

    if (const char* lines = std::getenv("LINES"))
    {
        const int value = std::atoi(lines);
        if (value > 0)
            return value;
    }

    return 40;
}

} // namespace app::ui