#pragma once

#include "raylib.h"
#include "spdlog/spdlog.h"
#include "imgui.h"
#include "rlImGui.h"
#include "menu.hpp"
#include <vector>
#include <string>


class AppWindow {
protected:
    bool initialized = false;
    bool active = false;

    ImGuiMenu* menu = nullptr;

    std::vector<std::string> exception_messages = {};

public:
    AppWindow();
    ~AppWindow();

    void init();
    bool is_active();
    void run();
    void quit();
};
