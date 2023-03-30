#pragma once

#include "raylib.h"
#include "spdlog/spdlog.h"
#include "imgui.h"
#include "rlImGui.h"
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


class AppWindow {
protected:
    bool initialized = false;
    bool active = false;

public:
    AppWindow() {
        SetTraceLogCallback(TraceLog);
        spdlog::set_pattern("[%H:%M:%S][%l] %v");
    }

    void init() {
        if (initialized) {
            return;
        }

        InitWindow(1280, 720, "Editor");
        SetTargetFPS(60);
        rlImGuiSetup(true);
	    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
        // SetExitKey(KEY_NULL);

        InitAudioDevice();

        initialized = true;
    }

    bool is_active() {
        return active && !WindowShouldClose();
    }

    void run() {
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

            BeginDrawing();
            ClearBackground(DARKGRAY);

            rlImGuiBegin();
            // Imgui logic there, since its done in draw cycle

            rlImGuiEnd();
            EndDrawing();
        }
        rlImGuiShutdown();

        CloseWindow();
    }

    void quit() {
        active = false;
    }
};
