#include "properties.hpp"
#include "misc/cpp/imgui_stdlib.h"
#include "objects.hpp"
#include "menu.hpp"
#include "view.hpp"


// IconProperty
IconProperty::IconProperty(std::string s)
    : ObjectProperty(s)
    , view(IconView(this)) {
    set_name("icon");

    add_validator(new FilePathValidator());
}

IconProperty::~IconProperty() {
    if (texture.has_value()) {
        UnloadTexture(texture.value());
    }
}

void IconProperty::perform_validation() {
    ObjectProperty<std::string>::perform_validation();
    if (texture.has_value()) {
        UnloadTexture(texture.value());
    }
    texture = LoadTexture(validated_data.value().c_str());
}

const std::optional<Texture>& IconProperty::get_texture() {
    return texture;
}

void IconProperty::draw() {
    view.draw();
}


// SpeedProperty
SpeedProperty::SpeedProperty(float val, float _min, float _max)
    : ObjectProperty(val)
    , view(SpeedView(this))
    , min(_min)
    , max(_max) {
    set_name("speed");

    FloatInRangeValidator* v = new FloatInRangeValidator();
    v->set_min(min);
    v->set_max(max);
    add_validator(v);
}

float SpeedProperty::get_min() {
    return min;
}

float SpeedProperty::get_max() {
    return max;
}

nlohmann::json SpeedProperty::to_json() {
    nlohmann::json ret = {};
    ret["min"] = min;
    ret["max"] = max;
    ret["value"] = get_validated_data();

    return ret;
}

void SpeedProperty::draw() {
    view.draw();
}


// MaterialProperty
MaterialProperty::MaterialProperty(std::string val, std::vector<std::string> ch)
    : ObjectProperty(val)
    , view(MaterialView(this))
    , choices(ch) {
    set_name("material");

    TextChoicesValidator* v = new TextChoicesValidator();
    v->set_choices(choices);
    add_validator(v);
}

const std::vector<std::string>& MaterialProperty::get_choices() {
    return choices;
}

int MaterialProperty::get_choices_amount() {
    return choices.size();
}

nlohmann::json MaterialProperty::to_json() {
    nlohmann::json ret = {};
    ret["choices"] = choices;
    ret["value"] = get_validated_data();

    return ret;
}

void MaterialProperty::draw() {
    view.draw();
}


// PointsProperty
PointsProperty::PointsProperty(int val)
    : ObjectProperty(val)
    , view(PointsView(this)) {
    set_name("points");

    add_validator(new IntegerPositiveValidator());
}

void PointsProperty::draw() {
    view.draw();
}
