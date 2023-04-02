#pragma once

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "properties.hpp"
#include <string>
#include <map>
#include <fstream>
#include "view.hpp"

class ImGuiMenu;
class EditorObject;
class ObjectStorage;

class ImGuiEditorObjectWindow: public MenuWindow {
private:
    EditorObject* parent;

public:
    ImGuiEditorObjectWindow(EditorObject* p): parent(p) {}

    EditorObject* get_parent() {
        return parent;
    }

    void draw() override;
};

class EditorObject {
protected:
    std::string name;
    // TODO: store everything in a map. For now, this jank will do instead
    IconProperty* icon = nullptr;
    SpeedProperty* speed = nullptr;
    MaterialProperty* material = nullptr;
    PointsProperty* points = nullptr;

    ImGuiEditorObjectWindow* window = nullptr;

public:
    ImGuiEditorObjectWindow* get_window() {
        if (window == nullptr) {
            window = new ImGuiEditorObjectWindow(this);
        }

        return window;
    }

    EditorObject(const std::string& n);

    ~EditorObject();

    bool add_icon(IconProperty* i);

    bool add_speed(SpeedProperty* sp);

    bool add_material(MaterialProperty* mat);

    bool add_points(PointsProperty* pts);

    bool is_valid();

    std::string get_name();

    std::string to_string();

    nlohmann::json to_json();

    IconProperty* get_icon();

    SpeedProperty* get_speed();

    MaterialProperty* get_material();

    PointsProperty* get_points();

    void update() {
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
};


class ObjectStorage {
private:
    ImGuiMenu* parent_menu = nullptr;

protected:
    std::map<std::string, EditorObject> objects = {};

public:
    ObjectStorage(ImGuiMenu* p);

    ImGuiMenu* get_parent();

    bool from_json(nlohmann::json& data);

    bool from_json_file(const std::string &path);

    int size();

    // Will throw if object does not exist
    const EditorObject& get_object(const std::string& name);

    bool has_item(const std::string& name) {
        return (objects.find(name) != objects.end());
    }

    std::map<std::string, EditorObject>& get_objects() {
        return objects;
    }

    bool add_object(const std::string& name, EditorObject obj);

    bool add_object(const std::string& name);

    nlohmann::json to_json();

    bool to_json_file(const std::string& path);

    virtual void draw();
};
