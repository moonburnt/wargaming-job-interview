#pragma once

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "validators.hpp"
#include <string>
#include <optional>

#include "imgui.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"


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

    virtual void draw() {
        spdlog::info("Drawing property {}", get_name());
        ImGui::Text("%s", to_string().c_str());
    }
};


// TODO: also store image
class IconProperty : public ObjectProperty<std::string> {
public:
    IconProperty(std::string p);
};


class SpeedProperty : public ObjectProperty<float> {
public:
    SpeedProperty(float val, float min, float max);
};


class MaterialProperty : public ObjectProperty<std::string> {
public:
    MaterialProperty(std::string val, std::vector<std::string> choices);
};


class PointsProperty: public ObjectProperty<int> {
public:
    PointsProperty(int val);
};
