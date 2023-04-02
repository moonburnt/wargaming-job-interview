#pragma once

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "validators.hpp"
#include <string>
#include <optional>
#include "err_logger.hpp"
#include "imgui.h"
#include "rlImGui.h"
#include "view.hpp"

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

    virtual void perform_validation() {
        for (auto i: validators) {
            i->validate(data);
        }
        is_validated = true;
        validated_data = data;
    }

    // Validate data
    std::optional<T> validate() {
        if (!is_validated) {
            perform_validation();
        }

        return validated_data;
    }

    T get_validated_data() {
        return validated_data.value_or(T());
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
        ImGui::Text("%s", to_string().c_str());
    }
};


// FD
class EditorObject;

class IconProperty : public ObjectProperty<std::string> {
private:
    IconView view;
    std::optional<Texture> texture;

public:
    IconProperty(std::string str);
    ~IconProperty();

    void perform_validation() override;
    const std::optional<Texture>& get_texture();

    void draw() override;
};


class SpeedProperty : public ObjectProperty<float> {
private:
    SpeedView view;

protected:
    float min;
    float max;

public:
    SpeedProperty(float val, float _min, float _max);

    float get_min();
    float get_max();

    nlohmann::json to_json() override;

    void draw() override;
};


class MaterialProperty : public ObjectProperty<std::string> {
private:
    MaterialView view;

protected:
    std::vector<std::string> choices;

public:
    MaterialProperty(std::string val, std::vector<std::string> choices);

    const std::vector<std::string>& get_choices();
    int get_choices_amount();

    nlohmann::json to_json() override;

    void draw() override;
};


class PointsProperty: public ObjectProperty<int> {
private:
    PointsView view;

public:
    PointsProperty(int val);

    void draw() override;
};
