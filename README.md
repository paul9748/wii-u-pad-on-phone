
# Cemu Wii U Gamepad Emulator for Smartphone

## Overview
This Node.js project aims to emulate a Wii U gamepad using a smartphone in Cemu, a Wii U emulator. The project includes features such as adding a virtual display, selecting a display on the client page, and sending gyro sensor data, connected joystick values, and touchscreen inputs to Cemu.
## Features
- Virtual display addition for emulating the Wii U Gamepad screen.
- Transmission of gyro sensor data from the smartphone to Cemu.
- Transmission of connected joystick values to Cemu.
- Forwarding touchscreen input from the smartphone to Cemu.

## Dependencies
- Node.js: This project is built using Node.js for server-side scripting.
- C++: Some functionalities are implemented using C++.
- [ParsecVdd](https://github.com/nomi-san/parsec-vdd): Used for certain aspects of the project. Download and install it before running the project.

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/cemu-gamepad-emulator.git
   cd cemu-gamepad-emulator
   ```

2. Install dependencies:
   ```bash
   npm install
   ```

3. Start the server:
   ```bash
   npm start
   ```

## Usage
1. Ensure that Cemu is properly configured on your system.
2. Open the client page on your smartphone by navigating to the provided URL.
3. Select the virtual display and configure other settings as needed.
4. Interact with the virtual Wii U Gamepad on your smartphone.

## Contributing
Feel free to contribute to the project by submitting issues or pull requests. Any contributions are welcome!

## Credits
This project is inspired by [ParsecVdd](https://github.com/nomi-san/parsec-vdd). Special thanks to the original author for their work.

## License
This project is licensed under the [MIT License](LICENSE).
