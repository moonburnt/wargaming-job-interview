#include "properties.hpp"


IconProperty::IconProperty(std::string p) : ObjectProperty(p) {
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

