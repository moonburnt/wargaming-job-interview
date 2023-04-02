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
    ImGuiEditorObjectWindow(EditorObject* p);

    EditorObject* get_parent();
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

    EditorObject(const std::string& n);

    ~EditorObject();

    ImGuiEditorObjectWindow* get_window();

    bool is_valid();

    bool add_icon(IconProperty* i);
    bool add_speed(SpeedProperty* sp);
    bool add_material(MaterialProperty* mat);
    bool add_points(PointsProperty* pts);

    std::string get_name();
    std::string to_string();
    nlohmann::json to_json();

    IconProperty* get_icon();
    SpeedProperty* get_speed();
    MaterialProperty* get_material();
    PointsProperty* get_points();

    void update();
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

    bool has_item(const std::string& name);

    std::map<std::string, EditorObject>& get_objects();

    bool add_object(const std::string& name, EditorObject obj);
    bool add_object(const std::string& name);

    nlohmann::json to_json();
    bool to_json_file(const std::string& path);

    virtual void draw();
};
