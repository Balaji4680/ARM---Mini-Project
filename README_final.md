<div align="center">

# 🔥 Kitchen Safety
## Heat and Gas Monitoring System

**A password-protected kitchen safety monitor built on the LPC2148 (ARM7)**

<img src="images/badge_mcu.svg" alt="MCU: LPC2148 ARM7" height="36">
<img src="images/badge_ide.svg" alt="IDE: Keil µVision" height="36">
<img src="images/badge_language.svg" alt="Language: Embedded C" height="36">

[Overview](#1-overview) · [How it works](#2-how-it-works) · [Hardware](#3-hardware) · [Wiring](#4-wiring-and-pin-connections) · [Build](#6-build-and-flash) · [Usage](#7-how-to-use-the-system) · [Testing](#8-testing-checklist) · [Help](#11-troubleshooting)

</div>

---

## 📑 Table of Contents

1. [Overview](#1-overview)
2. [How it works](#2-how-it-works)
3. [Hardware](#3-hardware)
4. [Wiring and pin connections](#4-wiring-and-pin-connections)
5. [Software and project files](#5-software-and-project-files)
6. [Build and flash](#6-build-and-flash)
7. [How to use the system](#7-how-to-use-the-system)
8. [Testing checklist](#8-testing-checklist)
9. [Changing the default settings](#9-changing-the-default-settings)
10. [Code structure](#10-code-structure)
11. [Troubleshooting](#11-troubleshooting)
12. [Limitations and future work](#12-limitations-and-future-work)
13. [FAQ](#13-faq)
14. [Author](#author)

---

## 1. Overview

Gas leaks and overheated cooking areas are among the most common causes of kitchen accidents. This project watches both at the same time. It reads a **LM35** temperature sensor and an **MQ-2** gas sensor, shows the values and the current time on a 16x2 LCD, and switches on a **buzzer and LED** the moment either value goes above its limit.

All settings (clock, alarm limits, password) sit behind a keypad password and are changed from an on-screen menu.

<div align="center">

<img src="https://github.com/user-attachments/assets/77b90455-faad-443c-9160-079423600d65" width="560" alt="Block diagram">

<sub><b>Figure 1.</b> Block diagram of the system</sub>

</div>

### ✨ Features

| 🌡️ Live monitoring | 🕒 Real-time clock | 🚨 Instant alarm |
|:---|:---|:---|
| Temperature and gas are read continuously | Time and date kept by the board's RTC battery | Buzzer and LED turn ON when a limit is crossed |

| 📢 Alert message | 🔇 Alarm mute | 📜 Event history |
|:---|:---|:---|
| `ALERT!!` with `TEMP IS HIGH!` or `GAS IS HIGH!` for 2.5 s | Switch 2 silences buzzer and LED | Last alarm (time + value) pops up every 10 s for 3 s |

| 🔐 Password menu | ⛔ Auto-lock | ⚙️ Settings menu |
|:---|:---|:---|
| Settings open only with the correct password | 3 wrong tries lock the system for 10 s | Clock, limits and password editable from the keypad |

### 🎯 Default values

| Temperature limit | Gas limit | Password | Menu time-out |
|:---:|:---:|:---:|:---:|
| **40 °C** | **300** (scale 0–1023) | **`1234`** | **30 s** |

> [!CAUTION]
> This is a learning and prototype project. It is not a certified gas or fire detector, so do not rely on it as your only safety device.

---

## 2. How it works

<div align="center">

<img src="images/system_architecture.svg" width="860" alt="System architecture">

<sub><b>Figure 2.</b> Inputs, the LPC2148 and its internal blocks, and the outputs</sub>

</div>

### Sensing

| Sensor | What it gives | How the program uses it |
|---|---|---|
| **LM35** | 10 mV for every 1 °C (for example 400 mV at 40 °C) | The ADC reads the voltage and `LM35tC()` converts it to °C |
| **MQ-2** | An analog voltage that rises with gas or smoke concentration | The ADC reads it on channel 2 as a value from 0 to 1023 |

### Decision rules

| Condition | Result |
|---|---|
| Temperature is above the temperature limit | Temperature alarm |
| Gas reading is above the gas limit | Gas alarm |
| Reading crosses its limit for the first time | Buzzer and LED ON, alert shown for 2.5 s, event saved |
| Reading falls back to the limit or below | Buzzer and LED OFF |
| Switch 2 pressed | Buzzer and LED muted |

### Alarm logic

<div align="center">

<img src="images/alarm_logic_flow.svg" width="860" alt="Alarm logic flow">

<sub><b>Figure 3.</b> What happens on every pass through the danger check</sub>

</div>

### Main loop

<div align="center">

<img src="images/main_loop_flow.svg" width="860" alt="Main loop flow">

<sub><b>Figure 4.</b> The six steps repeated forever in <code>main()</code></sub>

</div>

---

## 3. Hardware

| # | Component | Purpose |
|:-:|---|---|
| 1 | LPC2148 ARM7 board (Vector India *Advanced Development Board for ARM7*) | Controller, LCD, buzzer, LEDs, switches, RTC |
| 2 | 16x2 character LCD | Shows everything (on the board) |
| 3 | 4x4 matrix keypad | Password and number entry |
| 4 | MQ-2 gas sensor module | Detects gas and smoke |
| 5 | LM35 temperature sensor | Measures temperature (10 mV per °C) |
| 6 | Buzzer | Audible alarm (on the board) |
| 7 | External 5 V / 3.3 V power supply board | Powers sensors and board |
| 8 | Jumper wires and a small perfboard | Connections |

<div align="center">

<img src="https://github.com/user-attachments/assets/79616819-5b8a-40f1-8a41-15ab2f2a48fe" width="760" alt="Labeled hardware overview">

<sub><b>Figure 5.</b> Labeled photo of the complete setup</sub>

<br><br>

<img src="https://github.com/user-attachments/assets/2c2352c4-acf6-4f5b-af07-5bb2c9a17e3a" width="760" alt="Hardware with keypad">

<sub><b>Figure 6.</b> Setup with the keypad connected through the ribbon cable (marked 7 in Figure 5)</sub>

</div>

### Keypad layout

```
┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ A │
├───┼───┼───┼───┤
│ 4 │ 5 │ 6 │ B │
├───┼───┼───┼───┤
│ 7 │ 8 │ 9 │ C │   C = delete last digit
├───┼───┼───┼───┤
│ * │ 0 │ # │ D │   # = confirm (any non-digit key works)
└───┴───┴───┴───┘
```

---

## 4. Wiring and pin connections

<div align="center">

<img src="images/wiring_setup.svg" width="860" alt="Wiring and pin connections">

<sub><b>Figure 7.</b> Which sensor, switch or output goes to which LPC2148 pin</sub>

</div>

These pins come from the `#define` lines at the top of `src/main.c`.

| Function | LPC2148 pin | Type and notes |
|---|:-:|---|
| Buzzer | **P0.28** (`BUZZER_PIN`) | Output |
| Alarm LED | **P0.30** (`LED_PIN`) | Output |
| Switch 1, open settings menu | **P0.1** | Hardware interrupt (EINT0) |
| Switch 2, mute alarm | **P0.3** (`SWITCH2_PIN`) | Input, active-low (pressed = 0) |
| MQ-2 gas sensor | **P0.29** (AIN2) | Analog input, ADC channel 2 |
| LM35 temperature sensor | ADC pin set in `LM35.c` | Analog input (write the exact pin and channel here) |
| LCD, keypad, RTC | On-board | Handled by `LCD.c`, `KPM.c`, `RTC.c` |

> [!IMPORTANT]
> If you change a pin in the `#define` lines, move the physical wire too. Never put two functions on one pin.

> [!WARNING]
> LPC2148 analog inputs accept **3.3 V at most**. If your MQ-2 module can output more than 3.3 V on its analog pin, reduce it first (for example with a resistor divider) or confirm the module's output range before connecting.

### Power connections

| Part | Supply |
|---|---|
| LM35 | 5 V, GND, output to the ADC pin |
| MQ-2 module | 5 V, GND, analog output (AOUT) to P0.29 |
| Board | From the external 5 V / 3.3 V supply board |

---

## 5. Software and project files

| Tool | Use |
|---|---|
| **Keil µVision (MDK-ARM)** | Write, compile and build (the code uses the Keil `__irq` keyword and `<lpc21xx.h>`) |
| **Flash Magic** | Load the `.hex` file over the serial (UART) port |
| **USB-to-serial cable or the board's DB9 port** | PC to board connection for flashing |

### Repository layout

```
kitchen-safety-system/
├── README.md
├── src/
│   └── main.c                <- main program
└── images/
    ├── badge_mcu.svg  badge_ide.svg  badge_language.svg
    ├── system_architecture.svg
    ├── wiring_setup.svg
    ├── main_loop_flow.svg
    ├── alarm_logic_flow.svg
    └── menu_flow.svg
```

### Driver files used by `main.c`

Keep these in the **same Keil project** as `main.c`, including their `.c` files:

| Header | Provides |
|---|---|
| `types.h` | `u32`, `s32`, `u8`, `f32` type names |
| `delay.h` | `delay_ms()` |
| `ADC.h`, `ADC_defines.h` | ADC setup and `Read_ADC()` |
| `LCD.h`, `LCD_defines.h` | LCD commands and printing (`StrLCD`, `U32LCD`, `S32LCD`, …) |
| `LM35.h` | `LM35tC()`, returns temperature in °C |
| `KPM.h` | Keypad functions (`Init_KPM`, `KeyScan`, `ColScan`) |
| `RTC.h` | RTC functions (`RTC_Init`, `GetRTCTimeInfo`, `SetRTCTimeInfo`, …) |

---

## 6. Build and flash

<details open>
<summary><b>Step 1 – Create the Keil project</b></summary>

1. Open Keil µVision, then **Project → New µVision Project**.
2. Choose a folder and a name, and select the device **NXP → LPC2148**.
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

Press **F7** and fix errors until you see `0 Error(s)`. A `.hex` file appears in the output folder.
</details>

<details open>
<summary><b>Step 5 – Flash the board</b></summary>

1. Connect the board to the PC with the serial cable and power it on.
2. Move the **ISP switch** to the *program* position and press **RST**.
3. Open **Flash Magic**, choose device **LPC2148**, the correct **COM port**, baud rate **9600** (or as your lab specifies), and select the `.hex` file.
4. Click **Start** and wait for *Finished*.
5. Move the ISP switch back to the *run* position and press **RST**.

The Vector ID and the project title should now appear on the LCD.
</details>

---

## 7. How to use the system

### 7.1 Start-up
The LCD first shows the **Vector ID**, then the project title scrolls across the second line. After that the normal screen appears.

### 7.2 Normal screen

```
HH:MM:SS T:xx°C
DD/MM/YYYY S:x
```

`T:` is the temperature in °C. `S:` is the gas status, **0 = safe** and **1 = gas above limit**.

<div align="center">

| Gas safe (`S:0`) | Gas above limit (`S:1`) |
|:---:|:---:|
| <img src="https://github.com/user-attachments/assets/53544444-2789-4eed-94f6-908e30ecabc0" width="340" alt="Normal screen, gas safe"> | <img src="https://github.com/user-attachments/assets/b28a05b3-c1b9-410f-a31b-5f4dc8a626a8" width="340" alt="Normal screen, gas high"> |

</div>

### 7.3 Alarm and alert

When temperature or gas **first goes above its limit**:

1. Buzzer and LED turn **ON**.
2. The LCD shows an alert for **2.5 seconds**.
3. The normal screen returns. Buzzer and LED stay ON until the reading falls back to the limit or below.

Press **Switch 2** to mute. Every 10 seconds the last alarm event (time and value) is shown for 3 seconds, and the buzzer stays silent during that popup.

<div align="center">

| Temperature alert | Gas alert |
|:---:|:---:|
| <img src="https://github.com/user-attachments/assets/d4a71d7b-f4ef-4066-8e9c-954abd9936b2" width="340" alt="Temperature alert"> | <img src="https://github.com/user-attachments/assets/ad606707-162c-4ead-bd1c-d4ffdd96124c" width="340" alt="Gas alert"> |

</div>

### 7.4 Open the settings menu

1. Press **Switch 1**.
2. Type the password on the keypad (digits appear as `*`).
3. Press any non-digit key such as `#` to confirm. **C** deletes the last digit.

<div align="center">

<img src="https://github.com/user-attachments/assets/73f7bf1b-e85e-4f7b-8dcf-b772f4bb9476" width="380" alt="Password entry">

<sub><b>Password entry screen</b></sub>

| ❌ Wrong password | 🔒 Three wrong tries |
|:---:|:---:|
| <img src="https://github.com/user-attachments/assets/f9d041b2-028b-4795-b4df-8f5851c40004" width="340" alt="Access denied"> | <img src="https://github.com/user-attachments/assets/375fb9cf-d285-4342-b072-3fec7f63a1e7" width="340" alt="Locked"> |
| *Access Denied* and a short beep | Locked for 10 s with a countdown, then the password is asked again |

</div>

### 7.5 Settings menu

```
1.RTC 2.SET  30     <- number = seconds left to choose
3.PASS 4.EXIT
```

<div align="center">

<img src="https://github.com/user-attachments/assets/9d6e0b7c-2833-43a3-9e56-59e80ac557b0" width="420" alt="Settings menu">

<br><br>

<img src="images/menu_flow.svg" width="860" alt="Password and settings menu flow">

<sub><b>Figure 8.</b> From Switch 1 to the settings menu, including the lock-out path</sub>

</div>

> [!NOTE]
> The menu closes by itself after **30 seconds** without a key press.

| Key | Option | What it does |
|:-:|---|---|
| **1** | RTC | Set the clock and date |
| **2** | SET | `1.TEMP` sets the temperature limit (0–200 °C), `2.GAS` sets the gas limit (0–1023) |
| **3** | PASS | Enter the current password, then the new one, then confirm it |
| **4** | EXIT | Return to the normal screen |

<details>
<summary><b>🕒 RTC menu keys and ranges</b></summary>

| Key | Sets | Allowed range |
|:-:|---|:-:|
| 1 | Hour | 0 – 23 |
| 2 | Minute | 0 – 59 |
| 3 | Second | 0 – 59 |
| 4 | Date | 1 – 31 |
| 5 | Month | 1 – 12 |
| 6 | Year | 2000 – 2099 |
| 7 | Exit | Back to the previous menu |

Type the number and press a non-digit key (for example `#`) to save. Wrong values show *Invalid! Retry*.

<div align="center">
<img src="https://github.com/user-attachments/assets/9bdc7a63-8c4c-44e0-b05f-64fb92acc3c6" width="420" alt="RTC menu">
</div>
</details>

---

## 8. Testing checklist

Use this list after flashing to confirm every feature works.

| # | Test | How | Expected result |
|:-:|---|---|---|
| 1 | Start-up | Power on | Vector ID, scrolling title, then the normal screen |
| 2 | Clock | Watch the first line | Seconds count up; time survives a power-off if the RTC battery is fitted |
| 3 | Temperature reading | Hold the LM35 gently between warm fingers | `T:` value rises |
| 4 | Temperature alarm | Warm the LM35 above the limit (or lower the limit in the menu) | Buzzer and LED ON, `TEMP IS HIGH!` for 2.5 s |
| 5 | Gas alarm | Bring an unlit lighter close to the MQ-2 and release a very short burst of gas (or lower the gas limit in the menu) | `S:1`, buzzer and LED ON, `GAS IS HIGH!` for 2.5 s |
| 6 | Mute | Press Switch 2 during an alarm | Buzzer and LED go OFF |
| 7 | Event popup | Wait 10 s after an alarm | Last event (time + value) shown for 3 s |
| 8 | Password | Press Switch 1, enter `1234`, press `#` | Settings menu opens |
| 9 | Wrong password | Enter a wrong code three times | *Access Denied* beeps, then a 10 s lock |
| 10 | Menu time-out | Open the menu and wait | Returns to the normal screen after 30 s |

> [!TIP]
> Never test with a flame or heated appliances too close to the board. Lowering a limit in the menu is the safest way to trigger an alarm.

---

## 9. Changing the default settings

Open `src/main.c` and edit the `#define` lines at the top, then rebuild and flash:

```c
#define TEMP_LIMIT        40     // alarm above this temperature (°C)
#define GAS_LIMIT         300    // alarm above this gas reading (0-1023)
#define DEFAULT_PASSWORD  1234   // starting password
#define MENU_WAIT_TIME    30000  // menu time-out (ms)
#define POPUP_EVERY       10000  // last-event popup interval (ms)
#define POPUP_FOR         3000   // last-event popup duration (ms)
```

The alert duration is the `delay_ms(2500)` line inside `check_for_danger()`.

---

## 10. Code structure

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
| 11 | `check_for_danger()` compares readings with limits and shows the alert |
| 12 | Normal screen |
| 13 | Password, number entry, RTC, set-point and password menus, lock-out |
| 14 | `main()` loop (see Figure 4) |

---

## 11. Troubleshooting

<details>
<summary><b>Click to expand the problem table</b></summary>

| Problem | Likely cause and fix |
|---|---|
| LCD is blank | Adjust the contrast potentiometer near the LCD; check the LCD data wires |
| Temperature wrong or very high | Check the LM35 wiring (5 V, GND, output) and the conversion in `LM35.c` |
| Gas alert always on | Let the MQ-2 warm up for a minute or two; adjust its on-board potentiometer or raise `GAS_LIMIT` |
| Keypad gives wrong keys | Check ribbon cable orientation and the key table in `KPM.c` |
| Time resets to 00:00:00 after power-off | Check the RTC coin-cell battery on the board |
| Cannot flash | Check COM port, baud rate and that the ISP switch is in program mode |
| Menu never opens | Check the Switch 1 wire on P0.1 and that the interrupt setup runs |
| Buzzer silent | Check the buzzer pin wire; make sure Switch 2 (mute) was not pressed |
| Time runs too fast or slow | PCLK is not 15 MHz; adjust `T0PR` (see Step 3 of the build) |

</details>

---

## 12. Limitations and future work

> [!WARNING]
> The password, temperature limit and gas limit are kept in RAM only. **They return to the defaults after a power cycle.**

- The sensors are not read while the 2.5 s alert or another delay screen is showing.
- The keypad password is numeric only.

**Ideas for improvement**

| Idea | Benefit |
|---|---|
| Save settings in flash or EEPROM | Limits and password survive a power cycle |
| SMS or app notification | Alert reaches you when you are away |
| Relay to close a gas valve | Automatic protection, not just a warning |
| Second gas or flame sensor | Fewer missed events |

---

## 13. FAQ

**Which key confirms a number or password?**
Any non-digit key. `#` is the easiest to remember.

**Why does `S:` show only 0 or 1?**
It is a status flag for gas, not the raw value. The raw value (0–1023) is compared with `GAS_LIMIT`.

**How do I change the password if I forget it?**
Passwords are stored in RAM only, so switching the board off and on restores the default `1234`.

**Can I use a different gas sensor?**
Yes, if it gives an analog voltage of 3.3 V or less. Connect it to P0.29 or change the pin and ADC channel in the code.

---

<div align="center">

### 👤 Author
**Balaji Pidikiti**

</div>
