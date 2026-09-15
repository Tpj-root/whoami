#pragma once

#include <string>
#include <utility>
#include <vector>

#include "ui/Terminal.hpp"

namespace app {

/// One cell of the two column layout.
struct PanelLine
{
    std::string text;
    ui::Color   color = ui::Color::Default;

    PanelLine() = default;
    PanelLine(std::string value, ui::Color tint)
        : text(std::move(value)), color(tint) {}
};

/// Everything a command wants to show.
///
///   left  -> extra rows of the *left* column  (rows 1..n)
///   right -> rows of the *right* column       (rows 0..n, row 0 shares
///            the line with the prompt that the user already typed)
class CommandResult
{
public:
    CommandResult() = default;

    CommandResult& Left(std::string text, ui::Color color = ui::Color::Default)
    {
        _left.emplace_back(std::move(text), color);
        return *this;
    }

    CommandResult& Right(std::string text, ui::Color color = ui::Color::Default)
    {
        _right.emplace_back(std::move(text), color);
        return *this;
    }

    CommandResult& Exit(bool value = true)  { _exit  = value; return *this; }
    CommandResult& Clear(bool value = true) { _clear = value; return *this; }

    const std::vector<PanelLine>& LeftLines()  const noexcept { return _left;  }
    const std::vector<PanelLine>& RightLines() const noexcept { return _right; }

    bool ExitRequested()  const noexcept { return _exit;  }
    bool ClearRequested() const noexcept { return _clear; }

    static CommandResult Ok(std::string text)
    {
        CommandResult result;
        result.Right(std::move(text), ui::Color::BrightGreen);
        return result;
    }

    static CommandResult Error(std::string text)
    {
        CommandResult result;
        result.Right(std::move(text), ui::Color::BrightRed);
        return result;
    }

private:
    std::vector<PanelLine> _left;
    std::vector<PanelLine> _right;
    bool _exit  = false;
    bool _clear = false;
};

} // namespace app