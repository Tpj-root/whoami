#pragma once

#include <string>
#include <string_view>

// ---------------------------------------------------------------------------
//  ColoredTextConsole  (https://github.com/dizzpy/ColoredTextConsole)
//  ships a header called "colored_text.h" that defines plain ANSI SGR macros.
//  If the header is not on the include path we fall back to identical codes
//  so that the project still compiles.
// ---------------------------------------------------------------------------
#if defined(__has_include)
#  if __has_include("colored_text.h")
#    include "colored_text.h"
#    define APP_HAVE_COLORED_TEXT 1
#  endif
#endif

#ifndef APP_HAVE_COLORED_TEXT
#  define APP_HAVE_COLORED_TEXT 0
#endif

#ifndef RESET
#  define RESET          "\033[0m"
#endif
#ifndef BLACK
#  define BLACK          "\033[0;30m"
#endif
#ifndef RED
#  define RED            "\033[0;31m"
#endif
#ifndef GREEN
#  define GREEN          "\033[0;32m"
#endif
#ifndef YELLOW
#  define YELLOW         "\033[0;33m"
#endif
#ifndef BLUE
#  define BLUE           "\033[0;34m"
#endif
#ifndef MAGENTA
#  define MAGENTA        "\033[0;35m"
#endif
#ifndef CYAN
#  define CYAN           "\033[0;36m"
#endif
#ifndef WHITE
#  define WHITE          "\033[0;37m"
#endif
#ifndef BRIGHT_BLACK
#  define BRIGHT_BLACK   "\033[1;30m"
#endif
#ifndef BRIGHT_RED
#  define BRIGHT_RED     "\033[1;31m"
#endif
#ifndef BRIGHT_GREEN
#  define BRIGHT_GREEN   "\033[1;32m"
#endif
#ifndef BRIGHT_YELLOW
#  define BRIGHT_YELLOW  "\033[1;33m"
#endif
#ifndef BRIGHT_BLUE
#  define BRIGHT_BLUE    "\033[1;34m"
#endif
#ifndef BRIGHT_MAGENTA
#  define BRIGHT_MAGENTA "\033[1;35m"
#endif
#ifndef BRIGHT_CYAN
#  define BRIGHT_CYAN    "\033[1;36m"
#endif
#ifndef BRIGHT_WHITE
#  define BRIGHT_WHITE   "\033[1;37m"
#endif

namespace app::ui {

enum class Color
{
    Default,
    Black,  Red,     Green,  Yellow, Blue, Magenta, Cyan, White,
    BrightBlack, BrightRed, BrightGreen, BrightYellow,
    BrightBlue,  BrightMagenta, BrightCyan, BrightWhite
};

class Terminal
{
public:
    static const char* Code(Color color) noexcept;

    static std::string Paint(std::string_view text, Color color);

    /// Number of printable characters (ANSI sequences are ignored, UTF-8 is counted once).
    static std::size_t VisibleLength(std::string_view text) noexcept;

    static int Width()  noexcept;
    static int Height() noexcept;
};

} // namespace app::ui