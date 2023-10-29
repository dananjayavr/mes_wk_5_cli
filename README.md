### Introduction

Port of Elecia White's [reusable](https://github.com/eleciawhite/reusable/) library to STM32F439ZI MCU. 

This port uses NUCLEO-F439ZI board and one of its UARTs connected to the Virtual COM port. No additional hardware is necessary (apart from a micro USB cable to connect the board to the computer).

> Note: In this project, the `Src/retarget.c` contains redefinitions of standard I/O functions to use UART. However, the reusable library port uses API calls from ST's HAL for data reception, instead of a built-in C functions such as `scanf()`.

Build instructions for the project can be found from the section  *Prerequisites* and beyond. 

#### How does this work ?

The main loop contain two functions/tasks. A `ledTask` to simulate a simple task being performed by the MCU and  `ConsoleProcess` task exposed by the reusable library. 

Before entering to the main loop, `ConsoleInit` function is called to initiate UART Tx in interrupt mode. Once the UART is initialized, the bulk of the data processing happens in the `ConsoleProcess` function.

`Src/reusable/consoleIo.c` file contains the majority of custom code for the STM32F4xx platform. The `ConsoleIoReceive` function process the `rx_buffer`, which contains the commands sent by the user from a host PC. 

The `HAL_UART_RxCpltCallback` function copies each character received through UART interface to the `rx_buffer`. Once it detects a complete command (i.e. some text followed by a newline/carriage return character), it signals the `ConsoleReceive` function to begin processing the buffer. 

#### Custom Commands

Three custom commands have been implemented in the `Src/reusable/consoleCommands.c` file. 

- `ConsoleCommandToggleIO` : will toggle the given GPIO pin
- `ConsoleCommandReadIO`  : will read the current state of the GPIO pin
- `ConsoleCommandWriteIO` : will set the value of a given GPIO pin

In order to avoid tightly coupling any HAL API calls to the reusable library code itself, within each `ConsoleCommandXXX` function, a weakly defined callback function mechanism is employed. 

For example, `ConsoleCommandToggleIO` function will parse the command and parameters supplied, makes sure the supplied command syntax is correct and finally calls the call-back function with the relevant parameters.

The call-back functions are defined in `Src/main.c` file. 

#### Improvements

The code as it stands can be improved in the following ways: 

- There's a fair bit of repetition in `Src/reusable/consoleCommands.c` file (inside custom command definitions). These could be refactored out to a common function.
- In the UART Tx interrupt call-back function, some edge-cases are not taken in to account. Such as user entering a command (obviously invalid) more than 256 characters long. 
- Perhaps the weakly defined call-back mechanism could be integrated to all commands.

------

### Prerequisites

- CMake
- ARM GNU GCC Tools
- MinGW

### Build the project (Manual)

- Create a build folder (ex. cmake-build-debug) and cd to that folder

- Run `cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=../arm-none-eabi-gcc.cmake -DCMAKE_BUILD_TYPE=Debug ..`

- Then, run `cmake --build . -- -j 4`

- You should obtain .bin and .hex files for your target

### Run project

- Use STM32CubeProgrammer to program the device.

### Build the project (with CLion)

- Open the folder containing CMakeLists.txt and *.cmake file with CLion.
- In File -> Settings -> Build, Execution, Deployment section, add the following to CMake options field: `-DCMAKE_TOOLCHAIN_FILE=arm-none-eabi-gcc.cmake`
- CLion should reload the CMake project without any errors.
- Build the project

#### Debug using OpenOCD and CLion

- Create a new run configuration (Run -> Edit Configurations)
- Add a new OpenOCD Download & Run configuration
- In the board config file field, supply the appropriate configuration file. Ex. https://github.com/ntfreak/openocd/blob/master/tcl/board/st_nucleo_f4.cfg
- Also make sure to fill the Executable field ex. template.out file in the build folder
- Pressing the Run button should now flash the target
- To debug, place a breakpoint and click on the green bug (debug) icon. The debugger should stop at the breakpoint

### Troubleshooting

- Undefined reference to _sbrk: https://stackoverflow.com/questions/5764414/undefined-reference-to-sbrk

### Sources

- https://github.com/posborne/cmsis-svd/blob/master/data/STMicro/STM32F401.svd
- https://dev.to/younup/cmake-on-stm32-the-beginning-3766
- https://dev.to/younup/cmake-on-stm32-episode-2-build-with-clion-2lae
- https://dev.to/younup/recompiler-vos-projets-c-et-c-plus-rapidement-avec-ccache-23in