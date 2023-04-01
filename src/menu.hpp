#pragma once

#include "raylib.h"
#include "imgui.h"
#include <string>
#include <vector>


#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"

#include "view.hpp"
#include "objects.hpp"

// FD
class AppWindow;


// class MenuWindow {
// public:
// 	bool is_open = true;
//     virtual void draw() = 0;

//     virtual ~MenuWindow() = default;
// };


class ImGuiInfoWindow: public MenuWindow {
private:
    const std::string msg;
    const std::string name;

public:
    ImGuiInfoWindow(const std::string& n, const std::string& txt);

    void update(float) override {
        if (!is_open) {
            must_die = true;
        }
    }

    void draw() override;
};


// class ImGuiEditorObjectWindow: public MenuWindow {
// private:
//     EditorObject* parent;

// public:
//     ImGuiEditorObjectWindow(EditorObject* p): parent(p) {}

//     void draw() override {
//         if (!is_open) {
//             return;
//         }

//         if (ImGui::Begin(parent->get_name().c_str(), &is_open, ImGuiWindowFlags_None)) {
//             ImGui::Text("%s", parent->to_string().c_str());
//             ImGui::PopStyleVar();
//         }
//         ImGui::End();
//     }
// };


class MenuStorage {
protected:
    std::vector<MenuWindow*> submenus;

public:
    void add(MenuWindow* w) {
        if (std::find(submenus.begin(), submenus.end(), w) != submenus.end()) {
            return;
        }
        else {
            submenus.push_back(w);
        }
    }

    void remove_dead() {
        // cleanup
        submenus.erase(
            std::remove_if(
                submenus.begin(),
                submenus.end(),
                [](MenuWindow* i){ return (i->must_die); }),
            submenus.end()
        );
    }

    // virtual void update(float) {
    //     // cleanup
    //     submenus.erase(
    //         std::remove_if(
    //             submenus.begin(),
    //             submenus.end(),
    //             [](MenuWindow* i){ return !(i->is_open); }),
    //         submenus.end()
    //     );
    // }

    void draw() {
        for (auto i: submenus) {
            if (i->is_open) {
                i->draw();
            }
        }
    }
};

class ImGuiMenu {
private:
    std::string read_fp;
    AppWindow* window;

    MenuStorage submenus;

    ObjectStorage storage;

public:
    ImGuiMenu(AppWindow* w);

    void add_submenu(MenuWindow* w) {
        submenus.add(w);
    }

    // Non-draw update logic there
    void update(float dt);

    // Update logic happend in draw cycle coz its an immediate-mode gui
    void draw();
};
