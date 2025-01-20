# Arduino TVout Simulator

This project allows you to experiment and compile your Arduino TVout game or other experiments on a PC using SDL2. It is not perfectly emulate Atmega, but provides a framework and API to simulate Arduino's TVout library, enabling development and testing without the need for physical hardware. The inputs are simulate through mouse, game controllers and/or keyboard.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Contributing](#contributing)
- [License](#license)

## Installation

To set up the project, clone the repository and ensure you have SDL2 development files installed on your system.

```bash
git clone https://github.com/yorak/arduino-tvout-simulator.git
cd arduino-tvout-simulator
# Follow platform-specific instructions to install SDL2
```

## Usage

Compile and run the demo Arduino TVout project on your PC.

```bash
make
bin/tvout_simulator_demo
```

To create your own apps, take a look at `demo.cpp`, create similar file, modify Makefile, compile and run.

## Features

- **Arduino Simulation**: PC implementations Arduino functions such as `pinMode`, `digitalWrite`, `digitalRead`, and more.
- **TVout Functions**: Supports TVout functions like `draw_line`, `draw_rect`, `draw_circle`, and more.
- **SDL2 Integration**: Utilizes SDL2 for rendering and input handling.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
