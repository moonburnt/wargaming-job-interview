#include "objects.hpp"
#include "err_logger.hpp"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "menu.hpp"


void ImGuiEditorObjectWindow::draw() {
    if (!is_open) {
        return;
    }

    if (ImGui::Begin(parent->get_name().c_str(), &is_open, ImGuiWindowFlags_None)) {
        IconProperty* icon = parent->get_icon();
        if (icon != nullptr) {
            icon->draw();
            // ImGui::Text("Icon: %s", icon->to_string().c_str());
        }

        SpeedProperty* speed = parent->get_speed();
        if (speed != nullptr) {
            // ImGui::Text("Speed: %s", speed->to_string().c_str());
            speed->draw();
        }

        MaterialProperty* material = parent->get_material();
        if (material != nullptr) {
            // ImGui::Text("Material: %s", material->to_string().c_str());
            material->draw();
        }

        PointsProperty* points = parent->get_points();
        if (points != nullptr) {
            // ImGui::Text("Points: %s", points->to_string().c_str());
            points->draw();
        }
        // ImGui::Text("%s", parent->to_string().c_str());
        ImGui::PopStyleVar();
    }
    ImGui::End();
}


EditorObject::EditorObject(const std::string& n) : name(n) {
    // window = new ImGuiInfoWindow(n, to_string());
    // window = new ImGuiEditorObjectWindow(this);
}

EditorObject::~EditorObject() {

    // TODO: investigate. seems like it cause segfault for now
    // if (icon != nullptr) {
    //     delete icon;
    // }
    // if (speed != nullptr) {
    //     delete speed;
    // }
    // if (material != nullptr) {
    //     delete material;
    // }
    // if (points != nullptr) {
    //     delete points;
    // }

    // Will be deleted by parent's cleanup
    if (window != nullptr) {
        window->must_die = true;
    }
}

void EditorObject::add_icon(IconProperty* i) {
    icon = i;
}

void EditorObject::add_speed(SpeedProperty* sp) {
    speed = sp;
}

void EditorObject::add_material(MaterialProperty* mat) {
    material = mat;
}

void EditorObject::add_points(PointsProperty* pts) {
    points = pts;
}

std::string EditorObject::get_name() {
    return name;
}

std::string EditorObject::to_string() {
    std::string ret = "EditorObject '" + get_name() + "': (";
    if (icon != nullptr) {
        ret += icon->to_string() + ", ";
    }
    if (speed != nullptr) {
        ret += speed->to_string() + ", ";
    }
    if (material != nullptr) {
        ret += material->to_string() + ", ";
    }
    if (points != nullptr) {
        ret += points->to_string() + ", ";
    }

    ret += ")";
    return ret;
}

nlohmann::json EditorObject::to_json() {
    nlohmann::json data;
    if (icon != nullptr) {
        data["icon"] = icon->to_json();
    }
    if (speed != nullptr) {
        data["speed"] = speed->to_json();
    }
    if (material != nullptr) {
        data["material"] = material->to_json();
    }
    if (points != nullptr) {
        data["points"] = points->to_json();
    }

    return data;
}

IconProperty* EditorObject::get_icon() {
    return icon;
}

SpeedProperty* EditorObject::get_speed() {
    return speed;
}

MaterialProperty* EditorObject::get_material() {
    return material;
}

PointsProperty* EditorObject::get_points() {
    return points;
}

// void EditorObject::draw() {
//     spdlog::info("drawing object {}", get_name());

//     if (icon != nullptr) {
//         icon->draw();
//     }
//     if (speed != nullptr) {
//         speed->draw();
//     }
//     if (material != nullptr) {
//         material->draw();
//     }
//     if (points != nullptr) {
//         points->draw();
//     }
// }


bool ObjectStorage::from_json(nlohmann::json& data) {
    try {
        for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
            std::string obj_name = it.key();
            if (objects.find(obj_name) != objects.end()) {
                ExceptionLogger::get_logger().log_exception(
                    fmt::format(
                        "Already parsed object with name {}, skipping",
                        obj_name
                    )
                );
            }
            EditorObject obj = EditorObject(obj_name);
            auto props = it.value();

            // TODO: maybe add different error handling options - skip,
            // replace with default, etc
            for (nlohmann::json::iterator pit = props.begin(); pit != props.end(); pit++) {
                std::string key = pit.key();

                // New traversal method may cause issues on incorrectly formatted json

                if (!key.compare("icon")) {
                    IconProperty* i = new IconProperty(pit.value());

                    try {
                        i->validate();
                    }
                    catch (ValidationError& v_err) {
                        ExceptionLogger::get_logger().log_exception(v_err.what());
                    }
                    obj.add_icon(i);
                }
                else if (!key.compare("speed")) {
                    SpeedProperty* sp =
                        new SpeedProperty(
                            pit.value()["value"],
                            pit.value()["min"],
                            pit.value()["max"]
                        );

                    try {
                        sp->validate();
                    }
                    catch (ValidationError& v_err) {
                        ExceptionLogger::get_logger().log_exception(v_err.what());
                    }
                    obj.add_speed(sp);
                }
                else if (!key.compare("material")) {
                    MaterialProperty* mat = new MaterialProperty(
                        pit.value()["value"],
                        pit.value()["choices"]
                    );

                    try {
                        mat->validate();
                    }
                    catch (ValidationError& v_err) {
                        ExceptionLogger::get_logger().log_exception(v_err.what());
                    }
                    obj.add_material(mat);
                }
                else if (!key.compare("points")) {
                    PointsProperty* pts = new PointsProperty(
                        pit.value()
                    );

                    try {
                        pts->validate();
                    }
                    catch (ValidationError& v_err) {
                        ExceptionLogger::get_logger().log_exception(v_err.what());
                    }
                    obj.add_points(pts);
                }
                else {
                    ExceptionLogger::get_logger().log_exception(
                        fmt::format("Unable to find parse rules for key {}", pit.key())
                    );
                }
            }
            spdlog::info("Parsed object: {}", obj.to_string());
            objects.insert(std::make_pair(obj_name, obj));
        }
    }
    catch (const nlohmann::detail::type_error& err) {
        ExceptionLogger::get_logger().log_exception(err.what());

        return false;
    }
    return true;
}

bool ObjectStorage::from_json_file(const std::string &path) {
    std::ifstream file;
    nlohmann::json data;

    file.open(path);
    // TODO: more detailed error handling
    if (!file.good()) {
        spdlog::error("Unable to open {}", path);
        return false;
    }

    try {
        file >> data;
    }
    catch (const nlohmann::detail::parse_error& err) {
        spdlog::error(err.what());

        return false;
    }

    return from_json(data);
}

int ObjectStorage::size() {
    return objects.size();
}

// Will throw if object does not exist
const EditorObject& ObjectStorage::get_object(const std::string& name) {
    return objects.at(name);
}

bool ObjectStorage::add_object(const std::string& name, EditorObject obj) {
    if (objects.find(name) != objects.end()) {
        ExceptionLogger::get_logger().log_exception(
            fmt::format("Unable to add object with name '{}': already exists",  name)
        );
        return false;
    }
    else {
        objects.insert(std::make_pair(name, obj));
        return true;
    }
}

bool ObjectStorage::add_object(const std::string& name) {
    return add_object(name, EditorObject(name));
}

nlohmann::json ObjectStorage::to_json() {
    nlohmann::json data;

    for (auto& [k, v]: objects) {
        data[k] = v.to_json();
    }

    return data;
}

bool ObjectStorage::to_json_file(const std::string& path) {
    nlohmann::json data = to_json();

    std::ofstream file;

    file.open(path);
    if (!file.good()) {
        ExceptionLogger::get_logger().log_exception(fmt::format("Unable to open {}", path));
        return false;
    }

    try {
        file << data;
    }
    catch (const nlohmann::detail::parse_error& err) {
        ExceptionLogger::get_logger().log_exception(err.what());
        return false;
    }

    return true;
}

void ObjectStorage::draw() {
    for (auto & [k, v]: objects) {
        if (ImGui::MenuItem(k.c_str())) {
            // parent_menu->add_submenu(v.window);
            // v.window.is_open = true;
            // v.get_window()->is_open = true;
            ImGuiEditorObjectWindow* w = v.get_window();
            w->is_open = true;
            parent_menu->add_submenu(w);
        }
    }

    ImGui::MenuItem("------");

    ImGui::MenuItem("Create New");

    // for (auto i: submenus) {
    //     if (i->is_open) {
    //         i->draw();
    //     }
    // }
}
