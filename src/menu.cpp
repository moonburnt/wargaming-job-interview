#include "menu.hpp"
#include "window.hpp"
#include "spdlog/spdlog.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "err_logger.hpp"


ImGuiInfoWindow::ImGuiInfoWindow(const std::string& n, const std::string& txt)
    : msg(txt)
    , name(n) {}

void ImGuiInfoWindow::draw() {
    if (!is_open) {
        return;
    }

    if (ImGui::Begin(name.c_str(), &is_open, ImGuiWindowFlags_None)) {
        ImGui::Text("%s", msg.c_str());
        ImGui::PopStyleVar();
    }
    ImGui::End();
}


ImGuiMenu::ImGuiMenu(AppWindow* w): window(w), storage(this) {
    // submenus.push_back(new PropEditorWindow());
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
                // TODO: unhardcode the path
                // Either show native system file selection dialog,
                // or pick one of imgui-powered solutions from github
                storage.from_json_file("./components.json");
            }

            if (ImGui::MenuItem("Save")) {
                // TODO: unhardcode the path
                // Either show native system file selection dialog,
                // or pick one of imgui-powered solutions from github
                storage.to_json_file("./components.json");
            }

            if (ImGui::MenuItem("Exit"))
                window->quit();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Props")) {
            storage.draw();
            // for (auto & [k, v]: objects) {
            //     if (ImGui::MenuItem(k.c_str())) {
            //         v.draw();
            //     }
            // }

            // ImGui::MenuItem("------");

            // ImGui::MenuItem("Create New");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    submenus.draw();
}
