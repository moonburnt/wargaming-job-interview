#include "objects.hpp"
#include "err_logger.hpp"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "menu.hpp"


// ImGuiEditorObjectWindow
ImGuiEditorObjectWindow::ImGuiEditorObjectWindow(EditorObject* p): parent(p) {}

EditorObject* ImGuiEditorObjectWindow::get_parent() {
    return parent;
}

void ImGuiEditorObjectWindow::draw() {
    if (!is_open) {
        return;
    }

    if (ImGui::Begin(parent->get_name().c_str(), &is_open, ImGuiWindowFlags_None)) {
        IconProperty* icon = parent->get_icon();
        if (icon != nullptr) {
            icon->draw();
        }

        SpeedProperty* speed = parent->get_speed();
        if (speed != nullptr) {
            speed->draw();
        }

        MaterialProperty* material = parent->get_material();
        if (material != nullptr) {
            material->draw();
        }

        PointsProperty* points = parent->get_points();
        if (points != nullptr) {
            points->draw();
        }
    }
    ImGui::End();
}


// EditorObject
EditorObject::EditorObject(const std::string& n)
    : name(n) {
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

ImGuiEditorObjectWindow* EditorObject::get_window() {
    if (window == nullptr) {
        window = new ImGuiEditorObjectWindow(this);
    }

    return window;
}

bool EditorObject::is_valid() {
    return !(
        (icon == nullptr) &&
        (points == nullptr) &&
        (speed == nullptr) &&
        (material == nullptr)
    );
}

bool EditorObject::add_icon(IconProperty* i) {
    bool got_err = false;

    try {
        i->validate();
    }
    catch (ValidationError& v_err) {
        ExceptionLogger::get_logger().log_exception(v_err.what());
        got_err = true;
    }


    icon = i;

    return got_err;
}

bool EditorObject::add_speed(SpeedProperty* sp) {
    bool got_err = false;

    try {
        sp->validate();
    }
    catch (ValidationError& v_err) {
        ExceptionLogger::get_logger().log_exception(v_err.what());
        got_err = true;
    }


    speed = sp;

    return got_err;
}

bool EditorObject::add_material(MaterialProperty* mat) {
    bool got_err = false;

    try {
        mat->validate();
    }
    catch (ValidationError& v_err) {
        ExceptionLogger::get_logger().log_exception(v_err.what());
        got_err = true;
    }

    material = mat;

    return got_err;
}

bool EditorObject::add_points(PointsProperty* pts) {
    bool got_err = false;

    try {
        pts->validate();
    }
    catch (ValidationError& v_err) {
        ExceptionLogger::get_logger().log_exception(v_err.what());
        got_err = true;
    }

    points = pts;

    return got_err;
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

void EditorObject::update() {
    if (icon != nullptr) {
        icon->draw();
    }
    if (speed != nullptr) {
        speed->draw();
    }
    if (material != nullptr) {
        material->draw();
    }
    if (points != nullptr) {
        points->draw();
    }
}


// ObjectStorage
ObjectStorage::ObjectStorage(ImGuiMenu* p)
    : parent_menu(p) {}

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

                // New traversal method may cause issues on incorrectly formatted
                // json. Also pointers will flood the memory if not deleted in
                // obj's destructor

                if (!key.compare("icon")) {
                    obj.add_icon(new IconProperty(pit.value()));
                }
                else if (!key.compare("speed")) {
                    obj.add_speed(
                        new SpeedProperty(
                            pit.value()["value"],
                            pit.value()["min"],
                            pit.value()["max"]
                        )
                    );
                }
                else if (!key.compare("material")) {
                    obj.add_material(
                        new MaterialProperty(
                            pit.value()["value"],
                            pit.value()["choices"]
                        )
                    );
                }
                else if (!key.compare("points")) {
                    obj.add_points(new PointsProperty(pit.value()));
                }
                else {
                    ExceptionLogger::get_logger().log_exception(
                        fmt::format("Unable to find parse rules for key {}", pit.key())
                    );
                }
            }

            if (!obj.is_valid()) {
                ExceptionLogger::get_logger().log_exception(
                    fmt::format(
                        "Object {} does not have a single prop!",
                        obj.get_name()
                    )
                );
            }
            else {
                spdlog::info("Parsed object: {}", obj.to_string());
                add_object(obj_name, obj);
            }
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
    if (!file.good()) {
        ExceptionLogger::get_logger().log_exception(
            fmt::format("Unable to open {}", path)
        );
        return false;
    }

    try {
        file >> data;
    }
    catch (const nlohmann::detail::parse_error& err) {
        ExceptionLogger::get_logger().log_exception(err.what());

        return false;
    }

    return from_json(data);
}

ImGuiMenu* ObjectStorage::get_parent() {
    return parent_menu;
}

int ObjectStorage::size() {
    return objects.size();
}

const EditorObject& ObjectStorage::get_object(const std::string& name) {
    return objects.at(name);
}

bool ObjectStorage::has_item(const std::string& name) {
    return (objects.find(name) != objects.end());
}

std::map<std::string, EditorObject>& ObjectStorage::get_objects() {
    return objects;
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
            ImGuiEditorObjectWindow* w = v.get_window();
            w->is_open = true;
            parent_menu->add_submenu(w);
        }
    }
}
