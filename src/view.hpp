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
    IconView(IconProperty* p);

    void draw();
};


class SpeedView {
private:
    SpeedProperty* parent;

    float current;
public:
    SpeedView(SpeedProperty* p);

    void draw();
};


class MaterialView {
private:
    MaterialProperty* parent;

    int current = 0;

public:
    MaterialView(MaterialProperty* p);

    void draw();
};


class PointsView {
private:
    PointsProperty* parent;

    int current = 0;

public:
    PointsView(PointsProperty* p);

    void draw();
};
