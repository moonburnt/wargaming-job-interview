#pragma once


class MenuWindow {
public:
	bool is_open = true;
    bool must_die = false;
    virtual void draw() = 0;

    virtual void update(float) {}

    virtual ~MenuWindow() = default;
};
