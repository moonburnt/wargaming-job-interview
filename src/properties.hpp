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
#include "rlImGuiColors.h"

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

// FDs for views
class IconProperty;
class SpeedProperty;
class MaterialProperty;
class PointsProperty;

class IconView {
private:
    IconProperty* parent;

    std::string current_txt;

    bool showing_texture = false;

public:
    IconView(IconProperty* p)
        : parent(p) {}

    void draw();
};


class SpeedView {
private:
    SpeedProperty* parent;

    float current;
public:
    SpeedView(SpeedProperty* p): parent(p) {}

    void draw();
};


class MaterialView {
private:
    MaterialProperty* parent;

    int current = 0;

public:
    MaterialView(MaterialProperty* p): parent(p) {}

    void draw();
};


class PointsView {
private:
    PointsProperty* parent;

    int current = 0;

public:
    PointsView(PointsProperty* p): parent(p) {}

    void draw();
};


class EditorObject;


class IconProperty : public ObjectProperty<std::string> {
private:
    IconView view;
    std::optional<Texture> texture;

public:
    IconProperty(std::string str);
    ~IconProperty() {
        if (texture.has_value()) {
            UnloadTexture(texture.value());
        }
    }

    void perform_validation() override {
        ObjectProperty<std::string>::perform_validation();
        if (texture.has_value()) {
            UnloadTexture(texture.value());
        }
        texture = LoadTexture(validated_data.value().c_str());
    }

    const std::optional<Texture>& get_texture() {
        return texture;
    }

    void draw() override {
        view.draw();
    }
};


class SpeedProperty : public ObjectProperty<float> {
private:
    SpeedView view;

protected:
    float min;
    float max;

public:
    SpeedProperty(float val, float _min, float _max);

    float get_min() {
        return min;
    }

    float get_max() {
        return max;
    }

    void draw() override {
        view.draw();
    }
};


class MaterialProperty : public ObjectProperty<std::string> {
private:
    MaterialView view;

protected:
    std::vector<std::string> choices;

public:
    MaterialProperty(std::string val, std::vector<std::string> choices);

    const std::vector<std::string>& get_choices() {
        return choices;
    }

    int get_choices_amount() {
        return choices.size();
    }

    void draw() override {
        view.draw();
    }
};


class PointsProperty: public ObjectProperty<int> {
private:
    PointsView view;

public:
    PointsProperty(int val);

    void draw() override {
        view.draw();
    }
};
