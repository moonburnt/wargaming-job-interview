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
    ValidationError(const std::string& d);

    const char* what() const throw() override;
};

template <typename T> class Validator {
public:
    virtual void validate(T) = 0;

};

class FilePathValidator: public Validator<std::string> {
    void validate(std::string path) override;
};


class FloatInRangeValidator: public Validator<float> {
protected:
    float min = 0.0f;
    float max = 0.0f;

public:
    void set_min(float m);
    void set_max(float m);

    void validate(float val) override;
};


class TextChoicesValidator: public Validator<std::string> {
protected:
    std::vector<std::string> choices = {};

public:
    void set_choices(std::vector<std::string> c);

    void validate(std::string str) override;
};


class IntegerPositiveValidator: public Validator<int> {
public:
    void validate(int i) override;
};
