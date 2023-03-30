#pragma once

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "properties.hpp"
#include <string>
#include <map>
#include <fstream>

class EditorObject {
protected:
    std::string name;
    // TODO: store everything in a map. For now, this jank will do instead
    IconProperty* icon = nullptr;
    SpeedProperty* speed = nullptr;
    MaterialProperty* material = nullptr;
    PointsProperty* points = nullptr;

public:
    EditorObject(const std::string& n);

    void add_icon(IconProperty* i);

    void add_speed(SpeedProperty* sp);

    void add_material(MaterialProperty* mat);

    void add_points(PointsProperty* pts);

    std::string get_name();

    std::string to_string();

    nlohmann::json to_json();

    IconProperty* get_icon();

    SpeedProperty* get_speed();

    MaterialProperty* get_material();

    PointsProperty* get_points();

    virtual void draw();
};


class ObjectStorage {
protected:
    std::map<std::string, EditorObject> objects = {};

public:
    bool from_json(nlohmann::json& data);

    bool from_json_file(const std::string &path);

    int size();

    // Will throw if object does not exist
    const EditorObject& get_object(const std::string& name);

    bool add_object(const std::string& name, EditorObject obj);

    bool add_object(const std::string& name);

    nlohmann::json to_json();

    bool to_json_file(const std::string& path);

    virtual void draw();
};
