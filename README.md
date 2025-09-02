# 🚗 Speedometer Project

This project is a **client–server system** that simulates a car dashboard.  
It was developed as part of a school assignment and demonstrates real-time communication between a server and a client application.

---

## ✨ Overview
- **Server**: simulates a dashboard and generates car data  
  (speed, temperature, battery level, light signals).  
-  **Client**: displays this data in a GUI with icons, colors, and sound effects.  
-  **Protocols**: communication works over **UART (Serial)** or **TCP/IP**.  
-  **Architecture**: abstract communication layer makes it easy to switch between protocols.  

---

##  Tech Stack
- **C++ / Qt6** → GUI, multimedia, and serial communication  
- **ESP32 / PlatformIO** → firmware for client & server boards  
- **CMake** → build system for both desktop apps & microcontrollers  

---


## 🚀 How to Run
### Build desktop applications
```bash
cmake -S . -B build -DUARTCOM=ON   # or OFF for TCP
cmake --build build --target client server
```

Build ESP32 firmware (UART mode only)
```bash
cd build
make build      # compile ESP32 client + server (no device required)
make upload            # build & flash to ESP32 boards
```

🎥 Demo

A short demo video is included in this repository (demo.mp4) showing the client and server in action.


👤 Author

Developed by Parham
Originally based on a collaborative school project.
