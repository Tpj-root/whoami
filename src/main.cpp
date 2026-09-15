#include "app/Application.hpp"

#include <exception>
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        auto options = app::Application::ParseCommandLine(argc, argv);
        app::Application application(std::move(options));
        return application.Run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "fatal: " << ex.what() << '\n';
        return 1;
    }
}