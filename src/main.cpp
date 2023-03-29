#include "nlohmann/json.hpp"
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <optional>
#include <filesystem>
#include <any>

template <typename T> class Validator {
public:
    virtual void validate(T) = 0;

};

class FilePathValidator: public Validator<std::string> {
    void validate(std::string path) override {
        // TODO: throw on err
        std::filesystem::path fp = path;
        if (std::filesystem::exists(fp)) {
            std::cout << "yes";
        }
    }
};


class FloatInRangeValidator: public Validator<float> {
protected:
    float min = 0.0f;
    float max = 0.0f;

public:
    void set_min(float m) {
        min = m;
    }
    void set_max(float m) {
        max = m;
    }

    void validate(float val) override {
        // TODO: throw on err
        if (min < val < max) {
            std::cout << "yes";
        }
    }
};


class TextChoicesValidator: public Validator<std::string> {
protected:
    std::vector<std::string> choices = {};

public:
    void set_choices(std::vector<std::string> c) {
        choices = c;
    }

    void validate(std::string str) override {
        // TODO: throw on err
        for(std::string& ch: choices) {
            if(!str.compare(ch)) {
                std::cout << "yes";
                return;
            }
        }
    }
};


class IntegerPositiveValidator: public Validator<int> {
public:
    void validate(int i) override {
        if (i >= 0) {
            std::cout << "yes";
        }
    }
};


template <typename TIn, typename TOut> class ObjectProperty {
protected:
    std::string name = "";
    // bool is_validated = false;
    // bool validation_result = false;
    TIn data;
    std::optional<TOut> validated_data;

    void set_name(const std::string& n) {
        name = n;
    }

    std::vector<Validator<TIn>*> validators = {};

public:


    // ObjectProperty(const std::string &n) : name(n) {};
    ObjectProperty(TIn i) : data(i) {}

    std::string get_name() {
        return name;
    }

    // TODO: del validators on delete
    virtual ~ObjectProperty() = default;


    void add_validator(Validator<TIn>* f) {
        validators.push_back(f);
    }

    // TODO: throw on error
    void perform_validation() {
        for (auto i: validators) {
            i->validate(data);
        }
    }

    // bool validate() {
    //     validation_result = perform_validation();
    //     is_validated = true;
    //     return validation_result;
    // };

    virtual void perform_serialization() = 0;

    // Validate data and serialize it into data field
    std::optional<TOut> serialize() {
        perform_validation();
        perform_serialization();
        return validated_data;
    }

    std::optional<TOut> get_validated_data() {
        // TODO: refuse to return data if is_validated is false
        return validated_data;
    }

    virtual std::string to_string() {
        return get_name();
    }

    TIn get_data() {
        return data;
    }
};

// Temporary. TODO: serialize into in-memory image
class Icon : public ObjectProperty<std::string, std::string> {
public:
    Icon(std::string p) : ObjectProperty(p) {
        set_name("icon");

        add_validator(new FilePathValidator());
    }

    // Temporary
    void perform_serialization() override {
        validated_data = data;
    }

    std::string to_string() override {
        return get_name() + "(" + get_data() + ")";
    }
};


class Speed : public ObjectProperty<float, float> {
public:
    Speed(float val, float min, float max) : ObjectProperty(val) {
        set_name("speed");

        FloatInRangeValidator* v = new FloatInRangeValidator();
        v->set_min(min);
        v->set_max(max);
        add_validator(v);
    }

    // Temporary
    void perform_serialization() override {
        validated_data = data;
    }

    std::string to_string() override {
        return get_name() + "(" + std::to_string(get_data()) + ")";
    }
};


class Material : public ObjectProperty<std::string, std::string> {
public:
    Material(std::string val, std::vector<std::string> choices) : ObjectProperty(val) {
        set_name("material");

        TextChoicesValidator* v = new TextChoicesValidator();
        v->set_choices(choices);
        add_validator(v);
    }

    // Temporary
    void perform_serialization() override {
        validated_data = data;
    }

    std::string to_string() override {
        return get_name() + "(" + get_data() + ")";
    }
};


class Points: public ObjectProperty<int, int> {
public:
    Points(int val) : ObjectProperty(val) {
        set_name("points");

        add_validator(new IntegerPositiveValidator());
    }

    // Temporary
    void perform_serialization() override {
        validated_data = data;
    }

    std::string to_string() override {
        return get_name() + "(" + std::to_string(get_data()) + ")";
    }
};


class EditorObject {
protected:
    std::string name;
    // TODO: store everything in a map. For now, this jank will do instead
    Icon* icon = nullptr;
    Speed* speed = nullptr;
    Material* material = nullptr;
    Points* points = nullptr;

public:
    EditorObject(const std::string& n) : name(n) {}

    // void add_prop(ObjectProperty<std::any, std::any>* prop) {
    //     props.push_back(prop);
    // }
    void add_icon(Icon* i) {
        icon = i;
    }

    void add_speed(Speed* sp) {
        speed = sp;
    }

    void add_material(Material* mat) {
        material = mat;
    }

    void add_points(Points* pts) {
        points = pts;
    }

    std::string get_name() {
        return name;
    }

    std::string to_string() {
        std::string ret = "EditorObject '" + get_name() + "': (";
        // for (auto i: props) {
        //     ret += i.get_name() + ", ";
        // }
        if (icon != nullptr) {
            ret += icon->to_string() + ", ";
        }
        if (speed != nullptr) {
            ret += speed->to_string() + ", ";
        }
        if (material != nullptr) {
            ret += material->to_string() + ", ";
        }
        if (points != nullptr) {
            ret += points->to_string() + ", ";
        }

        ret += ")";
        return ret;
    }
};

class JsonParser {
private:
    std::string path;

public:
    JsonParser(const std::string& p) : path(p) {}

    bool parse() {
        std::ifstream file;
        file.open(path);
        // TODO: more detailed error handling
        if (!file.good()) {
            std::printf("Unable to open %s\n", path.c_str());
            return false;
        }

        nlohmann::json data;
        // SavefileFields save_data;

        try {
            file >> data;
        }
        catch (const nlohmann::detail::parse_error& err) {
            std::printf("%s\n", err.what());

            return false;
        }

        try {
            for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
                EditorObject obj = EditorObject(it.key());
                auto props = it.value();
                // std::cout << obj.get_name() << ":" << props << "\n";

                for (nlohmann::json::iterator pit = props.begin(); pit != props.end(); pit++) {
                    std::string key = pit.key();

                    if (!key.compare("icon")) {
                        Icon* i = new Icon(pit.value());
                        i->serialize();
                        obj.add_icon(i);
                        // std::cout << props[key] << ":a" << pit.value() << "\n";

                        // std::count << obj.st
                    }
                    else if (!key.compare("speed")) {
                        // TODO: try/catch
                        Speed* sp = new Speed(
                            pit.value()["value"],
                            pit.value()["min"],
                            pit.value()["max"]
                        );
                        sp->serialize();
                        obj.add_speed(sp);
                        // std::cout << props[key] << ":a" << pit.value() << "\n";
                        // std::count << obj.st
                    }
                    else if (!key.compare("material")) {
                        Material* mat = new Material(
                            pit.value()["value"],
                            pit.value()["choices"]
                        );
                        mat->serialize();
                        obj.add_material(mat);
                    }
                    else if (!key.compare("points")) {
                        Points* pts = new Points(
                            pit.value()
                        );
                        pts->serialize();
                        obj.add_points(pts);
                    }
                    else {
                        std::cout << "Unable to find parse rules for key" << pit.key() << "\n";
                    }
                }
                std::cout << "Parsed object: " << obj.to_string() << "\n";
            }
        }
        catch (const nlohmann::detail::type_error& err) {
            std::printf("%s\n", err.what());

            return false;
        }

        // try {
        //     save_data = {
        //         data["player_stats"],
        //         data["dungeon_stats"],
        //         data["map_settings"],
        //         data["map_layout"]
        //     };
        // }
        // catch (const nlohmann::detail::type_error& err) {
        //     spdlog::warn(err.what());

        //     return false;
        // }

        // savefile = save_data;

        return true;
    }
};



int main(int argc, char* const* argv) {
    // Processing launch arguments.
    // For now there is just one - to toggle on debug messages.

    std::string path = "";

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (std::strcmp(argv[i], "--json") == 0) {
                if (i+1 < argc) {
                    path = argv[i+1];
                    break;
                }
            }
        }
    }
    if (std::strcmp(path.c_str(), "") == 0) {
        std::printf("Expected a path after '--json'\n");
        return 1;
    }

    std::printf("Attempting to load json on path %s\n", path.c_str());
    JsonParser jp = JsonParser(path);
    jp.parse();

    return 0;
}
