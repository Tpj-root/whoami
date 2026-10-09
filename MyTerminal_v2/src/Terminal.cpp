#include "Terminal.hpp"

#include <iostream>
#include <string>

namespace Color
{
    constexpr const char* reset  = "\033[0m";
    constexpr const char* cyan   = "\033[36m";
    constexpr const char* green  = "\033[32m";
    constexpr const char* yellow = "\033[33m";
    constexpr const char* red    = "\033[31m";
    constexpr const char* white  = "\033[97m";
    constexpr const char* blue   = "\033[34m";
    constexpr const char* gray   = "\033[90m";
}


// --------------------------------------------------
// Color printing helper
// --------------------------------------------------

void print(const char* color, const std::string& text)
{
    std::cout << color
              << text
              << Color::reset;
}


// --------------------------------------------------
// Terminal
// --------------------------------------------------

Terminal::Terminal()
{
}


// --------------------------------------------------
// Header
// --------------------------------------------------

void Terminal::showHeader()
{
    std::cout << '\n';

    print(Color::cyan,
        "╔══════════════════════════════════════════════════════════════╗");

    std::cout << '\n';

    print(Color::cyan, "║");

    print(Color::white,
        "                       MY TERMINAL v0.1                       ");

    print(Color::cyan, "║");

    std::cout << '\n';

    print(Color::cyan,
        "╠══════════════════════════════════════════════════════════════╣");

    std::cout << '\n';

    print(Color::cyan, "║");

    print(Color::green,
        "  ● SYSTEM READY                                             ");

    print(Color::cyan, "║");

    std::cout << '\n';

    print(Color::cyan,
        "╚══════════════════════════════════════════════════════════════╝");

    std::cout << "\n\n";
}


// --------------------------------------------------
// Help
// --------------------------------------------------

void Terminal::showHelp()
{
    std::cout << '\n';

    print(Color::cyan,
        "╔══════════════════════════════════════════════════════════════╗");

    std::cout << '\n';

    print(Color::cyan, "║");

    print(Color::white,
        "                       COMMANDS                               ");

    print(Color::cyan, "║");

    std::cout << '\n';

    print(Color::cyan,
        "╠══════════════════════════════════════════════════════════════╣");

    std::cout << '\n';

    print(Color::cyan, "║  ");

    print(Color::yellow, "help");

    std::cout << "       Show this help\n";

    print(Color::cyan, "║  ");

    print(Color::yellow, "about");

    std::cout << "      About this terminal\n";

    print(Color::cyan, "║  ");

    print(Color::yellow, "status");

    std::cout << "     Show system status\n";

    print(Color::cyan, "║  ");

    print(Color::yellow, "clear");

    std::cout << "      Clear screen\n";

    print(Color::cyan, "║  ");

    print(Color::red, "exit");

    std::cout << "       Exit terminal\n";

    print(Color::cyan,
        "╚══════════════════════════════════════════════════════════════╝");

    std::cout << "\n\n";
}


// --------------------------------------------------
// About
// --------------------------------------------------

void Terminal::showAbout()
{
    std::cout << '\n';

    print(Color::cyan,
        "╔══════════════════════════════════════════════════════════════╗");

    std::cout << '\n';

    print(Color::cyan, "║  ");

    print(Color::white, "MY TERMINAL");

    std::cout << '\n';

    print(Color::cyan, "║  ");

    print(Color::green, "C++ OOP CLI Project");

    std::cout << '\n';

    print(Color::cyan, "║  Built with C++ and CMake");

    std::cout << '\n';

    print(Color::cyan, "║  ANSI terminal colors");

    std::cout << '\n';

    print(Color::cyan,
        "╚══════════════════════════════════════════════════════════════╝");

    std::cout << "\n\n";
}


// --------------------------------------------------
// Status
// --------------------------------------------------

void Terminal::showStatus()
{
    std::cout << '\n';

    print(Color::cyan,
        "╔══════════════════════════════════════════════════════════════╗");

    std::cout << '\n';

    print(Color::cyan, "║  ");

    print(Color::green, "● ONLINE");

    std::cout << '\n';

    print(Color::cyan, "║  Terminal : ");

    print(Color::yellow, "MY TERMINAL");

    std::cout << '\n';

    print(Color::cyan, "║  Version  : ");

    print(Color::yellow, "0.1");

    std::cout << '\n';

    print(Color::cyan, "║  Engine   : ");

    print(Color::yellow, "C++ OOP");

    std::cout << '\n';

    print(Color::cyan,
        "╚══════════════════════════════════════════════════════════════╝");

    std::cout << "\n\n";
}


// --------------------------------------------------
// Command processing
// --------------------------------------------------

void Terminal::processCommand(const std::string& command)
{
    if (command == "help")
    {
        showHelp();
    }
    else if (command == "about")
    {
        showAbout();
    }
    else if (command == "status")
    {
        showStatus();
    }
    else if (command == "clear")
    {
        std::cout << "\033[2J\033[H";

        showHeader();
    }
    else if (command == "exit")
    {
        print(Color::green,
              "\nGoodbye. See you again.\n");
    }
    else if (!command.empty())
    {
        print(Color::red,
              "\n✖ Unknown command: ");

        std::cout << command << '\n';

        print(Color::gray,
              "  Type 'help' to see available commands.\n\n");
    }
}


// --------------------------------------------------
// Start
// --------------------------------------------------

void Terminal::start()
{
    showHeader();

    std::string command;

    while (true)
    {
        print(Color::blue, "myterm");
        print(Color::white, "> ");

        std::getline(std::cin, command);

        if (command == "exit")
        {
            processCommand(command);
            break;
        }

        processCommand(command);
    }
}