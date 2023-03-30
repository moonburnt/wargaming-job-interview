#pragma once

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "properties.hpp"
#include <string>
#include <unordered_map>
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
    EditorObject(const std::string& n) : name(n) {}

    void add_icon(IconProperty* i) {
        icon = i;
    }

    void add_speed(SpeedProperty* sp) {
        speed = sp;
    }

    void add_material(MaterialProperty* mat) {
        material = mat;
    }

    void add_points(PointsProperty* pts) {
        points = pts;
    }

    std::string get_name() {
        return name;
    }

    std::string to_string() {
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

    nlohmann::json to_json() {
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

    IconProperty* get_icon() {
        return icon;
    }

    SpeedProperty* get_speed() {
        return speed;
    }

    MaterialProperty* get_material() {
        return material;
    }

    PointsProperty* get_points() {
        return points;
    }
};


class ObjectStorage {
protected:
    std::unordered_map<std::string, EditorObject> objects = {};

public:
    bool from_json(nlohmann::json& data) {
        try {
            for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
                std::string obj_name = it.key();
                if (objects.find(obj_name) != objects.end()) {
                    spdlog::warn(
                        "Already parsed object with name {}, skipping",
                        obj_name
                    );
                }
                EditorObject obj = EditorObject(obj_name);
                auto props = it.value();

                // TODO: maybe add different error handling options - skip,
                // replace with default, etc
                for (nlohmann::json::iterator pit = props.begin(); pit != props.end(); pit++) {
                    std::string key = pit.key();

                    if (!key.compare("icon")) {
                        IconProperty* i;
                        try {
                            i = new IconProperty(pit.value());
                            i->validate();
                        }
                        catch (ValidationError& v_err) {
                            spdlog::warn(v_err.what());
                            continue;
                        }
                        obj.add_icon(i);
                    }
                    else if (!key.compare("speed")) {
                        SpeedProperty* sp;
                        try {
                            sp = new SpeedProperty(
                                pit.value()["value"],
                                pit.value()["min"],
                                pit.value()["max"]
                            );
                            sp->validate();
                        }
                        catch (ValidationError& v_err) {
                            spdlog::warn(v_err.what());
                            continue;
                        }
                        obj.add_speed(sp);
                    }
                    else if (!key.compare("material")) {
                        MaterialProperty* mat;
                        try {
                            mat = new MaterialProperty(
                                pit.value()["value"],
                                pit.value()["choices"]
                            );
                            mat->validate();
                        }
                        catch (ValidationError& v_err) {
                            spdlog::warn(v_err.what());
                            continue;
                        }
                        obj.add_material(mat);
                    }
                    else if (!key.compare("points")) {
                        PointsProperty* pts;
                        try {
                            pts = new PointsProperty(
                                pit.value()
                            );
                            pts->validate();
                        }
                        catch (ValidationError& v_err) {
                            spdlog::warn(v_err.what());
                            continue;
                        }
                        obj.add_points(pts);
                    }
                    else {
                        spdlog::warn(
                            "Unable to find parse rules for key {}", pit.key()
                        );
                    }
                }
                spdlog::info("Parsed object: {}", obj.to_string());
                objects.insert(std::make_pair(obj_name, obj));
            }
        }
        catch (const nlohmann::detail::type_error& err) {
            spdlog::warn(err.what());

            return false;
        }
        return true;
    }

    bool from_json_file(const std::string &path) {
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

    int size() {
        return objects.size();
    }

    // Will throw if object does not exist
    const EditorObject& get_object(const std::string& name) {
        return objects.at(name);
    }

    bool add_object(const std::string& name, EditorObject obj) {
        if (objects.find(name) != objects.end()) {
            spdlog::warn(
                "Unable to add object with name '{}': already exists",  name
            );
            return false;
        }
        else {
            objects.insert(std::make_pair(name, obj));
            return true;
        }
    }

    bool add_object(const std::string& name) {
        return add_object(name, EditorObject(name));
    }

    nlohmann::json to_json() {
        nlohmann::json data;

        for (auto& [k, v]: objects) {
            data[k] = v.to_json();
        }

        return data;
    }

    bool to_json_file(const std::string& path) {
        nlohmann::json data = to_json();

        std::ofstream file;

        file.open(path);
        if (!file.good()) {
            spdlog::error("Unable to open {}", path);
            return false;
        }

        try {
            file << data;
        }
        catch (const nlohmann::detail::parse_error& err) {
            spdlog::error(err.what());
            return false;
        }

        return true;
    }
};
