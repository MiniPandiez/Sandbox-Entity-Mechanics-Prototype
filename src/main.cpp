#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "Simulation.h"

int main() {
    constexpr float kWorldWidth = 1280.0f;
    constexpr float kWorldHeight = 720.0f;
    constexpr std::size_t kEntityCount = 500;
    constexpr float kDeltaTime = 1.0f / 60.0f;
    constexpr std::size_t kFrames = 300;

    Simulation simulation(kWorldWidth, kWorldHeight, kEntityCount);
    simulation.seedEntities(kEntityCount);

    const std::string outputPath = "output/entity_state.csv";
    simulation.run(kFrames, kDeltaTime, outputPath);

    std::cout << "Sandbox Entity Mechanics Prototype complete." << std::endl;
    std::cout << "Entities: " << kEntityCount << ", Frames: " << kFrames << std::endl;
    std::cout << "Output: " << outputPath << std::endl;

    return 0;
}
