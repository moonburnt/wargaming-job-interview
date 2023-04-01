#pragma once

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include <string>


class MenuWindow {
public:
	bool is_open = true;
    bool must_die = false;
    virtual void draw() = 0;

    virtual void update(float) {}

    virtual ~MenuWindow() = default;
};


class ImGuiShowImageWindow: public MenuWindow {
private:
    const std::string name;
    Texture img;


public:
    ImGuiShowImageWindow(const std::string& n, const std::string& p)
        : name(n)
        , img(LoadTexture(p.c_str())) {}

    ~ImGuiShowImageWindow() {
        UnloadTexture(img);
    }

    void update(float) override {
        if (!is_open) {
            must_die = true;
        }
    }

    void draw() override {
        if (!is_open) {
            return;
        }

        if (ImGui::Begin(name.c_str(), &is_open, ImGuiWindowFlags_None)) {
            // rlImGuiImageRect(
            //     &img,
            //     static_cast<int>(img.width),
            //     static_cast<int>(img.height),
            //     { 0 }
            // );
        }
        ImGui::End();
    }
};
