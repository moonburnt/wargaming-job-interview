#include "view.hpp"
#include "misc/cpp/imgui_stdlib.h"
#include "objects.hpp"
#include "menu.hpp"


// IconView
IconView::IconView(IconProperty* p): parent(p) {}

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
        showing_texture = false;
        parent->set_value(current_txt);
        try {
            parent->validate();
        }
        catch (ValidationError& v_err) {
            ExceptionLogger::get_logger().log_exception(v_err.what());
        }
    }

    if (ImGui::Button(fmt::format("Show {}", parent->get_name()).c_str())) {
        showing_texture = true;
    }

    if (showing_texture) {
        std::optional<Texture> t = parent->get_texture();
        if (t.has_value()) {
            DrawTextureV(t.value(), {0.0f, 0.0f}, WHITE);
        }
    }
}


// SpeedView
SpeedView::SpeedView(SpeedProperty* p): parent(p) {}

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


// MaterialView
MaterialView::MaterialView(MaterialProperty* p): parent(p) {}

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


// PointsView
PointsView::PointsView(PointsProperty* p): parent(p) {}

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
