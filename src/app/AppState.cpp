#include "app/AppState.hpp"

namespace app {

const char* AppState::ModeName(Mode mode) noexcept
{
    switch (mode)
    {
    case Mode::Unknown: return "UNKNOWN";
    case Mode::Buy:     return "BUY";
    case Mode::Sell:    return "SELL";
    case Mode::Off:     return "OFF";
    case Mode::AllOn:   return "ALL_ON";
    }
    return "UNKNOWN";
}

void AppState::AddHistory(const std::string& entry)
{
    std::lock_guard<std::mutex> lock(_historyMutex);
    if (!_history.empty() && _history.back() == entry)
        return;                            // collapse immediate duplicates
    _history.push_back(entry);
}

std::vector<std::string> AppState::History() const
{
    std::lock_guard<std::mutex> lock(_historyMutex);
    return _history;
}

} // namespace app