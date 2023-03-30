#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include <string>
#include <fstream>
#include <optional>
#include <filesystem>
#include <exception>
#include <unordered_map>


class ValidationError : public std::exception {
protected:
    const std::string err;

public:
    ValidationError(const std::string& d): err(
        fmt::format("Validation error: {}", d)
    ) {}

    const char* what() const throw() override {
        return err.c_str();
    }
};

template <typename T> class Validator {
public:
    virtual void validate(T) = 0;

};

class FilePathValidator: public Validator<std::string> {
    void validate(std::string path) override {
        spdlog::debug("Performing validation of {}", path);
        std::filesystem::path fp = path;
        if (std::filesystem::exists(fp)) {
            spdlog::debug("Success");
            return;
        }
        else {
            spdlog::debug("Fail");
            throw ValidationError(
                fmt::format("Path {} does not lead to a file", path)
            );
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
        spdlog::debug("Performing validation of {}", val);
        if (min < val < max) {
            spdlog::debug("Success");
            return;
        }
        else {
            spdlog::debug("Fail");
            throw ValidationError(
                fmt::format("{} is not in between {} and {}", val, min, max)
            );
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
        spdlog::debug("Performing validation of {}", str);
        for(std::string& ch: choices) {
            if(!str.compare(ch)) {
                spdlog::debug("Success");
                return;
            }
        }

        spdlog::debug("Fail");
        throw ValidationError(
            fmt::format("{} is not one of the valid choices", str)
        );
    }
};


class IntegerPositiveValidator: public Validator<int> {
public:
    void validate(int i) override {
        spdlog::debug("Performing validation of {}", i);
        if (i >= 0) {
            spdlog::debug("Success");
            return;
        }
        else {
            spdlog::debug("Fail");
            throw ValidationError(
                fmt::format("{} is not a positive integer", i)
            );
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
        return fmt::format("{} ({})", get_name(), get_data());
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
    std::unordered_map<std::string, EditorObject> objects = {};

public:
    JsonParser(const std::string& p) : path(p) {}

    bool parse() {
        std::ifstream file;
        nlohmann::json data;

        file.open(path);
        // TODO: more detailed error handling
        if (!file.good()) {
            spdlog::error("Unable to open {}", path);
            return false;
        }

        try {
            file >> data;
        }
        catch (const nlohmann::detail::parse_error& err) {
            spdlog::error(err.what());

            return false;
        }

        try {
            for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
                std::string obj_name = it.key();
                if (objects.find(obj_name) != objects.end()) {
                    spdlog::warn(
                        "Already parsed object with name {}, skipping",
                        obj_name
                    );
                }
                EditorObject obj = EditorObject(obj_name);
                auto props = it.value();

                // TODO: maybe add different error handling options - skip,
                // replace with default, etc
                for (nlohmann::json::iterator pit = props.begin(); pit != props.end(); pit++) {
                    std::string key = pit.key();

                    if (!key.compare("icon")) {
                        Icon* i;
                        try {
                            i = new Icon(pit.value());
                            i->serialize();
                        }
                        catch (ValidationError& v_err) {
                            spdlog::warn(v_err.what());
                            continue;
                        }
                        obj.add_icon(i);
                    }
                    else if (!key.compare("speed")) {
                        Speed* sp;
                        try {
                            sp = new Speed(
                                pit.value()["value"],
                                pit.value()["min"],
                                pit.value()["max"]
                            );
                            sp->serialize();
                        }
                        catch (ValidationError& v_err) {
                            spdlog::warn(v_err.what());
                            continue;
                        }
                        obj.add_speed(sp);
                    }
                    else if (!key.compare("material")) {
                        Material* mat;
                        try {
                            mat = new Material(
                                pit.value()["value"],
                                pit.value()["choices"]
                            );
                            mat->serialize();
                        }
                        catch (ValidationError& v_err) {
                            spdlog::warn(v_err.what());
                            continue;
                        }
                        obj.add_material(mat);
                    }
                    else if (!key.compare("points")) {
                        Points* pts;
                        try {
                            pts = new Points(
                                pit.value()
                            );
                            pts->serialize();
                        }
                        catch (ValidationError& v_err) {
                            spdlog::warn(v_err.what());
                            continue;
                        }
                        obj.add_points(pts);
                    }
                    else {
                        spdlog::warn(
                            "Unable to find parse rules for key {}", pit.key()
                        );
                    }
                }
                spdlog::info("Parsed object: {}", obj.to_string());
                objects.insert(std::make_pair(obj_name, obj));
            }
        }
        catch (const nlohmann::detail::type_error& err) {
            spdlog::warn(err.what());

            return false;
        }

        return true;
    }

    const std::unordered_map<std::string, EditorObject>& get_parsed_objects() {
        return objects;
    }
};



int main(int argc, char* const* argv) {
    spdlog::set_pattern("[%H:%M:%S][%l]%$ %v");

    std::string path = "";

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (!std::string(argv[i]).compare("--json")) {
                if (i+1 < argc) {
                    path = argv[i+1];
                    break;
                }
            }
        }
    }
    if (!path.compare("")) {
        spdlog::error("Expected a path after '--json'");
        return 1;
    }

    spdlog::info("Attempting to load json on path {}", path);
    JsonParser jp = JsonParser(path);
    jp.parse();

    auto i = jp.get_parsed_objects();

    spdlog::info("Done. Successfully fetched {} objects", i.size());

    return 0;
}
