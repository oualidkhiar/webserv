#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void initTmpFiles() {
    const std::string marker = "/tmp/myserver_initialized";

    if (fs::exists(marker)) {
        std::cout << "Tmp files already initialized.\n";
        return; // already done
    }

    std::cout << "Initializing tmp files...\n";

    // Example: create some tmp files
    std::ofstream("/tmp/file1.txt") << "Hello, world!\n";
    std::ofstream("/tmp/file2.txt") << "Server temp file\n";

    // Create marker file to indicate initialization is done
    std::ofstream(marker) << "done\n";

    std::cout << "Tmp files created.\n";
}

int main() {
    initTmpFiles();

    // ... rest of server startup ...
}