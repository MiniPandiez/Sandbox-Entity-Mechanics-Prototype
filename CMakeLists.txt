cmake_minimum_required(VERSION 3.16)

project(SandboxEntityMechanicsPrototype VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sandbox_entity_mechanics
    src/main.cpp
    src/Simulation.cpp
)

target_include_directories(sandbox_entity_mechanics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MSVC)
    target_compile_options(sandbox_entity_mechanics PRIVATE /W4)
else()
    target_compile_options(sandbox_entity_mechanics PRIVATE -Wall -Wextra -Wpedantic)
endif()
