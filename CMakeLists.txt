cmake_minimum_required(VERSION 3.20)

project(WhoShell
    VERSION     1.0.0
    DESCRIPTION "Two column 'who>' shell for the Arduino Nano 4-LED serial controller"
    LANGUAGES   CXX
)

# ---------------------------------------------------------------------------
# C++ configuration
# ---------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------

option(
    APP_WITH_CPPLOGGING
    "Use chronoxor/CppLogging for persistent logging"
    OFF
)

option(
    APP_WITH_COLORED_TEXT
    "Fetch dizzpy/ColoredTextConsole for ANSI colours"
    OFF
)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

find_package(Threads REQUIRED)

include(FetchContent)

# ---------------------------------------------------------------------------
# fmt
#
# https://github.com/fmtlib/fmt
# ---------------------------------------------------------------------------

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        12.0.0
)

FetchContent_MakeAvailable(fmt)

# ---------------------------------------------------------------------------
# ColoredTextConsole
#
# Repository:
# https://github.com/dizzpy/ColoredTextConsole
#
# No release tag is required.
# We use the main branch.
# ---------------------------------------------------------------------------

if(APP_WITH_COLORED_TEXT)

    FetchContent_Declare(
        coloredtextconsole
        GIT_REPOSITORY https://github.com/dizzpy/ColoredTextConsole.git
        GIT_TAG        main
    )

    FetchContent_MakeAvailable(coloredtextconsole)

endif()

# ---------------------------------------------------------------------------
# CppCommon 1.0.6.0
#
# Release:
# https://github.com/chronoxor/CppCommon/releases/tag/1.0.6.0
#
# IMPORTANT:
# Do NOT use "master".
# ---------------------------------------------------------------------------

if(APP_WITH_CPPLOGGING)

    FetchContent_Declare(
        cppcommon
        GIT_REPOSITORY https://github.com/chronoxor/CppCommon.git
        GIT_TAG        1.0.6.0
    )

    FetchContent_MakeAvailable(cppcommon)

    # -----------------------------------------------------------------------
    # CppLogging 1.0.6.0
    #
    # Release:
    # https://github.com/chronoxor/CppLogging/releases/tag/1.0.6.0
    # -----------------------------------------------------------------------

    FetchContent_Declare(
        cpplogging
        GIT_REPOSITORY https://github.com/chronoxor/CppLogging.git
        GIT_TAG        1.0.6.0
    )

    FetchContent_MakeAvailable(cpplogging)

endif()

# ---------------------------------------------------------------------------
# Application sources
# ---------------------------------------------------------------------------

set(APP_SOURCES

    src/main.cpp

    # Application
    src/app/AppState.cpp
    src/app/Application.cpp
    src/app/Command.cpp
    src/app/Shell.cpp

    # Commands
    src/commands/BasicCommands.cpp
    src/commands/LedCommands.cpp
    src/commands/RegisterAll.cpp
    src/commands/SerialCommands.cpp

    # I/O
    src/io/SerialPort.cpp

    # Logging
    src/log/Log.cpp

    # UI
    src/ui/PanelRenderer.cpp
    src/ui/Terminal.cpp
)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------

add_executable(
    whoshell
    ${APP_SOURCES}
)

# ---------------------------------------------------------------------------
# Include directories
# ---------------------------------------------------------------------------

target_include_directories(
    whoshell
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ColoredTextConsole is header-only.
#
# The repository contains:
#
#     colored_text.h
#
# so we add the fetched source directory directly.
# ---------------------------------------------------------------------------

if(APP_WITH_COLORED_TEXT)

    target_include_directories(
        whoshell
        PRIVATE
            ${coloredtextconsole_SOURCE_DIR}
    )

endif()

# ---------------------------------------------------------------------------
# Base libraries
# ---------------------------------------------------------------------------

target_link_libraries(
    whoshell
    PRIVATE
        fmt::fmt
        Threads::Threads
)

# ---------------------------------------------------------------------------
# CppLogging
# ---------------------------------------------------------------------------

if(APP_WITH_CPPLOGGING)

    # CppLogging's CMake target name can vary depending on
    # the version/configuration, so detect the available target.

    if(TARGET cpplogging)

        set(_cpplogging_target cpplogging)

    elseif(TARGET logging)

        set(_cpplogging_target logging)

    elseif(TARGET CppLogging)

        set(_cpplogging_target CppLogging)

    else()

        message(
            FATAL_ERROR
            "CppLogging target was not created by FetchContent"
        )

    endif()

    target_link_libraries(
        whoshell
        PRIVATE
            ${_cpplogging_target}
    )

    target_compile_definitions(
        whoshell
        PRIVATE
            APP_WITH_CPPLOGGING=1
    )

    target_sources(
        whoshell
        PRIVATE
            src/log/CppLoggingBackend.cpp
    )

endif()

# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    target_compile_options(
        whoshell
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )

endif()

# ---------------------------------------------------------------------------
# Build information
# ---------------------------------------------------------------------------

message(STATUS "")
message(STATUS "========================================")
message(STATUS " WhoShell")
message(STATUS "========================================")
message(STATUS " Version       : ${PROJECT_VERSION}")
message(STATUS " Build type    : ${CMAKE_BUILD_TYPE}")
message(STATUS " C++ standard  : C++${CMAKE_CXX_STANDARD}")
message(STATUS " CppLogging    : ${APP_WITH_CPPLOGGING}")
message(STATUS " ColoredText   : ${APP_WITH_COLORED_TEXT}")
message(STATUS "========================================")
message(STATUS "")
