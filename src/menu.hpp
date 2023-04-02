#pragma once

#include "raylib.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "rlImGui.h"
#include "view.hpp"
#include "objects.hpp"
#include "err_logger.hpp"
#include <string>
#include <vector>
#include <ios>


// FD
class AppWindow;


class ImGuiInfoWindow: public MenuWindow {
private:
    const std::string msg;
    const std::string name;

public:
    ImGuiInfoWindow(const std::string& n, const std::string& txt);

    void update(float) override;
    void draw() override;
};


class CreateObjectWindow: public MenuWindow {
private:
    ObjectStorage* storage = nullptr;

    std::string current_name = "";

    // This can be organized better
    bool has_icon = false;
    std::string icon_path = "";

    bool has_speed = false;
    float current_speed = 0.0f;
    float min_speed = 0.0f;
    float max_speed = 0.0f;

    bool has_material = false;
    std::vector<std::string> material_choices = {};
    int current_choice = 0;
    std::string choices_string = "";
    std::string current_mat = "";

    bool has_points = false;
    int points = 0;

    bool has_errors = false;

public:
    CreateObjectWindow(ObjectStorage* st);

    void update(float) override;
    void draw() override;
};


class FileActionWindow: public MenuWindow {
private:
    const std::string name;
    std::string current;

protected:
    const std::string action_name;
    ObjectStorage* storage = nullptr;

public:
    FileActionWindow(const std::string n, const std::string an, ObjectStorage* s);

    void update(float) override;
    virtual bool perform_action(const std::string &) = 0;
    void draw() override;
};


class LoadFileWindow: public FileActionWindow {
public:
    LoadFileWindow(ObjectStorage* s);

    bool perform_action(const std::string& fp) override;
};

class SaveFileWindow: public FileActionWindow {
public:
    SaveFileWindow(ObjectStorage* s);

    bool perform_action(const std::string& fp) override;
};


class MenuStorage {
protected:
    std::vector<MenuWindow*> submenus;

public:
    void add(MenuWindow* w);
    void remove_dead();
    void draw();
};

class ImGuiMenu {
private:
    std::string read_fp;
    AppWindow* window;
    MenuStorage submenus;
    ObjectStorage storage;

public:
    ImGuiMenu(AppWindow* w);

    void add_submenu(MenuWindow* w);

    // Non-draw update logic there
    void update(float dt);

    // Update logic happend in draw cycle coz its an immediate-mode gui
    void draw();
};
