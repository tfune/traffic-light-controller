# Arduino Traffic Light Controller

An embedded traffic light control system built on an Arduino Mega 2560 using a finite state machine architecture, interrupt-driven emergency handling, and non-blocking real-time timing logic.

This project was developed to demonstrate core embedded systems concepts including:

* finite state machines (FSM)
* real-time embedded control
* interrupt service routines (ISR)
* non-blocking timing
* input debouncing
* hardware/software integration

---

# Demo & Media

**Demo Video:**
[Watch on YouTube](https://youtu.be/WH35ogzfD9o)

**Circuit Layout:**

![Circuit Layout](media/circuit_layout.jpg)

**Circuit Layout (Close-Up)**

![Circuit Layout Close-Up](media/circuit_layout_close_up.jpg)

# Features

* Finite state machine traffic control architecture
* Non-blocking timing using `millis()`
* Four-way intersection light sequencing
* Interrupt-driven emergency override mode
* Software-debounced pedestrian crossing input
* Dedicated pedestrian crossing indicator
* Real-time asynchronous input handling
* Modular and expandable embedded design

---

# Hardware Used

| Component | Description |
| --- | --- |
| Microcontroller | Arduino Mega 2560 |
| LEDs | 12 traffic signal LEDs |
| Pedestrian Indicator | White LED |
| Buttons | Pedestrian and emergency inputs |
| Breadboards | Dual breadboard layout |
| Resistors | Current limiting for LEDs |
| Wiring | Jumper wire interconnects |

---

# System Architecture

```text
TrafficLightController
├── Traffic State Logic
├── Pedestrian Input Handling
├── Emergency Interrupt Handling
└── LED Output Control
```

## Module Responsibilities

### Traffic State Logic

* controls intersection sequencing
* manages FSM state transitions
* handles non-blocking timing logic

### Pedestrian Input Handling

* monitors crossing requests
* applies software debouncing
* triggers pedestrian crossing phase

### Emergency Interrupt Handling

* interrupt-driven emergency detection
* forces flashing all-red operation
* overrides normal traffic sequencing

### LED Output Control

* controls all signal outputs
* synchronizes intersection states
* manages pedestrian indicator LED

---

# State Machine

```text
NORTH_SOUTH_GREEN
↓
NORTH_SOUTH_YELLOW
↓
ALL_RED
↓
EAST_WEST_GREEN
↓
EAST_WEST_YELLOW
↓
ALL_RED

PEDESTRIAN and EMERGENCY modes accessible asynchronously
```

## State Summary

* **NORTH_SOUTH_GREEN** → north/south traffic flows
* **NORTH_SOUTH_YELLOW** → transition warning phase
* **ALL_RED** → intersection safety buffer
* **EAST_WEST_GREEN** → east/west traffic flows
* **EAST_WEST_YELLOW** → transition warning phase
* **PEDESTRIAN MODE** → activates pedestrian crossing indicator
* **EMERGENCY MODE** → flashing all-red override state

---

# Embedded Concepts Demonstrated

* Finite state machines
* Interrupt service routines
* Non-blocking embedded timing
* Software debouncing
* Real-time event handling
* Embedded GPIO control
* Modular firmware architecture
* Hardware/software integration

---

# Real-Time Timing

The controller uses the Arduino `millis()` timer for non-blocking operation rather than `delay()` calls.

This allows the system to:

* remain responsive to emergency interrupts
* process pedestrian requests asynchronously
* maintain deterministic state transitions

Example timing logic:

```cpp
if(currentMillis - previousMillis >= interval)
```

---

# Interrupt-Driven Emergency Mode

Emergency mode is triggered using a hardware interrupt for immediate response.

When activated:

* normal traffic sequencing is suspended
* all directions enter flashing red operation
* the system remains in emergency mode until cleared

---

# Pin Mapping

| Function | Pins |
| --- | --- |
| North–South #1 | 22, 24, 26 |
| North–South #2 | 28, 30, 32 |
| East–West #1 | 34, 36, 38 |
| East–West #2 | 40, 42, 44 |
| Pedestrian LED | 46 |
| Pedestrian Button | 49 |
| Emergency Interrupt Button | 2 |

---

# Future Improvements

* Add pedestrian countdown display
* Implement adaptive traffic timing using sensors
* Add UART/Bluetooth remote monitoring
* Port firmware to STM32 hardware
* Design a custom PCB for signal control circuitry

---

# Author

Trevor Fune