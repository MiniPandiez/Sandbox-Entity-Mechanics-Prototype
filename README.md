# Sandbox Entity Mechanics Prototype

A compact gameplay simulation prototype focused on autonomous entity behavior, spatial reasoning, and collision resolution. The project emphasizes clean C++ logic over art-heavy presentation, while keeping the simulation performant enough for 500+ active agents.

## Features

- 500 autonomous entities with randomized motion patterns
- Spatial collision detection and resolution
- Bitfield-driven state flags for behavior control
- Output of simulation state to CSV for analysis and visualization
- Python plotting utility for quick result inspection
- CMake-based project setup for Visual Studio and CLI builds

## Build and run

### Windows with Visual Studio

1. Open the folder in Visual Studio 2022.
2. Use the CMake Tools integration or configure with CMake presets.
3. Build the target `sandbox_entity_mechanics`.

### Command line

```bash
cmake -S . -B build
cmake --build build --config Release
./build/sandbox_entity_mechanics
```

## Output files

The simulator writes live entity data to:

```text
output/entity_state.csv
```

You can visualize the final state with Python:

```bash
python3 -m pip install -r requirements.txt
python3 scripts/visualize.py --input output/entity_state.csv --output output/entity_positions.png
```

## Project goals

This prototype demonstrates:

- linear algebra for motion and trajectory simulation
- collision response using simple impulse-based resolution
- optimized memory usage via compact entity storage
- clean, modular gameplay logic suitable for a portfolio project
