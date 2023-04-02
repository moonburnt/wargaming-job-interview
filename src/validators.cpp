#include "validators.hpp"

ValidationError::ValidationError(const std::string& d): err(
    fmt::format("Validation error: {}", d)
) {}

const char* ValidationError::what() const throw() {
    return err.c_str();
}


void FilePathValidator::validate(std::string path) {
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


void FloatInRangeValidator::set_min(float m) {
    min = m;
}
void FloatInRangeValidator::set_max(float m) {
    max = m;
}

void FloatInRangeValidator::validate(float val) {
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


void TextChoicesValidator::set_choices(std::vector<std::string> c) {
    choices = c;
}

void TextChoicesValidator::validate(std::string str) {
    spdlog::debug("Performing validation of {}", str);

    if (choices.empty()) {
        spdlog::debug("Fail");
        throw ValidationError(
            fmt::format("Choices list must have at least one item")
        );
    }

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


void IntegerPositiveValidator::validate(int i) {
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
