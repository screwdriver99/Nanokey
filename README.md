<h2>Nanokey</h2>
<img src="keyboard.png" align="left" width=30% title="keyboard icons">

<p align="left"><img src="https://img.shields.io/badge/compiler-arm--none--eabi--gcc-brightgreen?style=flat" alt="shields"> <img src="https://img.shields.io/badge/language-C%2Fasm-red?style=flat" alt="shields"> <img src="https://img.shields.io/badge/build_system-CMake-purple?style=flat" alt="shields"> <img src="https://img.shields.io/badge/platform-ARM-blue?style=flat" alt="shields"></p>

<p id="description" style="text-size:20">A minimal firmware for the Keychron K8 Pro keyboard</p>

  
### Features

*   Complete USB/Bluetooth support
*   Firmware upgrade via USB
*   Easily customizable light effects

<br><br><br>
<h2>Installation Steps 🛠️</h2>

1. make the build directory

```
mkdir build && cd build
```

2. make the build system

```
cmake ..
```

3. build the project

```
make
```

4. flash the firmware using a tool of your choice (openocd/STM32CubeProgrammer)


After the first Nanokey flash, it will be possible to boot to DFU keeping ESC pressed at startup.<br>


<h2>Roadmap</h2>

- [x] USB protocol
- [x] Bluetooth features
- [x] Battery management with emergency sleep state
- [ ] Improve the timing, removing the delayms in the event loop
- [ ] Implement OS switch feature (Win/MacOS)
- [ ] Use the three keys in the top-right corner of the keyboard (now unmapped)
- [ ] Implement consumer keys
- [ ] Implement recordable macros
- [ ] Make the device customizable by a standard user via gui app
- [ ] Rewrite the BT module firmware and its protocol

<h2>Contribution Guidelines</h2>

Feel free to improve the code and ask for a pull request

<h2>License 🛡️</h2>

This project contains some STM and Keychron code. 

Please refer to the licensing information contained in the relevant directories.

<br>
<h4>Credits</h4>

<a href="https://www.flaticon.com/free-icons/keyboard" title="keyboard icons">Keyboard icons created by Freepik - Flaticon</a>
