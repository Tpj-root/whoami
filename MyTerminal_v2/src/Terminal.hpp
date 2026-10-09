#pragma once

#include <string>

class Terminal
{
public:
    Terminal();

    void start();

private:
    void showHeader();
    void showHelp();
    void showAbout();
    void showStatus();

    void processCommand(const std::string& command);
};