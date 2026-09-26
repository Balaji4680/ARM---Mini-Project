# Kitchen Safety Heat and Gas Monitoring System

An embedded C project on the **LPC2148** (ARM7TDMI) microcontroller that continuously
monitors kitchen temperature and gas levels, raises an alert when either crosses a
safe limit, and lets the user reconfigure the system through a password-protected
on-device menu. Developed and simulated in **Proteus**.

---

## Features

- **Continuous monitoring** of temperature (LM35) and gas concentration (MQ2)
- **Live alert screen** the moment a limit is crossed (`ALERT!! / TEMP IS HIGH!` or
  `ALERT!! / GAS IS HIGH!`), with the buzzer and LED activated together
- **Event log (most recent)** — remembers which sensor triggered the last alert,
  its value, and the time it happened, shown automatically every 10 seconds
- **Two-screen startup splash** — a static ID screen followed by a scrolling project title
- **Password-protected settings menu**, opened by a hardware interrupt (Switch1):
  - Edit the RTC (hour, minute, second, date, month, year — one field at a time,
    each range-checked)
  - Edit the temperature/gas alert setpoints
  - Change the password (requires the current password first, then confirms the new one twice)
  - A live on-screen countdown while choosing an option; the menu times out and
    returns to normal monitoring if left idle
- **Lockout protection** — 3 wrong password attempts in a row triggers a 10-second
  lockout with a live on-screen countdown, then returns to the password prompt
- **Backspace-capable keypad entry** — the `C` key removes only the last digit typed,
  not the whole entry; password digits flash briefly before being masked with `*`
- **Switch2** silences the buzzer/LED at any time

---

## Hardware

| Component      | Notes                                      |
|----------------|---------------------------------------------|
| LPC2148        | ARM7TDMI-S microcontroller                  |
| 16x2 LCD       | Character display, 8-bit interface          |
| LM35           | Analog temperature sensor                    |
| MQ2            | Analog gas/smoke sensor                      |
| 4x4 matrix keypad | Menu navigation and numeric entry         |
| Buzzer + LED   | Combined alarm output                        |
| Switch1        | Hardware interrupt (EINT0) — opens the menu  |
| Switch2        | Silences the buzzer/LED                      |

### Pin Assignments

| Signal          | Pin    | Notes                                              |
|-----------------|--------|-----------------------------------------------------|
| LCD data/control| P0.8–P0.18 | 8-bit data bus + RS/RW/EN                      |
| LM35 (AIN0)     | P0.27  | ADC channel 0                                       |
| MQ2 (AIN2)      | P0.29  | ADC channel 2                                       |
| Keypad rows     | P1.16–P1.19 | —                                              |
| Keypad columns  | P1.20–P1.23 | —                                              |
| Switch1 (EINT0) | P0.1   | Active-low, hardware interrupt                      |
| Switch2         | P0.3   | Active-low, polled                                  |
| Buzzer          | P0.28  | *(confirm no conflict with ADC AIN1 on your board)* |
| LED             | P0.25  | |

> Double-check `BUZZER_PIN`/`LED_PIN` in `main.c` against your actual wiring before flashing —
> some ADC channels reserve specific pins as analog-only.

---

## Block Diagram

<img width="601" height="440" alt="block_diagram" src="https://github.com/user-attachments/assets/b002529c-a784-4946-bc4a-c89820e08cde" />


---

## Default Settings

| Setting          | Default | Editable from menu? |
|------------------|---------|----------------------|
| Temperature limit| 40 °C   | Yes                  |
| Gas limit        | 300 (raw ADC, 0–1023) | Yes    |
| Password         | 1234    | Yes                  |
| Menu timeout     | 30 sec  | No (compile-time)    |
| Lockout duration | 10 sec  | No (compile-time)    |
| Event popup       | every 10 sec, shown for 3 sec | No (compile-time) |

All of the above are adjustable `#define`s at the top of `main.c`.

---

## Software

- **Language:** Embedded C
- **Toolchain:** Keil µVision (or equivalent ARM7 toolchain)
- **Programming:** Flash Magic via USB-UART
- **Simulation:** Proteus (LPC2148 model)

---

## File Structure

```
main.c          - Application logic (sensors, menu, alarm, display)
LCD.c / .h      - 16x2 character LCD driver
ADC.c / .h      - ADC driver shared by LM35 and MQ2
LM35.c / .h     - Temperature conversion helper
KPM.c / .h      - 4x4 matrix keypad driver
RTC.c / .h      - Real-time clock driver
delay.c / .h    - Software delay routines
types.h         - Shared type definitions (u8, u32, s32, etc.)
```

---

## Menu Map

Press **Switch1** → enter password →

```
1.RTC 2.SETPT
3.PASS 4.EXIT
```

- **1 (RTC):** `1.H 2.M 3.S 4.D 5.M 6.Y 7.E` — edit one field at a time, loops
  back until Exit (7)
- **2 (SETPT):** choose Temp or Gas, enter a new limit
- **3 (PASS):** enter current password, then new password twice
- **4 (EXIT)** or 30-second timeout: return to normal monitoring

Three wrong password attempts in a row → 10-second lockout countdown → back to
the password prompt.

---

## Notes

- The buzzer and LED always activate together — there is no independent control
  between the two alarm outputs.
- The last-event popup forces the alarm off for its full duration, regardless of
  the live sensor readings, so it can be read without noise/light interference.
- This project was developed iteratively; some in-code comments describe design
  trade-offs (e.g. why a fixed hysteresis band was removed) that are useful
  context if you plan to modify the alert logic further.

---

## License

Add your license of choice here (e.g. MIT) before publishing.
