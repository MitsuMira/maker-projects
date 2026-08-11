# Semáforo de cruce — Traffic Light Crossroad Controller

A fully working 4-way traffic light system built with an Arduino Uno, individual LEDs,
and push buttons on a breadboard. Pedestrians can request a crossing at any time using
hardware interrupts — both pedestrian lights go green simultaneously while all cars stop.

**Platform:** Arduino Uno (Elegoo Uno R3 compatible)
**Difficulty:** Intermediate
**Build time:** 2-3 hours
**Material cost:** ~€12-25
**License:** [CC BY-NC-SA 4.0](../../LICENSE)

---

## How it works

**Normal cycle:**
Car A green (8s) → yellow (2s) → all red (1s) → Car B green (8s) → yellow (2s) → all red (1s) → repeat

Pedestrian lights stay **red** during the entire car cycle.

**When a button is pressed (either one):**
The request is queued. At the next all-red transition, both pedestrian lights go green (4s),
then blink as a warning (2s), then all red again before the car cycle resumes.
A second press during an active crossing is ignored until the next cycle.

---

## Components

| Component | Quantity |
|---|---|
| Elegoo Uno R3 (or Arduino Uno) | 1 |
| Red LEDs 5mm | 4 |
| Yellow LEDs 5mm | 2 |
| Green LEDs 5mm | 4 |
| 220Ω resistors | 10 |
| Tactile push buttons | 2 |
| Breadboard (830 point) | 1 |
| Jumper wires M-M assorted | ~20 |
| USB cable (USB-A to USB-B) | 1 |

---

## Pin reference

| Pin | Constant | Component |
|---|---|---|
| 2 | `BTN_A` | Button A — INT0 (hardware interrupt) |
| 3 | `BTN_B` | Button B — INT1 (hardware interrupt) |
| 4 | `COCHE_A_ROJO` | Car A — Red LED |
| 5 | `COCHE_A_AMARILLO` | Car A — Yellow LED |
| 6 | `COCHE_A_VERDE` | Car A — Green LED |
| 7 | `PEAT_A_ROJO` | Pedestrian A — Red LED |
| 8 | `PEAT_A_VERDE` | Pedestrian A — Green LED |
| 9 | `COCHE_B_ROJO` | Car B — Red LED |
| 10 | `COCHE_B_AMARILLO` | Car B — Yellow LED |
| 11 | `COCHE_B_VERDE` | Car B — Green LED |
| 12 | `PEAT_B_ROJO` | Pedestrian B — Red LED |
| 13 | `PEAT_B_VERDE` | Pedestrian B — Green LED |
| GND | — | Common GND bus |

> **Note:** Pin 13 has the Uno's built-in LED — it will blink with Pedestrian B green.
> This is cosmetic and does not affect functionality.

> **Buttons:** use `INPUT_PULLUP` — no external resistor needed.
> Connect one terminal to pin 2 or 3, the other to GND.

---

## 3D printed housing

A printable housing, pole, and base for the traffic light is available on Printables:
[Crosswalk Traffic Light — Arduino Housing, Pole & Base](https://www.printables.com/model/1807397-crosswalk-traffic-light-arduino-housing-pole-base)

---

## Code

Sketch: [`semaforo-cruce.ino`](semaforo-cruce.ino)

The code uses:
- `enum` state machine — no `delay()` anywhere
- `millis()` for all timing — keeps interrupts responsive
- Hardware interrupts on INT0 and INT1
- `volatile` flag written by ISR, read in main loop

---

## Full guide

Wiring diagram, state tables, and build tips:
[mitsumira.com/proyectos/semaforo-cruce](https://mitsumira.com/proyectos/semaforo-cruce)
*(coming soon)*

---

## License

This project is licensed under [CC BY-NC-SA 4.0](../../LICENSE).
Free for personal and educational use with attribution to **MitsuMira**.
Commercial use is not permitted.

A [MitsuMira](https://mitsumira.com) project · 3D printing & maker education · Barcelona
