# Smart-Grid-Energy-Auditor
A smart grid energy auditor system developed using OOP concepts to analyze, monitor, and optimize energy consumption efficiently.
# ⚡ Smart Grid Energy Auditor

> A C++ simulation of an intelligent home energy monitoring system that audits appliance efficiency, manages load safety, and calculates tiered billing — built as a showcase of modern Object-Oriented Programming.

---

## 📌 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [OOP Concepts Demonstrated](#oop-concepts-demonstrated)
- [Project Structure](#project-structure)
- [System Workflow](#system-workflow)
- [Getting Started](#getting-started)
- [Sample Output](#sample-output)
- [Tech Stack](#tech-stack)
- [Author](#author)

---

## Overview

Modern households waste energy due to inefficient appliances (low power factor) and lack of real-time load management — leading to tripped breakers and inflated bills.

**Smart Grid Energy Auditor** acts as an intelligent software layer over a simulated home electrical system. It monitors live load, audits appliance efficiency, and computes dynamic billing — all from a clean console dashboard.

This is a **university OOP project** written in **C++17**, designed to demonstrate core and advanced Object-Oriented Programming principles in a realistic, practical context.

---

## Features

| Feature | Description |
|---|---|
| ⚡ **Live Grid Dashboard** | Real-time console view of all appliance loads vs. safety capacity |
| 🔴 **Safety Load Management** | Warns and flags when total wattage exceeds the grid limit (default: 5000W) |
| 🔍 **Efficiency Auditing** | Detects inductive loads (ACs, motors) with poor Power Factor (PF < 0.85) and recommends capacitor correction |
| 💰 **Tiered Dynamic Billing** | Calculates costs using standard vs. peak-hour rates via overloaded functions |
| 💾 **Persistent Logging** | Saves audit reports and session summaries to files via RAII destructors |
| 🔄 **Appliance State Tracking** | Each appliance can be in `ON`, `OFF`, or `STANDBY` state |

---

## OOP Concepts Demonstrated

This project is structured to showcase each major C++ OOP concept explicitly:

### 1. 🔒 Encapsulation
Raw `voltage`, `current`, and `powerFactor` are stored as `private` members in the `Appliance` base class. No derived class or external code can accidentally corrupt these values — they are only accessible through controlled interfaces.

### 2. 🧬 Inheritance
```
Appliance  (Abstract Base Class)
├── ResistiveLoad    → Heaters, Bulbs    (Load = V × I)
└── InductiveLoad    → ACs, Motors       (Load = V × I × PF)
```
Both derived classes inherit common properties (name, wattage, state) while implementing their own energy logic.

### 3. 🎭 Polymorphism
The `calculateLoad()` function is declared **pure virtual** in `Appliance`, forcing each derived class to provide its own implementation. A `GridController` holding a `vector<unique_ptr<Appliance>>` can call `calculateLoad()` on any appliance without knowing its type.

```cpp
// Same call, different behaviour depending on object type
for (auto& appliance : appliances) {
    totalLoad += appliance->calculateLoad(); // Polymorphic dispatch
}
```

### 4. ➕ Operator Overloading
The `+` operator is overloaded on `Appliance` to return the combined active wattage of two appliances — used by `GridController` to perform safety checks cleanly.

```cpp
float combined = ac + heater; // Returns sum of their active loads
```

### 5. 🤝 Friend Class
The `Auditor` class is declared a **friend** of `Appliance`. This grants it read-only access to private `powerFactor` data — without exposing a public getter that any code could call. This models the real-world concept of a privileged auditor having inspection rights.

```cpp
class Appliance {
    friend class Auditor; // Only Auditor can inspect private PF
private:
    float powerFactor;
};
```

### 6. 🏗️ Constructors & Destructors (RAII)
- **Constructors** initialize appliances with real-time energy prices loaded at startup.
- **Destructor** of `GridController` automatically saves the session summary to `data/session.txt` when the program exits — no explicit save call needed.

---

## Project Structure

```
SmartGridAuditor/
│
├── src/                        # All .cpp implementation files
│   ├── main.cpp                # Entry point — menu loop and session init
│   ├── Appliance.cpp           # Base class implementation
│   ├── ResistiveLoad.cpp       # Derived: Heaters, Bulbs
│   ├── InductiveLoad.cpp       # Derived: ACs, Motors
│   ├── GridController.cpp      # Load management + dashboard
│   ├── Auditor.cpp             # Friend class — efficiency audit + file logging
│   └── BillingEngine.cpp       # Tiered billing calculations
│
├── include/                    # All .h header files
│   ├── Appliance.h             # Abstract base class + friend declaration
│   ├── ApplianceState.h        # Enum: ON / OFF / STANDBY
│   ├── ResistiveLoad.h
│   ├── InductiveLoad.h
│   ├── GridController.h
│   ├── Auditor.h
│   └── BillingEngine.h
│
├── data/                       # Auto-generated persistent storage
│   ├── audit_logs.txt          # Written by Auditor class
│   └── session.txt             # Written by GridController destructor
│
├── tests/                      # Unit-style logic tests
│   ├── test_load_calculation.cpp
│   ├── test_operator_overload.cpp
│   └── test_billing.cpp
│
├── docs/                       # Presentation and research
│   ├── presentation.pptx
│   ├── class_diagram.png       # UML diagram of the class hierarchy
│   └── research_notes/
│       └── power_factor.md
│
├── CMakeLists.txt              # CMake build configuration
├── Makefile                    # Alternative: simple make build
├── .gitignore                  # Excludes binaries, .exe, object files
├── CONTRIBUTING.md             # Notes for collaborators
└── README.md                   # This file
```

---

## System Workflow

```
Phase 1 — SETUP
  └── GridController loads appliances (Samsung AC, Water Heater, LED Bulbs...)
      Each constructed with name, wattage, voltage, current, power factor

Phase 2 — MONITORING  (Operator Overloading + Polymorphism)
  └── GridController sums load using overloaded + operator
      If Total Load > 5000W → Safety Warning triggered

Phase 3 — AUDITING    (Friend Class)
  └── Auditor reads private powerFactor of each InductiveLoad
      If PF < 0.85 → Logs recommendation: "Apply Capacitor Correction"
      Report saved to data/audit_logs.txt

Phase 4 — BILLING     (Function Overloading)
  └── BillingEngine::calculate(units)           → Standard rate
      BillingEngine::calculate(units, isPeak)   → Peak-hour surcharge applied

Phase 5 — EXIT        (RAII Destructor)
  └── GridController destructor fires automatically
      Daily summary written to data/session.txt
```

---

## Getting Started

### Prerequisites
- C++17 compatible compiler (`g++ 9+` or `clang++ 9+`)
- CMake 3.15+ *(optional, for CMake build)*

### Build with CMake
```bash
git clone https://github.com/niladrisaikia27/OOPs_project.git
cd SmartGridAuditor
mkdir build && cd build
cmake ..
make
./SmartGridAuditor
```

### Build with Makefile
```bash
git clone https://github.com/niladrisaikia27/OOPs_project.git
cd SmartGridAuditor
make
./SmartGridAuditor
```

### Build Manually
```bash
g++ -std=c++17 -Iinclude src/*.cpp -o SmartGridAuditor
./SmartGridAuditor
```

---

## Sample Output

```
╔══════════════════════════════════════════════════╗
║        ⚡  SMART GRID ENERGY AUDITOR  ⚡          ║
╚══════════════════════════════════════════════════╝

 LIVE DASHBOARD — Total Load: 3850W / 5000W  [▓▓▓▓▓▓▓▓░░] 77%

 Appliance            State      Load
 ─────────────────────────────────────────
 Samsung 1.5T AC      ON         1800W
 Water Heater         ON         1500W
 LED Ceiling Lights   ON          120W
 Refrigerator         STANDBY     430W

⚠️  WARNING: Load approaching limit. Consider turning off the Water Heater.

[1] Run Efficiency Audit
[2] Calculate Bill
[3] Toggle Appliance State
[4] Exit
```

---

## Tech Stack

- **Language:** C++17
- **Paradigm:** Object-Oriented Programming
- **Interface:** Console / CLI Dashboard
- **Build System:** CMake + Makefile
- **Data Persistence:** File I/O (`fstream`)
- **Memory Management:** RAII, `std::unique_ptr`

---

## Team Members

**Niladri Saikia - 2412049 |**
**Rini Nath - 2412050 |**
**Bidisha Kakati - 2412069 |**
B.Tech, Computer Science & Engineering — NIT Silchar

> *Built as an Object-Oriented Programming course project.*
> *All energy values and appliance specs are simulated for demonstration purposes.*
