#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include <string>
#include <filesystem>
#include <exception>


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
