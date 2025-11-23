# ⚡ Electronics Laboratory: STM32 Firmware & Python Serial Interface

Welcome to the **Electronics Laboratory** repository. This project contains the complete source code, documentation, and tools for laboratory experiments performed using **STM32H Series** microcontrollers (specifically STM32H7/H5 Nucleo boards).

The repository bridges the gap between low-level embedded C programming and high-level Python scripting, featuring firmware for the MCU and host-side scripts for data acquisition, control, and real-time visualization via **UART (Serial Communication)**.

---

## 📂 Repository Structure

The project is organized into two main directories to distinguish between the embedded firmware and the PC-side software.

```text
├── Firmware/                 # STM32CubeIDE Projects (C/C++ Source Code)
│   ├── Lab_GPIO/             # Basic Digital I/O and Interrupt experiments
│   ├── Lab_ADC_DMA/          # Analog signal acquisition using DMA
│   ├── Lab_UART/             # Serial communication implementation
│   └── Drivers/              # STM32 HAL Drivers and CMSIS
│
├── Python_Host/              # PC-side Scripts for Control & Analysis
│   ├── requirements.txt      # List of Python dependencies
│   ├── serial_reader.py      # Basic script to read raw serial data
│   ├── data_logger.py        # Saves incoming data to CSV/TXT files
│   └── realtime_plot.py      # Live visualization of sensor data
│
├── Schematics/               # Circuit diagrams and pinout configurations
└── README.md                 # Project Documentation
```

---

## 🛠️ Hardware Requirements

To replicate these experiments, the following hardware is required:

*   **Microcontroller:** STM32H Series Nucleo Board (e.g., NUCLEO-H743ZI, NUCLEO-H563ZI).
*   **Connection:** USB Cable (Type-C or Micro-USB) for ST-LINK programming and UART communication.
*   **Components:** Breadboards, Oscilloscope, Signal Generator, and specific components (Resistors, Capacitors, Op-Amps, Sensors) as defined in each lab folder.

---

## 💻 Software Prerequisites

### 1. Embedded Development (Firmware)
*   **STM32CubeIDE:** [Download here](https://www.st.com/en/development-tools/stm32cubeide.html).
*   **ST-LINK Drivers:** Usually installed automatically with CubeIDE.
*   **Terminal Emulator:** (Optional) PuTTY, TeraTerm, or Minicom for raw debugging.

### 2. PC Control (Python)
*   **Python 3.8+**: Ensure Python is installed and added to your system PATH.
*   **Libraries:** The scripts rely on `pyserial` for communication and scientific libraries for plotting.

To install the necessary Python dependencies, run:

```bash
pip install pyserial matplotlib pandas numpy
```

---

## 🚀 Getting Started

### Part A: Flashing the Firmware (STM32)

1.  **Clone the Repository:**
    ```bash
    git clone https://github.com/[YOUR_USERNAME]/[REPO_NAME].git
    ```
2.  Open **STM32CubeIDE**.
3.  Navigate to `File > Import > General > Existing Projects into Workspace`.
4.  Browse and select the specific project folder inside the `Firmware/` directory.
5.  **Build** the project (Hammer icon 🔨).
6.  **Run/Debug** (Green Play button ▶️) to flash the code onto the board.

### Part B: Running the Python Interface

Once the board is flashed and connected via USB, the STM32 will act as a Virtual COM Port.

1.  **Identify the Port:**
    *   **Windows:** Open Device Manager → Ports (COM & LPT) → Look for "STMicroelectronics STLink Virtual COM Port" (e.g., `COM3`).
    *   **Linux/Mac:** Run `ls /dev/tty.*` in the terminal (usually `/dev/ttyACM0` or `/dev/tty.usbmodem...`).

2.  **Run a Script:**
    Navigate to the Python folder and run a script.

    *Example: Real-time Plotting*
    ```bash
    cd Python_Host
    python realtime_plot.py
    ```
    *(Note: You may need to edit the `COM_PORT` variable inside the script to match your computer).*

---

## 📡 Communication Protocol Details

The communication relies on **UART** (Asynchronous Serial).

*   **Baud Rate:** `115200` (Default) or `9600` (Always check `main.c` -> `huart.Init.BaudRate`).
*   **Data Bits:** 8
*   **Parity:** None
*   **Stop Bits:** 1

> **⚠️ CRITICAL:** The Baud Rate in the Python script **MUST** match the Baud Rate configured in the STM32 Firmware. If they do not match, you will see garbage characters.

### Data Format Example
The STM32 typically sends data strings terminated by a newline character (`\n`) for easy parsing:

```text
TIMESTAMP,SENSOR_VALUE,VOLTAGE
1001,4095,3.30
1002,2048,1.65
```

---

## 🐛 Troubleshooting

*   **Error: `Access is denied` (Python):**
    *   Make sure you don't have the Serial Monitor open inside STM32CubeIDE, PuTTY, or another terminal. Only one program can access the COM port at a time.
*   **No Data Received:**
    *   Check physical connections (TX/RX jumpers on the Nucleo board, if modified).
    *   Reset the board (Black button) after starting the Python script.
*   **Garbage/Random Characters:**
    *   This is almost always a **Baud Rate mismatch**. Check the `main.c` file and ensure the Python script uses the exact same speed (e.g., 115200).

---

## 👥 Authors

*   **[Livio Lando]** - *Firmware & Python Development*
*   **[Davide Pievaioli]** - *Hardware Testing*

---
*Project developed for the Electronics Laboratory Course - [Bicocca university / 2025]*
