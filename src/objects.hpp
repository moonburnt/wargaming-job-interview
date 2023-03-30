#pragma once

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "validators.hpp"
#include <string>
#include <fstream>
#include <optional>
#include <unordered_map>


template <typename T> class ObjectProperty {
protected:
    std::string name = "";
    bool is_validated = false;
    T data;
    std::optional<T> validated_data = std::nullopt;

    void set_name(const std::string& n) {
        name = n;
    }

    std::vector<Validator<T>*> validators = {};

public:
    ObjectProperty(T i) : data(i) {}

    void set_value(T val) {
        data = val;
        is_validated = false;
    }

    std::string get_name() {
        return name;
    }

    // TODO: del validators on delete
    virtual ~ObjectProperty() = default;


    void add_validator(Validator<T>* f) {
        validators.push_back(f);
    }

    void perform_validation() {
        for (auto i: validators) {
            i->validate(data);
        }
        is_validated = true;
        validated_data = data;
    }

    // Validate data
    std::optional<T> validate() {
        perform_validation();
        return validated_data;
    }

    T get_validated_data() {
        if (!is_validated) {
            throw ValidationError(
                "Unable to get validated data from non-validated source. "
                "Run perform_validation() and try again"
            );
        }
        else {
            return validated_data.value();
        }
    }

    virtual std::string to_string() {
        return fmt::format("{} ({})", get_name(), get_data());
    }

    virtual nlohmann::json to_json() {
        return get_validated_data();
    }

    T get_data() {
        return data;
    }
};


// TODO: also store image
class Icon : public ObjectProperty<std::string> {
public:
    Icon(std::string p) : ObjectProperty(p) {
        set_name("icon");

        add_validator(new FilePathValidator());
    }
};


class Speed : public ObjectProperty<float> {
public:
    Speed(float val, float min, float max) : ObjectProperty(val) {
        set_name("speed");

        FloatInRangeValidator* v = new FloatInRangeValidator();
        v->set_min(min);
        v->set_max(max);
        add_validator(v);
    }
};


class Material : public ObjectProperty<std::string> {
public:
    Material(std::string val, std::vector<std::string> choices) : ObjectProperty(val) {
        set_name("material");

        TextChoicesValidator* v = new TextChoicesValidator();
        v->set_choices(choices);
        add_validator(v);
    }
};


class Points: public ObjectProperty<int> {
public:
    Points(int val) : ObjectProperty(val) {
        set_name("points");

        add_validator(new IntegerPositiveValidator());
    }
};


class EditorObject {
protected:
    std::string name;
    // TODO: store everything in a map. For now, this jank will do instead
    Icon* icon = nullptr;
    Speed* speed = nullptr;
    Material* material = nullptr;
    Points* points = nullptr;

public:
    EditorObject(const std::string& n) : name(n) {}

    void add_icon(Icon* i) {
        icon = i;
    }

    void add_speed(Speed* sp) {
        speed = sp;
    }

    void add_material(Material* mat) {
        material = mat;
    }

    void add_points(Points* pts) {
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

    Icon* get_icon() {
        return icon;
    }

    Speed* get_speed() {
        return speed;
    }

    Material* get_material() {
        return material;
    }

    Points* get_points() {
        return points;
    }
};


class JsonParser {
private:
    std::string path;

public:
    JsonParser(const std::string& p) : path(p) {}

    bool parse(std::unordered_map<std::string, EditorObject>& objects) {
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
                        Icon* i;
                        try {
                            i = new Icon(pit.value());
                            i->validate();
                        }
                        catch (ValidationError& v_err) {
                            spdlog::warn(v_err.what());
                            continue;
                        }
                        obj.add_icon(i);
                    }
                    else if (!key.compare("speed")) {
                        Speed* sp;
                        try {
                            sp = new Speed(
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
                        Material* mat;
                        try {
                            mat = new Material(
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
                        Points* pts;
                        try {
                            pts = new Points(
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
};


class ObjectStorage {
protected:
    std::unordered_map<std::string, EditorObject> objects = {};

public:
    void populate_from_json(const std::string& p) {
        JsonParser jp = JsonParser(p);
        jp.parse(objects);
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
