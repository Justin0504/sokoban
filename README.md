# Sokoban Solver

A C++ implementation of an efficient solver for simplified Sokoban puzzles. This program analyzes grid-based maps to find the shortest sequence of moves that pushes all boxes to their designated target positions.

## Features

- **Map Parsing**: Reads grid-based maps from input files with support for various elements:
  - Walkable paths (.)
  - Impassable walls (#)
  - Player starting position (S)
  - Box positions (B)
  - Target positions (T)
  - Boxes on targets

- **Pathfinding Algorithm**: Employs an optimized search strategy to navigate the player through the grid while pushing boxes to their destinations.

- **Deadlock Detection**: Incorporates intelligent detection of unsolvable states to avoid unnecessary computation.

- **Efficient State Management**: Uses compact state representation to handle complex puzzle configurations within memory constraints.

## Implementation Details

- Written in modern C++17 following strict coding standards
- Memory-efficient state representation
- Handles puzzles with up to 8 boxes on grids up to 800 cells
- Outputs optimal solution sequences using directional notation (U, D, L, R)
- Includes comprehensive error checking for invalid map configurations

## Build Requirements

- C++17 compatible compiler (g++ recommended)
- Linux environment
- Standard C++ libraries

## Usage

The solver can process map files and output the shortest solution sequence or identify unsolvable configurations with "No solution!".
