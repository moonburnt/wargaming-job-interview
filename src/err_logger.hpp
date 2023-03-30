#pragma once

#include <vector>
#include <exception>
#include <string>


class ExceptionLogger {
protected:
    std::vector<std::string> exception_messages = {};

public:
    void log_exception(std::exception* exc);
    void log_exception(const std::string& exc);

    const std::vector<std::string>& get_messages();

    // Not thread-safe, will go bananas if cleared mid-iteration
    void clear();

    // Globally accessible instance
    static ExceptionLogger& get_logger();
};
