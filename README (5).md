<p align="center">
  <img width="2400" height="640" alt="banner" src="https://github.com/user-attachments/assets/df69613c-2ad4-4f9f-a1e1-e99e69ea15f9" />

</p>

<p align="center">
  <b>Smart gas-leak and overheat protection for your kitchen.</b>
</p>

<p align="center">
  A kitchen safety monitor built around the <b>LPC2148 (ARM7)</b> microcontroller.<br/>
  An <b>LM35</b> measures temperature while an <b>MQ-2</b> detects gas and smoke, and both are checked continuously.<br/>
  If either reading crosses its limit, the <b>buzzer and LED</b> switch on and the <b>LCD</b> shows what went wrong.<br/>
  All settings are protected by a <b>keypad password</b>, so only you can change them.
</p>

<p align="center">
  <img alt="MCU" src="https://img.shields.io/badge/MCU-LPC2148%20(ARM7)-blue?style=for-the-badge"/>
  <img alt="IDE" src="https://img.shields.io/badge/IDE-Keil%20%C2%B5Vision-orange?style=for-the-badge"/>
  <img alt="Language" src="https://img.shields.io/badge/Language-Embedded%20C-green?style=for-the-badge"/>
  <img alt="Sensors" src="https://img.shields.io/badge/Sensors-LM35%20%7C%20MQ--2-red?style=for-the-badge"/>
</p>

<p align="center">
  <img width="601" height="440" alt="Block diagram of the kitchen safety system" src="https://github.com/user-attachments/assets/77b90455-faad-443c-9160-079423600d65" />
  <br/>
  <sub><i>System block diagram</i></sub>
</p>

---

## 📑 Table of Contents

1. [Overview](#-overview)
2. [Hardware Required](#-hardware-required)
3. [Pin Connections](#-pin-connections)
4. [Software Required](#-software-required)
5. [Project Structure](#-project-structure)
6. [Build and Flash](#-build-and-flash)
7. [How to Use](#-how-to-use)
8. [Customising the Defaults](#-customising-the-defaults)
9. [Code Organisation](#-code-organisation)
10. [Troubleshooting](#-troubleshooting)
11. [Limitations and Future Work](#-limitations-and-future-work)
12. [Author](#-author)

---

## 🎯 Overview

<img width="2400" height="200" alt="sec-overview" src="https://github.com/user-attachments/assets/d8ecc50d-fbde-4c38-9075-62ea1e63f230" />


The system continuously reads a **LM35** temperature sensor and an **MQ-2** gas sensor. The live values and the real-time clock appear on a 16x2 LCD. If either reading goes above its limit, the buzzer and LED turn on and an alert message is displayed. All settings are protected by a keypad password and are edited from an on-screen menu.

### 📌 At a Glance

| 2 | 2.5 s | 3 | 30 s |
|:---:|:---:|:---:|:---:|
| **Sensors**<br/>LM35 + MQ-2 | **Alert message**<br/>shown when a limit is first crossed | **Wrong tries**<br/>then the system locks for 10 s | **Menu time-out**<br/>with no key press |

### 🧭 How It Works

<p align="center">
  <img width="2400" height="760" alt="how-it-works" src="https://github.com/user-attachments/assets/0f31bb2d-dccb-4b82-8317-bd13778ae6e2" />

</p>

### ⚡ Quick Start

1. Build the project in Keil and flash the `.hex` file ([details](#-build-and-flash)).
2. Power the board and wait for the Vector ID and title to finish.
3. Watch the live time, temperature (`T:`) and gas status (`S:`) on the LCD.
4. Press **Switch 1** and enter the default password `1234` to open the settings menu.

### ✨ Features

| Feature | Description |
|---|---|
| 📊 **Live monitoring** | Reads temperature and gas level continuously |
| 🕒 **Real-time clock** | Shows time and date, kept running by the board's RTC battery |
| 🚨 **Alarm** | Buzzer and LED turn ON when temperature or gas goes above its limit |
| ⚠️ **Alert message** | `ALERT!!` with `TEMP IS HIGH!` or `GAS IS HIGH!` is shown for 2.5 s when a limit is first crossed |
| 🔇 **Alarm mute** | Switch 2 silences the buzzer and LED |
| 📜 **Event history** | The last alarm (time + value) pops up every 10 s for 3 s |
| 🔐 **Password protection** | The settings menu opens only with the correct password |
| 🔒 **Auto-lock** | 3 wrong passwords lock the system for 10 s |
| ⚙️ **Settings menu** | Change the clock, temperature/gas limits and password from the keypad |

### 🔧 Default Values

| Item | Default |
|---|---|
| Temperature limit | `40 °C` |
| Gas limit | `300` (scale 0–1023) |
| Password | `1234` |
| Menu time-out | `30 seconds` |

---

## 🧰 Hardware Required

<img width="2400" height="200" alt="sec-hardware" src="https://github.com/user-attachments/assets/7d7fd86e-03fe-478c-8be7-5bff3de47b93" />


| # | Component | Purpose |
|:-:|---|---|
| 1 | LPC2148 ARM7 development board (Vector India *Advanced Development Board for ARM7*) | Main controller, on-board LCD, buzzer, LEDs, switches, RTC |
| 2 | 16x2 character LCD | Displays everything (on the board) |
| 3 | 4x4 matrix keypad | Enter the password and numbers |
| 4 | MQ-2 gas sensor module | Detects gas / smoke |
| 5 | LM35 temperature sensor | Measures temperature (10 mV per °C) |
| 6 | Buzzer | Audible alarm (on the board) |
| 7 | External 5 V / 3.3 V power supply board | Powers the sensors and the board |
| 8 | Jumper wires, small perfboard | Connections |

### 📷 Hardware Photos

<p align="center">
  <img width="700" alt="Labeled hardware overview" src="https://github.com/user-attachments/assets/79616819-5b8a-40f1-8a41-15ab2f2a48fe" />
  <br/>
  <sub><b>Labeled hardware overview</b></sub>
</p>

The 4x4 keypad plugs into the board through the flat ribbon cable (marked **7** in the labeled photo).

---

## 🔌 Pin Connections

These pins come from the `#define` lines at the top of `src/main.c`.

| Function | LPC2148 Pin | Notes |
|---|---|---|
| Buzzer | `P0.28` (`BUZZER_PIN`) | Output |
| Alarm LED | `P0.30` (`LED_PIN`) | Output |
| Switch 1 – open settings menu | `P0.1` | Hardware interrupt (EINT0) |
| Switch 2 – mute alarm | `P0.3` (`SWITCH2_PIN`) | Input, active-low (pressed = 0) |
| MQ-2 gas sensor | `P0.29` (AIN2) | Analog input, ADC channel 2 |
| LM35 temperature sensor | ADC pin used by `LM35.c` | Analog input *(write the exact pin/channel here)* |
| LCD, keypad, RTC | On-board connections | Handled by `LCD.c`, `KPM.c`, `RTC.c` |

> [!IMPORTANT]
> If you change a pin in the `#define` lines, change the physical wire as well. Never put two functions on the same pin.

---

## 💻 Software Required

| Tool | Use |
|---|---|
| **Keil µVision** (ARM/MDK) | Write, compile and build the project (the code uses the Keil `__irq` keyword and `<lpc21xx.h>`) |
| **Flash Magic** | Send the `.hex` file to the LPC2148 through the serial (UART) port |
| **USB-to-serial cable or the board's DB9 port** | Connection between PC and board for flashing |

---

## 📁 Project Structure

```
kitchen-safety-system/
├── README.md              <- this file
├── src/
│   └── main.c             <- the main program
└── images/                <- banners, diagrams and photos used in this README
```

`main.c` also uses your lab's driver files. Keep them in the **same Keil project** as `main.c`:

| Header | Provides |
|---|---|
| `types.h` | `u32`, `s32`, `u8`, `f32` type names |
| `delay.h` | `delay_ms()` |
| `ADC.h`, `ADC_defines.h` | ADC setup and `Read_ADC()` |
| `LCD.h`, `LCD_defines.h` | LCD commands and printing (`StrLCD`, `U32LCD`, `S32LCD`, …) |
| `LM35.h` | `LM35tC()` – returns temperature in °C |
| `KPM.h` | Keypad functions (`Init_KPM`, `KeyScan`, `ColScan`) |
| `RTC.h` | RTC functions (`RTC_Init`, `GetRTCTimeInfo`, `SetRTCTimeInfo`, …) |

Copy the matching `.c` files of these drivers into the project folder too, and add them to the Keil project.

---

## 🚀 Build and Flash

<img width="2400" height="200" alt="sec-build" src="https://github.com/user-attachments/assets/e5ad4837-888f-4de9-a85e-c94c10536f14" />


<details open>
<summary><b>Step 1 – Create the Keil project</b></summary>

1. Open Keil µVision → **Project → New µVision Project**.
2. Choose a folder, give it a name, and select the device **NXP → LPC2148**.
3. When asked to copy the *Startup file*, click **Yes**.
</details>

<details open>
<summary><b>Step 2 – Add the source files</b></summary>

1. Copy `main.c` and all driver `.c` / `.h` files into the project folder.
2. In the *Project* panel, right-click **Source Group 1 → Add Existing Files to Group** and add `main.c` plus every driver `.c` file.
</details>

<details open>
<summary><b>Step 3 – Set the clock and create the HEX file</b></summary>

1. **Project → Options for Target → Target**: set the crystal (Xtal) to **12 MHz**.
2. Open the **Output** tab and tick **Create HEX File**.
3. The code assumes a peripheral clock (PCLK) of **15 MHz** (`T0PR = 14999` gives a 1 ms timer tick). If your startup configuration uses a different PCLK, adjust this value.
</details>

<details open>
<summary><b>Step 4 – Build</b></summary>

Press **F7**. Fix any errors until you see `0 Error(s)`. A `.hex` file appears in the output folder.
</details>

<details open>
<summary><b>Step 5 – Flash the board</b></summary>

1. Connect the board to the PC with a serial cable and power it on.
2. Move the **ISP switch** to the *program* position and press **RST**.
3. Open **Flash Magic**, choose device **LPC2148**, the correct **COM port**, baud rate **9600** (or as your lab specifies), and select the `.hex` file.
4. Click **Start** and wait for *Finished*.
5. Move the ISP switch back to the *run* position and press **RST**.

The Vector ID and project title should now appear on the LCD.
</details>

---

## 🕹️ How to Use

<img width="2400" height="200" alt="sec-usage" src="https://github.com/user-attachments/assets/8ef565f2-9129-4e52-aaee-7a2dd5113511" />


### 1. Start-up

The LCD first shows the **Vector ID**, then the project title scrolling across the second line. After that the normal screen appears.

### 2. Normal Screen

```
HH:MM:SS T:xx°C
DD/MM/YYYY S:x
```

- `T:` is the temperature in °C.
- `S:` is the gas status: **0 = safe**, **1 = gas above limit**.

<table align="center">
  <tr>
    <th align="center" width="50%">Gas safe (<code>S:0</code>)</th>
    <th align="center" width="50%">Gas above limit (<code>S:1</code>)</th>
  </tr>
  <tr>
    <td align="center"><img width="100%" alt="Normal screen, gas safe" src="https://github.com/user-attachments/assets/53544444-2789-4eed-94f6-908e30ecabc0" /></td>
    <td align="center"><img width="100%" alt="Normal screen, gas high" src="https://github.com/user-attachments/assets/b28a05b3-c1b9-410f-a31b-5f4dc8a626a8" /></td>
  </tr>
</table>

### 3. Alarm and Alert

The two screens below show what to expect: the normal screen, and the alert for whichever reading goes high.

<p align="center">
  <img width="2400" height="640" alt="alarm-states" src="https://github.com/user-attachments/assets/954e7e23-d66a-423f-8ce0-64c621b58827" />

</p>

When temperature or gas **first goes above its limit**:

1. The buzzer and LED turn ON.
2. The LCD shows an alert for 2.5 seconds.
3. The event (time + value) is saved and the LCD returns to the normal screen.
4. The buzzer and LED stay ON until the reading falls back to the limit or below, or until **Switch 2** is pressed to mute them.

Real photos from the board:

<table align="center">
  <tr>
    <th align="center" width="50%">Temperature alert</th>
    <th align="center" width="50%">Gas alert</th>
  </tr>
  <tr>
    <td align="center"><img width="100%" alt="Temperature alert screen" src="https://github.com/user-attachments/assets/d4a71d7b-f4ef-4066-8e9c-954abd9936b2" /></td>
    <td align="center"><img width="100%" alt="Gas alert screen" src="https://github.com/user-attachments/assets/ad606707-162c-4ead-bd1c-d4ffdd96124c" /></td>
  </tr>
</table>

> **Last-event popup:** every 10 seconds, the last alarm event (time and value) is shown for 3 seconds. The buzzer stays silent during the popup.

#### Alarm flow

<p align="center">
  <img width="2400" height="880" alt="flow-alarm" src="https://github.com/user-attachments/assets/567ca460-8c6e-4483-a16c-7126c1801b5d" />

</p>

### 4. Open the Settings Menu

<img width="2400" height="200" alt="sec-security" src="https://github.com/user-attachments/assets/9a1a8c5e-0880-40c4-aec3-59fb8a70aef4" />


1. Press **Switch 1**.
2. Type the password on the keypad (digits appear as `*`).
3. Press any non-digit key (for example `#`) to confirm. Press **C** to delete the last digit.

<table align="center">
  <tr>
    <th align="center" width="33%">Password entry</th>
    <th align="center" width="33%">Wrong password</th>
    <th align="center" width="33%">Three wrong tries</th>
  </tr>
  <tr>
    <td align="center"><img width="100%" alt="Password entry screen" src="https://github.com/user-attachments/assets/73f7bf1b-e85e-4f7b-8dcf-b772f4bb9476" /></td>
    <td align="center"><img width="100%" alt="Access denied screen" src="https://github.com/user-attachments/assets/f9d041b2-028b-4795-b4df-8f5851c40004" /></td>
    <td align="center"><img width="100%" alt="System locked countdown" src="https://github.com/user-attachments/assets/375fb9cf-d285-4342-b072-3fec7f63a1e7" /></td>
  </tr>
  <tr>
    <td align="center"><sub>Digits appear as <code>*</code></sub></td>
    <td align="center"><sub><i>Access Denied</i> and a short beep</sub></td>
    <td align="center"><sub>Locked for 10 s with a countdown, then the password is asked again</sub></td>
  </tr>
</table>

### 5. Settings Menu

```
1.RTC 2.SET  30     <- number = seconds left to choose
3.PASS 4.EXIT
```

<table align="center">
  <tr>
    <th align="center" width="50%">Settings menu</th>
    <th align="center" width="50%">RTC menu</th>
  </tr>
  <tr>
    <td align="center"><img width="330" alt="Settings menu" src="https://github.com/user-attachments/assets/9d6e0b7c-2833-43a3-9e56-59e80ac557b0" /></td>
    <td align="center"><img width="330" alt="RTC menu" src="https://github.com/user-attachments/assets/9bdc7a63-8c4c-44e0-b05f-64fb92acc3c6" /></td>
  </tr>
  <tr>
    <td align="center"><sub>Choose 1 to 4</sub></td>
    <td align="center"><sub>Set hour, minute, second, date, month, year</sub></td>
  </tr>
</table>

The menu closes by itself after **30 seconds** without a key press.

| Key | Option | What it does |
|:-:|---|---|
| **1** | RTC | Set the clock and date (see [RTC menu](#6-rtc-menu)) |
| **2** | SET | `1.TEMP` – set the temperature limit (0–200 °C), `2.GAS` – set the gas limit (0–1023) |
| **3** | PASS | Change the password: enter the current password, then the new one, then confirm it |
| **4** | EXIT | Return to the normal screen |

#### Access and menu flow

<p align="center">
  <img width="2400" height="960" alt="flow-menu" src="https://github.com/user-attachments/assets/ddc88c57-2822-4e09-ab62-586c1ce97b64" />

</p>

### 6. RTC Menu

| Key | Sets | Allowed range |
|:-:|---|---|
| **1** | Hour | 0 – 23 |
| **2** | Minute | 0 – 59 |
| **3** | Second | 0 – 59 |
| **4** | Date | 1 – 31 |
| **5** | Month | 1 – 12 |
| **6** | Year | 2000 – 2099 |
| **7** | Exit | Back to the previous menu |

Type the number and press a non-digit key (for example `#`) to save. Wrong values show *Invalid! Retry*.

---

## 🛠️ Customising the Defaults

Open `src/main.c` and edit the `#define` lines at the top:

```c
#define TEMP_LIMIT        40     // alarm above this temperature (°C)
#define GAS_LIMIT         300    // alarm above this gas reading (0-1023)
#define DEFAULT_PASSWORD  1234   // starting password
#define MENU_WAIT_TIME    30000  // menu time-out in milliseconds
#define POPUP_EVERY       10000  // last-event popup interval (ms)
#define POPUP_FOR         3000   // last-event popup duration (ms)
```

Rebuild and flash again after any change. The alert duration is the `delay_ms(2500)` line inside `check_for_danger()`.

---

## 🧩 Code Organisation

`main.c` is split into numbered sections, so you can read it top to bottom:

| Section | Content |
|:-:|---|
| 1 | Settings (`#define`) |
| 2–3 | Event type and shared variables |
| 4 | 1 ms software clock using Timer 0 |
| 5 | Buzzer, LED and Switch 2 |
| 6 | Switch 1 interrupt (opens the menu) |
| 7–8 | Gas sensor and temperature display helper |
| 9 | Start-up splash screens |
| 10 | Saving and showing the last alarm event |
| 11 | `check_for_danger()` – compares readings with limits and shows the alert |
| 12 | Normal screen |
| 13 | Password, number entry, RTC, set-point and password menus, lock-out |
| 14 | `main()` – the main loop |

### Main loop

<p align="center">
  <img width="2400" height="680" alt="flow-main-loop" src="https://github.com/user-attachments/assets/cb46b944-7d0b-4f69-8a96-c05e45456391" />

</p>

---

## 🩺 Troubleshooting

<img width="2400" height="200" alt="sec-troubleshoot" src="https://github.com/user-attachments/assets/4d0b9023-558f-48cf-99c5-19abba676771" />


| Problem | Likely cause and fix |
|---|---|
| LCD is blank | Adjust the contrast potentiometer near the LCD; check the LCD data wires |
| Temperature always shows a wrong or very high value | Check the LM35 wiring (5 V, GND, output) and the ADC conversion in `LM35.c` |
| Gas alert always on | Let the MQ-2 warm up for a minute or two; adjust the sensor's on-board potentiometer or raise `GAS_LIMIT` |
| Keypad gives wrong keys | Check the ribbon cable orientation and the key table in `KPM.c` |
| Time resets to 00:00:00 after power-off | Check the RTC battery (coin cell on the board) |
| Cannot flash | Check COM port, baud rate and that the ISP switch is in program mode |
| Menu never opens | Check the Switch 1 wire on `P0.1` and that the interrupt setup runs |
| Buzzer silent | Check the buzzer pin wire; make sure Switch 2 (mute) was not pressed |

---

## 🔭 Limitations and Future Work

**Limitations**

- The password, temperature limit and gas limit are stored in RAM only. **They return to the defaults after a power cycle.**
- While the 2.5-second alert or a delay screen is showing, the sensors are not read.
- The keypad password is numeric only.

**Possible improvements**

- [ ] Store settings in flash / EEPROM
- [ ] Add SMS or app notifications
- [ ] Add a relay to switch off a gas valve

---

## 👤 Author

**TEJA REDDY**

<p align="center"><sub>⭐ If you found this project useful, consider giving it a star.</sub></p>
