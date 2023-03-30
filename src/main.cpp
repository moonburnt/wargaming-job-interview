#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "objects.hpp"
#include <string>


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

    ObjectStorage storage;
    storage.from_json_file(path);

    spdlog::info("Done. Successfully fetched {} objects", storage.size());

    storage.to_json_file("./out.json");

    return 0;
}
