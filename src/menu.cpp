#include "menu.hpp"
#include "window.hpp"
#include "spdlog/spdlog.h"
#include "imgui.h"


ImGuiInfoWindow::ImGuiInfoWindow(const std::string& n, const std::string& txt)
    : msg(txt)
    , name(n) {}

void ImGuiInfoWindow::draw() {
    if (!is_open) {
        return;
    }

    if (ImGui::Begin(name.c_str(), &is_open, ImGuiWindowFlags_None)) {
        ImGui::Text("%s", msg.c_str());
    }
    ImGui::End();
}


ImGuiMenu::ImGuiMenu(AppWindow* w): window(w), storage(this) {
}


void ImGuiMenu::update(float) {
    submenus.remove_dead();

    for (auto i: ExceptionLogger::get_logger().get_messages()) {
        submenus.add(new ImGuiInfoWindow("Error", i));
    }
    ExceptionLogger::get_logger().clear();
}


void ImGuiMenu::draw() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load")) {
                add_submenu(new LoadFileWindow(&storage));
            }

            if (ImGui::MenuItem("Save")) {
                add_submenu(new SaveFileWindow(&storage));
            }

            if (ImGui::MenuItem("Exit"))
                window->quit();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Props")) {
            storage.draw();

            ImGui::MenuItem("------");

            if (ImGui::MenuItem("Create New")) {
                submenus.add(new CreateObjectWindow(&storage));
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    submenus.draw();
}
