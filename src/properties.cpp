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

    ImGui::InputText("", &current_txt);

    bool confirmed = true;

    if (ImGui::Button("Validate")) {
        parent->set_value(current_txt);
        try {
            parent->validate();
        }
        catch (ValidationError& v_err) {
            ExceptionLogger::get_logger().log_exception(v_err.what());
            confirmed = false;
        }

        if (confirmed) {
            current_txt = "";
        }
    }
}


IconProperty::IconProperty(std::string p) : ObjectProperty(p), view(IconView(this)) {
    set_name("icon");

    add_validator(new FilePathValidator());
}

SpeedProperty::SpeedProperty(float val, float min, float max) : ObjectProperty(val) {
    set_name("speed");

    FloatInRangeValidator* v = new FloatInRangeValidator();
    v->set_min(min);
    v->set_max(max);
    add_validator(v);
}


MaterialProperty::MaterialProperty(std::string val, std::vector<std::string> choices) : ObjectProperty(val) {
    set_name("material");

    TextChoicesValidator* v = new TextChoicesValidator();
    v->set_choices(choices);
    add_validator(v);
}



PointsProperty::PointsProperty(int val) : ObjectProperty(val) {
    set_name("points");

    add_validator(new IntegerPositiveValidator());
}

