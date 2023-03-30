#include "err_logger.hpp"
#include "spdlog/spdlog.h"


void ExceptionLogger::log_exception(const std::string& exc) {
    spdlog::warn(exc);
    exception_messages.push_back(exc);
}

void ExceptionLogger::log_exception(std::exception* exc) {
    log_exception(exc->what());
}

const std::vector<std::string>& ExceptionLogger::get_messages() {
    return exception_messages;
}

void ExceptionLogger::clear() {
    exception_messages.clear();
}

ExceptionLogger& ExceptionLogger::get_logger() {
    static ExceptionLogger logger;
    return logger;
}
