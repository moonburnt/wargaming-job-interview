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

    void update(float) override {
        if (!is_open) {
            must_die = true;
        }
    }

    void draw() override;
};


class CreateObjectWindow: public MenuWindow {
private:
    ObjectStorage* storage = nullptr;

    std::string current_name;

    // This can be organized better
    bool has_icon = false;
    std::string icon_path;

    bool has_speed = false;
    float current_speed;
    float min_speed = 0.0f;
    float max_speed = 0.0f;

    bool has_material = false;
    std::vector<std::string> material_choices;
    int current_choice;
    std::string choices_string;
    std::string current_mat;

    bool has_points = false;
    int points = 0;

    bool has_errors = false;

public:
    CreateObjectWindow(ObjectStorage* st): storage(st) {}

    void update(float) override {
        if (!is_open) {
            must_die = true;
        }
    }

    void draw() override {
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
                ImGui::InputFloat("Enter current speed value", &min_speed);
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

                if (storage->has_item(current_name)) {
                    ExceptionLogger::get_logger().log_exception(
                        fmt::format(
                            "Item with name {} already exists in storage",
                            current_name
                        )
                    );
                    has_errors = true;
                }

                if (has_points && points < 0) {
                    ExceptionLogger::get_logger().log_exception(
                        "Points must be a positive integer"
                    );
                    has_errors = true;
                }

                if (has_material && material_choices.empty()) {
                    ExceptionLogger::get_logger().log_exception(
                        "Materials list must have at least one item"
                    );
                    has_errors = true;
                }

                if (has_speed && !(min_speed <= current_speed <= max_speed)) {
                    ExceptionLogger::get_logger().log_exception(
                        "Current speed must be higher or eq to min, lower or eq to max"
                    );
                    has_errors = true;
                }

                if (!has_icon && !has_material && !has_speed && !has_points) {
                    ExceptionLogger::get_logger().log_exception(
                        "Must have at least one prop"
                    );
                    has_errors = true;
                }

                if (!has_errors) {
                    EditorObject obj = EditorObject(current_name);
                    if (has_icon) {
                        obj.add_icon(new IconProperty(icon_path));
                    }
                    if (has_speed) {
                        obj.add_speed(
                            new SpeedProperty(
                                current_speed,
                                min_speed,
                                max_speed
                            )
                        );
                    }
                    if (has_material) {
                        obj.add_material(
                            new MaterialProperty(
                                material_choices[current_choice],
                                material_choices
                            )
                        );
                    }
                    if (has_points) {
                        obj.add_points(
                            new PointsProperty(points)
                        );
                    }

                    storage->add_object(current_name, obj);
                    is_open = false;
                }
            }

        }
        ImGui::End();
    }

};


class FileActionWindow: public MenuWindow {
private:
    const std::string name;
    std::string current;

protected:
    const std::string action_name;
    ObjectStorage* storage = nullptr;

public:
    FileActionWindow(const std::string n, const std::string an, ObjectStorage* s)
        : name(n)
        , action_name(an)
        , storage(s) {}

    void update(float) override {
        if (!is_open) {
            must_die = true;
        }
    }

    virtual bool perform_action(const std::string &) = 0;

    void draw() override {
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
};


class LoadFileWindow: public FileActionWindow {
public:
    LoadFileWindow(ObjectStorage* s)
        : FileActionWindow("Load File", "Load", s) {}

    bool perform_action(const std::string& fp) override {
        return storage->from_json_file(fp);
    }
};

class SaveFileWindow: public FileActionWindow {
public:
    SaveFileWindow(ObjectStorage* s)
        : FileActionWindow("Save File", "Save", s) {}

    bool perform_action(const std::string& fp) override {
        return storage->to_json_file(fp);
    }
};


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
