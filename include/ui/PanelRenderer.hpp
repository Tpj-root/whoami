#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "app/CommandResult.hpp"
#include "ui/Terminal.hpp"

namespace app::ui {

/// Draws the two column "who> ... | output" layout.
class PanelRenderer
{
public:
    explicit PanelRenderer(int leftWidth = 34);

    void SetLeftWidth(int width) noexcept { _leftWidth = width; }
    int  LeftWidth() const noexcept { return _leftWidth; }

    /// Renders one command/response block.
    void Render(const std::string& promptEcho, const CommandResult& result);

    /// Renders an out-of-band message (serial data, log record, ...).
    void RenderAsync(const std::string& tag, const std::string& text, Color color);

    void Banner(const std::string& title, const std::string& subtitle) const;
    void Clear() const;

    static std::vector<std::string> Wrap(std::string_view text, std::size_t width);

private:
    int EffectiveLeftWidth() const noexcept;
    static std::string Pad(std::size_t visible, int width);

    int _leftWidth;
};

} // namespace app::ui