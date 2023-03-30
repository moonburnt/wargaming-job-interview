#include "menu.hpp"
#include "window.hpp"
#include "spdlog/spdlog.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "err_logger.hpp"


ImGuiMenu::ImGuiMenu(AppWindow* w): window(w) {
    // submenus.push_back(new PropEditorWindow());
}


void ImGuiMenu::update(float) {
    // cleanup
    submenus.erase(
        std::remove_if(
            submenus.begin(),
            submenus.end(),
            [](MenuWindow* i){ return !(i->is_open); }),
        submenus.end()
    );

    for (auto i: ExceptionLogger::get_logger().get_messages()) {
        submenus.push_back(new ImGuiInfoWindow("Error", i));
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


            if (ImGui::MenuItem("Exit"))
                window->quit();



            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Props")) {
            storage.draw();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    for (auto i: submenus) {
        if (i->is_open) {
            i->draw();
        }
    }
}
