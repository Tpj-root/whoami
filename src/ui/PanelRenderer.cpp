#include "ui/PanelRenderer.hpp"

#include <algorithm>
#include <iostream>

namespace app::ui {

namespace {
constexpr int kMinLeftWidth    = 14;
constexpr int kMinRightWidth   = 16;
constexpr std::size_t kGutter  = 2;   // "| "
}

PanelRenderer::PanelRenderer(int leftWidth)
    : _leftWidth(leftWidth)
{
}

int PanelRenderer::EffectiveLeftWidth() const noexcept
{
    const int terminalWidth = Terminal::Width();
    const int maxAllowed    = std::max(kMinLeftWidth, terminalWidth / 2);

    int width = _leftWidth;
    if (width > maxAllowed) width = maxAllowed;
    if (width < kMinLeftWidth) width = kMinLeftWidth;
    return width;
}

std::string PanelRenderer::Pad(std::size_t visible, int width)
{
    if (width <= 0)
        return {};

    if (visible + 1 >= static_cast<std::size_t>(width))
        return std::string(1, ' ');

    return std::string(static_cast<std::size_t>(width) - visible, ' ');
}

std::vector<std::string> PanelRenderer::Wrap(std::string_view text, std::size_t width)
{
    std::vector<std::string> lines;

    if (width == 0)
        width = 1;

    std::string_view rest = text;

    for (;;)
    {
        if (rest.size() <= width)
        {
            lines.emplace_back(rest);
            break;
        }

        std::size_t cut = rest.rfind(' ', width);
        if (cut == std::string_view::npos || cut == 0)
            cut = width;

        lines.emplace_back(rest.substr(0, cut));
        rest.remove_prefix(cut);

        while (!rest.empty() && rest.front() == ' ')
            rest.remove_prefix(1);

        if (rest.empty())
            break;
    }

    if (lines.empty())
        lines.emplace_back();

    return lines;
}

void PanelRenderer::Render(const std::string& promptEcho, const CommandResult& result)
{
    const int leftWidth  = EffectiveLeftWidth();
    const int terminal   = Terminal::Width();
    const int rightWidth = std::max(kMinRightWidth,
                                    terminal - leftWidth - static_cast<int>(kGutter));

    // ---------------------------------------------------------------- right
    std::vector<std::string> right;
    for (const auto& line : result.RightLines())
    {
        auto pieces = Wrap(line.text, static_cast<std::size_t>(rightWidth));
        for (const auto& piece : pieces)
            right.push_back(Terminal::Paint(piece, line.color));
    }

    // ------------------------------------------------- left (rows 1 .. n)
    std::vector<std::string> leftColoured;
    std::vector<std::size_t> leftVisible;

    for (const auto& line : result.LeftLines())
    {
        leftColoured.push_back(Terminal::Paint(line.text, line.color));
        leftVisible.push_back(Terminal::VisibleLength(line.text));
    }

    const std::size_t rows = std::max<std::size_t>(
        1, std::max(leftColoured.size() + 1, right.size()));

    // ------------------------------------------------------------ row zero
    // The terminal has already echoed "who> <command>", the cursor is at the
    // end of that line, so we simply continue it.
    const std::size_t used = Terminal::VisibleLength(promptEcho);

    std::cout << Pad(used, leftWidth)
              << Terminal::Paint("| ", Color::BrightBlack)
              << (right.empty() ? std::string() : right[0])
              << '\n';

    // ---------------------------------------------------------- rows 1 .. n
    for (std::size_t row = 1; row < rows; ++row)
    {
        const bool hasLeft = (row - 1) < leftColoured.size();

        std::cout << (hasLeft ? leftColoured[row - 1] : std::string())
                  << Pad(hasLeft ? leftVisible[row - 1] : 0, leftWidth)
                  << Terminal::Paint("| ", Color::BrightBlack)
                  << (row < right.size() ? right[row] : std::string())
                  << '\n';
    }

    std::cout.flush();
}

void PanelRenderer::RenderAsync(const std::string& tag,
                                const std::string& text,
                                Color color)
{
    const int leftWidth  = EffectiveLeftWidth();
    const int terminal   = Terminal::Width();
    const int rightWidth = std::max(kMinRightWidth,
                                    terminal - leftWidth - static_cast<int>(kGutter));

    auto pieces = Wrap(text, static_cast<std::size_t>(rightWidth));

    for (std::size_t i = 0; i < pieces.size(); ++i)
    {
        if (i == 0)
        {
            std::cout << Terminal::Paint(tag, Color::BrightMagenta)
                      << Pad(Terminal::VisibleLength(tag), leftWidth)
                      << Terminal::Paint("| ", Color::BrightBlack)
                      << Terminal::Paint(pieces[i], color)
                      << '\n';
        }
        else
        {
            std::cout << std::string(static_cast<std::size_t>(leftWidth), ' ')
                      << Terminal::Paint("| ", Color::BrightBlack)
                      << Terminal::Paint(pieces[i], color)
                      << '\n';
        }
    }

    std::cout.flush();
}

void PanelRenderer::Banner(const std::string& title, const std::string& subtitle) const
{
    const int width = std::max(40, Terminal::Width() - 1);
    const std::string rule(static_cast<std::size_t>(width), '-');

    std::cout << Terminal::Paint(rule, Color::BrightBlack) << '\n'
              << "  " << Terminal::Paint(title,    Color::BrightCyan) << '\n'
              << "  " << Terminal::Paint(subtitle, Color::BrightBlack) << '\n'
              << Terminal::Paint(rule, Color::BrightBlack) << '\n';
    std::cout.flush();
}

void PanelRenderer::Clear() const
{
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

} // namespace app::ui