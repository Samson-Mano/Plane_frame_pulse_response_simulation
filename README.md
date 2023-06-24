# Plane Frame Pulse Response Simulation

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

## Description

The Plane Frame Pulse Response Simulation is a C++ and OpenGL-based software tool for analyzing the dynamic behavior of plane frames subjected to pulse-type excitations. The program incorporates various features such as point masses, constraints, support inclination, and point loads in the member. It utilizes advanced numerical techniques to perform Modal analysis, Modal superposition, and Pulse response analysis, providing valuable insights into the structural response of plane frames.

## Features

- Interactive graphical user interface (GUI) using OpenGL for visualizing plane frames and simulation results.
- Support for point masses to accurately model the dynamic behavior of the structure.
- Various constraint options, including fixed, pinned, and roller supports, to simulate different support conditions.
- Ability to incorporate support inclination to account for real-world scenarios.
- Point load application capability on individual members for dynamic load analysis.
- Modal analysis to determine natural frequencies and mode shapes of the frame structure.
- Modal superposition for combining mode shapes to analyze complex dynamic excitation patterns.
- Pulse response analysis to investigate the structural response to pulse-type excitations.
- Visualization of displacements, member forces, mode shapes, and pulse response animations.

## Installation and Usage

### Prerequisites

- C++ compiler with C++17 support (e.g., GCC, Clang).
- OpenGL libraries and development headers.

### Installation Steps

1. Clone or download the repository: `git clone https://github.com/Samson-Mano/Plane_frame_pulse_response_simulation.git`.
2. Build the project using your C++ compiler. For example, with GCC: `g++ -std=c++11 -o main main.cpp`.
3. Run the executable file generated: `./main`.

### Usage

1. Launch the program.
2. Define the plane frame geometry, including members, nodes, and point masses.
3. Specify the constraints and support conditions for each node.
4. Apply point loads to individual members, if needed.
5. Run the simulation to obtain the pulse response analysis results.
6. Visualize the frame structure, displacements, member forces, mode shapes, and pulse response animations.
7. Analyze and interpret the simulation results for structural insights.

## Contributing

Im not çurrently looking for constribution to this project. But, If you would like to contribute new features, enhancements, or bug fixes, please follow these steps:

1. Fork the repository.
2. Create a new branch for your feature/fix: `git checkout -b feature-name`.
3. Implement your changes and ensure all tests pass.
4. Commit your changes: `git commit -m "Add feature/fix name"`.
5. Push to the branch: `git push origin feature-name`.
6. Open a pull request on GitHub.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.

## Contact

For any questions, suggestions, or inquiries, please feel free to reach out:

- Project Maintainer: Samson Mano
- Email: saminnx@gmail.com
- GitHub: [Samson Mano](https://github.com/Samson-Mano)
