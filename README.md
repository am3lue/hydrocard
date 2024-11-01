# 💧 Hydrocard: RFID-Based Water Dispensing System with Keypad Control
_By [am3lue](mailto:am3lue@gmail.com)_

**Hydrocard** is a smart water dispensing system using RFID cards and a keypad for controlled access. Authorized users can dispense water based on their balance, which is managed and displayed in real time. Perfect for shared or community spaces needing secure water access!

---

## 🌊 Key Features
- **RFID Access Control**: Only users with authorized RFID cards can activate Hydrocard.
- **Keypad Dispensing Control**: Select dispensing duration via keypad.
- **Balance Management**: Each user has a unique balance tracked and saved in memory.
- **User-Friendly LCD Display**: Real-time feedback, including welcome messages, balance, and more.

---

## 🧰 Hardware Requirements
- **Arduino** (Uno, Mega, etc.)
- **MFRC522 RFID Module**
- **4x4 Keypad**
- **16x4 I2C LCD Display**
- **Solenoid Valve & Relay** (for water control)
- **External Power Supply** (as needed for the valve)
- **Wires and Breadboard**

## 🖥 Software Requirements
- [Arduino IDE](https://www.arduino.cc/en/software) with the following libraries:
  - `MFRC522` – For RFID handling
  - `Keypad` – For user input through keypad
  - `LiquidCrystal_I2C` – For LCD display control
  - `EEPROM` – For balance memory
  - `SPI` – For RFID communication

> **Tip**: Install each library from the Arduino IDE using **Sketch > Include Library > Manage Libraries…**

---

## 🎛 Setup

### Circuit Connections
1. **RFID Module**: Connect to Arduino’s SPI pins.
2. **Keypad**: Connect rows and columns to designated Arduino digital pins.
3. **LCD Display**: Connect via I2C (SDA and SCL pins).
4. **Solenoid Valve & Relay**: Connect relay to a designated output pin (`valvePin`).

### UID Configuration
- In the code, replace placeholder UIDs with actual UIDs of each authorized RFID card. Each UID corresponds to a specific user who will be welcomed by name!

---

## 🧑‍💻 Code Overview

### 📂 Variables & Structs
- **`authorizedCards`** – Stores UIDs and names of registered users.
- **`balance`** – Manages each user’s balance, saved in EEPROM.
- **`isAuthorized`** – Tracks whether the current user is authorized.

### 📜 Functions
- **`setup()`**: Initializes RFID, LCD, and loads balance from EEPROM.
- **`loop()`**: Main loop for card detection and keypad interaction.
- **`handleCard()`**: Checks if scanned card is authorized and activates the keypad.
- **`dispenseWater(int duration)`**: Manages water dispensing time and updates balance.
- **`isAuthorizedCard()`**: Verifies if the card is in the authorized list.
- **`compareUID()`**: Compares UIDs for verification.
- **`display()` & `key()`**: Helper functions for LCD updates.

---

## 🚀 Using Hydrocard

1. **Power on** the system.
2. **Scan RFID Card** – Hydrocard will recognize the user’s card and display their name and balance.
3. **Select Dispense Time** – Use the keypad (1-9) to set dispensing time in seconds.
4. **Dispense Water** – Water will flow for the chosen duration, and the balance will update accordingly.
5. **End Session** – Press `#` to cancel, or scan another card to reset.

---

## 📟 Example LCD Display

On startup:
Please scan card to activate keypad...


After card scan:
Welcome, Francis Masanja Balance: 950


While dispensing water:
Dispensing water... Remaining Balance: 920



---

## ⚠️ Troubleshooting
- **RFID Issues**: Ensure SPI wiring is correct (SDA, SCK, MOSI, MISO).
- **Keypad Unresponsive**: Verify row and column connections.
- **LCD Not Displaying**: Check I2C address (usually `0x27`) and connections.
- **Balance Not Updating**: Check EEPROM functionality.

---

## 📜 License
Hydrocard is open-source and free for personal and educational use. Feel free to adapt and enhance!

## ✨ Acknowledgments
Special thanks to the developers behind `MFRC522`, `Keypad`, `LiquidCrystal_I2C`, and the open-source community for inspiration and support.

---

_Developed with 💙 by [am3lue](mailto:am3lue@gmail.com)_
