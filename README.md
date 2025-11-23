# ⚡ Electronics Laboratory - STM32 Projects & Serial Communication

Welcome to the **Electronics Laboratory** repository. This project contains the firmware source code and host-side scripts developed for the laboratory experiments using **STM32H Series** microcontrollers (e.g., STM32H7/H5 Nucleo boards).

The repository is divided into two main parts:
1.  **Firmware (C/C++):** Projects developed in **STM32CubeIDE** utilizing the HAL library.
2.  **Host Software (Python):** Scripts for **Serial (UART) communication**, data acquisition, and real-time visualization.

---

## 📂 Repository Structure

```text
├── Firmware/                 # STM32CubeIDE Projects (C Source Code)
│   ├── Lab1_GPIO/            # Digital I/O experiments
│   ├── Lab2_ADC_PWM/         # Analog reading and signal generation
│   ├── Lab3_SerialCom/       # UART implementation
│   └── Drivers/              # STM32 HAL Drivers and CMSIS
│
├── Python_Scripts/           # Host-side scripts
│   ├── data_logger.py        # Script to save serial data to CSV
│   ├── real_time_plot.py     # Live plotting of incoming data
│   └── serial_control.py     # Send commands to the STM32
│
└── README.md                 # This documentation

## 🛠️ Hardware Requirements
To replicate these experiments, you will need:

Microcontroller: STM32H Series Nucleo / Discovery Board (e.g., STM32H743ZI).
Connection: USB Cable (Micro-USB or USB-C depending on the board) for programming and UART communication.
Components: Breadboard, Jumper wires, Resistors, LEDs, Sensors (as per specific lab instructions).

## 💻 Software Prerequisites

1. For Firmware (STM32)
STM32CubeIDE: Download here.
Drivers: ST-LINK USB drivers (usually installed with CubeIDE).
Terminal (Optional): PuTTY or TeraTerm for raw debugging.
2. For Host Scripts (Python)
Python 3.8+: Ensure Python is added to your system PATH.
Libraries: You need pyserial for communication and matplotlib/pandas for data handling.
Install the required Python packages using:

code
Bash
pip install pyserial matplotlib pandas numpy

## 🚀 Getting Started

Part A: Flashing the Firmware
Clone the Repository:
code
Bash
git clone https://github.com/[YOUR_USERNAME]/[REPO_NAME].git
Open STM32CubeIDE.
Go to File > Import > General > Existing Projects into Workspace.
Select the Firmware/ directory from this repo.
Build the project (Hammer icon) and Run/Debug (Green Play button) to flash the code onto the board.
Part B: Running Python Serial Scripts
Once the board is flashing and connected via USB:

Identify the COM Port (Windows) or /dev/tty (Linux/Mac) assigned to the STM32.
Windows: Check Device Manager -> Ports (COM & LPT).
Linux: Run ls /dev/ttyACM*.
Navigate to the script folder:
code
Bash
cd Python_Scripts
Run the desired script (example):
code
Bash
python real_time_plot.py --port COM3 --baud 115200
⚠️ Note on Baud Rate: Ensure the baud rate in the Python script matches the huart.Init.BaudRate configured in the STM32 C code (default is usually 115200 or 9600).
📡 Serial Communication Protocol
The communication between the STM32 and Python follows a specific format (modify this section based on your actual code):

Data Format: ASCII Strings or Binary Packets.
End of Line: \n (Newline) or \r\n.
Example Output from STM32:
code
Text
DATA,1234,3.3V
Example Command to STM32:
Send '1' -> Turn LED ON.
Send '0' -> Turn LED OFF.
## 🐛 Troubleshooting
"Access Denied" Error (Python): Close any other serial monitors (like the console inside STM32CubeIDE) before running the Python script. Only one program can use the COM port at a time.
No Data Received: Check if the TX/RX jumper pins on the Nucleo board are connected correctly.
Garbage Characters: Verify that the Baud Rate is identical on both the STM32 code and the Python script.
👥 Authors & Contributors
[Your Name] - Firmware & Python Development
[Teammate Name] - Hardware Setup & Testing
