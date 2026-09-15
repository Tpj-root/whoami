#pragma once

#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "ui/Terminal.hpp"

namespace app {

/// A message that appears in the right column *outside* of a command
/// (unsolicited Arduino output, log records, ...).
struct AsyncMessage
{
    std::string tag;
    std::string text;
    ui::Color   color = ui::Color::Default;
};

/// Thread safe mailbox between background producers (logger sinks, serial
/// polling) and the single consumer (the shell prompt).
class AsyncBus
{
public:
    void Push(AsyncMessage message)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _messages.push_back(std::move(message));
    }

    std::vector<AsyncMessage> Drain()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        std::vector<AsyncMessage> out;
        out.swap(_messages);
        return out;
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _messages.empty();
    }

private:
    mutable std::mutex        _mutex;
    std::vector<AsyncMessage> _messages;
};

} // namespace app