#pragma once

#include "raylib.h"
#include "imgui.h"
#include <string>
#include <vector>


#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"

#include "objects.hpp"

// FD
class AppWindow;


class MenuWindow {
public:
	bool is_open = true;
    virtual void draw() = 0;
};


class ImGuiInfoWindow: public MenuWindow {
private:
    const std::string msg;
    const std::string name;

public:
    ImGuiInfoWindow(const std::string& n, const std::string& txt);

    void draw() override;
};


class ImGuiMenu {
private:
    std::string read_fp;
    AppWindow* window;

    std::vector<MenuWindow*> submenus;

    ObjectStorage storage;

public:
    ImGuiMenu(AppWindow* w);

    // Non-draw update logic there
    void update(float dt);

    // Update logic happend in draw cycle coz its an immediate-mode gui
    void draw();
};
