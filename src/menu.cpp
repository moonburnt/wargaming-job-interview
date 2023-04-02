#include "menu.hpp"
#include "window.hpp"
#include "spdlog/spdlog.h"
#include "imgui.h"


// ImGuiInfoWindow
ImGuiInfoWindow::ImGuiInfoWindow(const std::string& n, const std::string& txt)
    : msg(txt)
    , name(n) {}

void ImGuiInfoWindow::update(float) {
    if (!is_open) {
        must_die = true;
    }
}

void ImGuiInfoWindow::draw() {
    if (!is_open) {
        return;
    }

    if (ImGui::Begin(name.c_str(), &is_open, ImGuiWindowFlags_None)) {
        ImGui::Text("%s", msg.c_str());
    }
    ImGui::End();
}


// CreateObjectWindow
CreateObjectWindow::CreateObjectWindow(ObjectStorage* st): storage(st) {}

void CreateObjectWindow::update(float) {
    if (!is_open) {
        must_die = true;
    }
}

void CreateObjectWindow::draw() {
    if (!is_open) {
        return;
    }

    if (ImGui::Begin("Add Item", &is_open, ImGuiWindowFlags_None)) {
        ImGui::InputText(
            "Enter item's name",
            &current_name
        );

        ImGui::Checkbox("Has Icon", &has_icon);

        if (has_icon) {
            ImGui::InputText(
                "Enter item's icon path",
                &icon_path
            );
        }
        else {
            icon_path = "";
        }

        ImGui::Checkbox("Has Speed", &has_speed);
        if (has_speed) {
            ImGui::InputFloat("Enter current speed value", &current_speed);
            ImGui::InputFloat("Enter min speed value", &min_speed);
            ImGui::InputFloat("Enter max speed value", &max_speed);
        }
        else {
            current_speed = 0.0f;
            min_speed = 0.0f;
            max_speed = 0.0f;
        }

        ImGui::Checkbox("Has Material", &has_material);
        if (has_material) {
            ImGui::Text("Choices: %s", choices_string.c_str());

            ImGui::InputText(
                "Enter item's new material",
                &current_mat
            );
            if (ImGui::Button("Add material")) {
                if (
                    std::find(
                        material_choices.begin(),
                        material_choices.end(),
                        current_mat
                    ) == material_choices.end()
                ) {
                    material_choices.push_back(current_mat);
                    choices_string += (current_mat + ", ");
                    current_mat = "";
                }
                else {
                    ExceptionLogger::get_logger().log_exception(
                        fmt::format(
                            "Material {} already exists in storage",
                            current_mat
                        )
                    );
                }
            }

            if (!material_choices.empty()) {
                if (
                    ImGui::BeginCombo(
                        "Current mat choice",
                        material_choices[current_choice].c_str()
                    )
                ) {
                    for (
                        int i = 0;
                        i < static_cast<int>(material_choices.size());
                        i++
                    ) {
                        const bool is_selected = (current_choice == i);
                        if (
                            ImGui::Selectable(
                                material_choices[i].c_str(),
                                is_selected
                            )
                        ) {
                            current_choice = i;
                        }

                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }
        }
        else {
            current_mat = "";
            choices_string = "";
            material_choices.clear();
        }

        ImGui::Checkbox("Has Points", &has_points);
        if (has_points) {
            ImGui::InputInt("Enter item's points", &points);
        }
        else {
            points = 0;
        }

        if (ImGui::Button("Save")) {
            has_errors = false;

            EditorObject obj = EditorObject(current_name);

            if (!has_errors && has_icon) {
                has_errors = obj.add_icon(new IconProperty(icon_path));
            }
            if (!has_errors && has_speed) {
                has_errors = obj.add_speed(
                    new SpeedProperty(
                        current_speed,
                        min_speed,
                        max_speed
                    )
                );
            }
            if (!has_errors && has_material) {
                has_errors = obj.add_material(
                    new MaterialProperty(
                        material_choices[current_choice],
                        material_choices
                    )
                );
            }
            if (!has_errors && has_points) {
                has_errors = obj.add_points(
                    new PointsProperty(points)
                );
            }

            if (!obj.is_valid()) {
                ExceptionLogger::get_logger().log_exception(
                    fmt::format(
                        "Object {} does not have a single prop!",
                        obj.get_name()
                    )
                );
                has_errors = true;
            }

            if (!has_errors) {
                has_errors = (!storage->add_object(current_name, obj));
            }

            if (!has_errors) {
                is_open = false;
            }
        }

    }
    ImGui::End();
}


// FileActionWindow
FileActionWindow::FileActionWindow(
    const std::string n,
    const std::string an,
    ObjectStorage* s
): name(n), action_name(an), storage(s) {}

void FileActionWindow::update(float) {
    if (!is_open) {
        must_die = true;
    }
}

void FileActionWindow::draw() {
    if (!is_open) {
        return;
    }

    if (ImGui::Begin(name.c_str(), &is_open, ImGuiWindowFlags_None)) {
        ImGui::InputText(
            "Enter filepath",
            &current
        );

        if (ImGui::Button(action_name.c_str())) {
            bool is_good = true;
            try {
                is_good = perform_action(current);
            }
            catch (std::ios_base::failure& exc) {
                ExceptionLogger::get_logger().log_exception(&exc);
                is_good = false;
            }

            if (is_good) {
                is_open = false;
            }
        }
    }
    ImGui::End();
}


// LoadFileWindow
LoadFileWindow::LoadFileWindow(ObjectStorage* s)
    : FileActionWindow("Load File", "Load", s) {}

bool LoadFileWindow::perform_action(const std::string& fp) {
    return storage->from_json_file(fp);
}


// SaveFileWindow
SaveFileWindow::SaveFileWindow(ObjectStorage* s)
    : FileActionWindow("Save File", "Save", s) {}

bool SaveFileWindow::perform_action(const std::string& fp) {
    return storage->to_json_file(fp);
}


// MenuStorage
void MenuStorage::add(MenuWindow* w) {
    if (std::find(submenus.begin(), submenus.end(), w) != submenus.end()) {
        return;
    }
    else {
        submenus.push_back(w);
    }
}

void MenuStorage::remove_dead() {
    // cleanup
    submenus.erase(
        std::remove_if(
            submenus.begin(),
            submenus.end(),
            [](MenuWindow* i){ return (i->must_die); }),
        submenus.end()
    );
}

void MenuStorage::draw() {
    for (auto i: submenus) {
        if (i->is_open) {
            i->draw();
        }
    }
}


// ImGuiMenu
ImGuiMenu::ImGuiMenu(AppWindow* w): window(w), storage(this) {}

void ImGuiMenu::add_submenu(MenuWindow* w) {
    submenus.add(w);
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
