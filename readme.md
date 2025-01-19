# Arduino TVout Emulator

This project allows you to experiment and compile your Arduino TVout game or other experiments on a PC using SDL2. It provides a framework to emulate Arduino's TVout library, enabling development and testing without the need for physical hardware.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Contributing](#contributing)
- [License](#license)

## Installation

To set up the project, clone the repository and ensure you have SDL2 installed on your system.

```bash
git clone https://github.com/yourusername/arduino-tvout-emulator.git
cd arduino-tvout-emulator
# Follow platform-specific instructions to install SDL2
```

## Usage

Compile and run your Arduino TVout projects on your PC.

```bash
make
./your_project_executable
```

## Features

- **Arduino Emulation**: Emulates Arduino functions such as `pinMode`, `digitalWrite`, `digitalRead`, and more.
- **SDL2 Integration**: Utilizes SDL2 for rendering and input handling.
- **TVout Functions**: Supports TVout functions like `draw_line`, `draw_rect`, `draw_circle`, and more.
- **Random and Delay Functions**: Provides `arduinoRandom`, `delay`, and other utility functions.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
