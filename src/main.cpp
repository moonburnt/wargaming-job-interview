#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "objects.hpp"
#include "window.hpp"
#include <string>

int main(int, char* const*) {
    AppWindow app;

    app.init();
    app.run();

    return 0;
}
