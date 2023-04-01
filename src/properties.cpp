#include "properties.hpp"
#include "misc/cpp/imgui_stdlib.h"


void IconView::draw() {
    ImGui::Text(
        "%s",
        fmt::format(
            "{}: current value: {}, validated value: {}",
            parent->get_name(),
            parent->get_data(),
            parent->get_validated_data()
        ).c_str()
    );

    ImGui::InputText(
        fmt::format("Enter {} value", parent->get_name()).c_str(),
        &current_txt
    );

    if (ImGui::Button(fmt::format("Validate {}", parent->get_name()).c_str())) {
        parent->set_value(current_txt);
        try {
            parent->validate();
        }
        catch (ValidationError& v_err) {
            ExceptionLogger::get_logger().log_exception(v_err.what());
        }
    }
}


void SpeedView::draw() {
    ImGui::Text(
        "%s",
        fmt::format(
            "{}: current value: {}, validated value: {}",
            parent->get_name(),
            parent->get_data(),
            parent->get_validated_data()
        ).c_str()
    );

    ImGui::SliderFloat(
        fmt::format("Enter {} value", parent->get_name()).c_str(),
        &current,
        parent->get_min(),
        parent->get_max()
    );

    if (ImGui::Button(fmt::format("Validate {}", parent->get_name()).c_str())) {
        parent->set_value(current);
        try {
            parent->validate();
        }
        catch (ValidationError& v_err) {
            ExceptionLogger::get_logger().log_exception(v_err.what());
        }
    }
}

void MaterialView::draw() {
    ImGui::Text(
        "%s",
        fmt::format(
            "{}: current value: {}, validated value: {}",
            parent->get_name(),
            parent->get_data(),
            parent->get_validated_data()
        ).c_str()
    );

    // This can be optimized
    std::vector<std::string> choices = parent->get_choices();

    if (ImGui::BeginCombo(fmt::format("Enter {} value", parent->get_name()).c_str(), choices[current].c_str())) {
        for (int i = 0; i < static_cast<int>(choices.size()); i++) {
            const bool is_selected = (current == i);
            if (ImGui::Selectable(choices[i].c_str(), is_selected)) {
                current = i;
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::Button(fmt::format("Validate {}", parent->get_name()).c_str())) {
        parent->set_value(choices[current]);
        try {
            parent->validate();
        }
        catch (ValidationError& v_err) {
            ExceptionLogger::get_logger().log_exception(v_err.what());
        }
    }
}


void PointsView::draw() {
    ImGui::Text(
        "%s",
        fmt::format(
            "{}: current value: {}, validated value: {}",
            parent->get_name(),
            parent->get_data(),
            parent->get_validated_data()
        ).c_str()
    );


    ImGui::InputInt(
        fmt::format("Enter {} value", parent->get_name()).c_str(),
        &current
    );

    if (ImGui::Button(fmt::format("Validate {}", parent->get_name()).c_str())) {
        parent->set_value(current);
        try {
            parent->validate();
        }
        catch (ValidationError& v_err) {
            ExceptionLogger::get_logger().log_exception(v_err.what());
        }
    }
}



IconProperty::IconProperty(std::string p) : ObjectProperty(p), view(IconView(this)) {
    set_name("icon");

    add_validator(new FilePathValidator());
}

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


MaterialProperty::MaterialProperty(std::string val, std::vector<std::string> ch)
    : ObjectProperty(val)
    , view(MaterialView(this))
    , choices(ch) {
    set_name("material");

    TextChoicesValidator* v = new TextChoicesValidator();
    v->set_choices(choices);
    add_validator(v);
}



PointsProperty::PointsProperty(int val)
    : ObjectProperty(val)
    , view(PointsView(this)) {
    set_name("points");

    add_validator(new IntegerPositiveValidator());
}

