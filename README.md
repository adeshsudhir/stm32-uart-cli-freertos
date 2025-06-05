# STM32 FreeRTOS UART CLI

This project implements a basic UART-based Command Line Interface (CLI) using FreeRTOS on an STM32F407 microcontroller. It allows command parsing over UART with task prioritization using FreeRTOS. The application is built with STM32CubeMX, STM32 HAL, and CMake.

---

## 🚀 Features

- ✅ UART interrupt-based command reception
- ✅ FreeRTOS tasks with different priorities
- ✅ LED control commands via UART
- ✅ Real-time command parsing using FreeRTOS queues
- ✅ Shell-like output prompt (`>>`) for user interaction

---

## 🧠 Commands Supported

| Command   | Description               |
|----------|---------------------------|
| `COMMANDS` | Show all supported commands |
| `LED ON`   | Turn on the orange LED      |
| `LED OFF`  | Turn off the orange LED     |

---

## 📷 Debug Output

Below is a screenshot of the working CLI interaction captured via UART terminal:

![Debug Screenshot](debug%20output.png)

---

## 📁 Project Structure

```bash
├── Core/               # Main application source files
├── Drivers/            # STM32 HAL drivers
├── Middlewares/        # Optional middleware libraries
├── .vscode/            # VS Code project settings
├── CMakeLists.txt      # Main CMake build script
├── stm32_uart_pchost.ioc # STM32CubeMX project file
└── README.md
