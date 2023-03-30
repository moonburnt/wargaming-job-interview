#include "raylib.h"
#include "spdlog/spdlog.h"
#include "imgui.h"
#include "rlImGui.h"
#include "window.hpp"
#include "menu.hpp"
#include <cstdio>



void TraceLog(int logLevel, const char* text, va_list args) {
    static char log_text[2048] = {0};
    std::vsprintf(log_text, text, args);

    switch (logLevel) {
    case LOG_TRACE:
        spdlog::trace(log_text);
        break;
    case LOG_DEBUG:
        spdlog::debug(log_text);
        break;
    case LOG_INFO:
        spdlog::info(log_text);
        break;
    case LOG_WARNING:
        spdlog::warn(log_text);
        break;
    case LOG_ERROR:
        spdlog::error(log_text);
        break;
    case LOG_FATAL:
        spdlog::critical(log_text);
        break;
    default:
        spdlog::info(log_text);
        break;
    }
}


AppWindow::AppWindow() {
    SetTraceLogCallback(TraceLog);
    spdlog::set_pattern("[%H:%M:%S][%l] %v");
}

AppWindow::~AppWindow() {
    if (menu != nullptr) {
        delete menu;
    }
}

void AppWindow::init() {
    if (initialized) {
        return;
    }

    InitWindow(1280, 720, "Editor");
    SetTargetFPS(60);
    rlImGuiSetup(true);
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    // SetExitKey(KEY_NULL);

    menu = new ImGuiMenu(this);

    InitAudioDevice();

    initialized = true;
}

bool AppWindow::is_active() {
    return active && !WindowShouldClose();
}

void AppWindow::run() {
    if (!initialized) {
        spdlog::critical("Attempting to run uninitialized Window.");
        abort();
    };

    if (active) {
        spdlog::warn("Window is already running.");
        return;
    }

    active = true;

    while (is_active()) {
        // Other update logic there
        menu->update(static_cast<float>(GetFrameTime()));

        BeginDrawing();
        ClearBackground(DARKGRAY);

        rlImGuiBegin();
        // Imgui logic there, since its done in draw cycle
        menu->draw();

        rlImGuiEnd();
        EndDrawing();
    }
    rlImGuiShutdown();

    CloseWindow();
}

void AppWindow::quit() {
    active = false;
}

