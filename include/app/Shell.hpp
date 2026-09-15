#pragma once

#include <string>
#include <vector>

#include "app/AsyncBus.hpp"
#include "app/Command.hpp"
#include "ui/PanelRenderer.hpp"

namespace app {

/// The interactive read-eval-print loop.
class Shell
{
public:
    Shell(CommandRegistry&   registry,
          CommandContext&   context,
          ui::PanelRenderer& renderer,
          AsyncBus&         bus,
          std::string       prompt = "who> ");

    int Run();

    void Stop() noexcept { _running = false; }

    AsyncBus&          Bus()    noexcept { return _bus; }
    const std::string& Prompt() const noexcept { return _prompt; }

private:
    void          DrainAsync();
    CommandResult Dispatch(const std::vector<std::string>& tokens);

    CommandRegistry&   _registry;
    CommandContext&    _context;
    ui::PanelRenderer& _renderer;
    AsyncBus&          _bus;
    std::string        _prompt;
    bool               _running = true;
};

} // namespace app