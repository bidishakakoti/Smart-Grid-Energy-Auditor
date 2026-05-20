# Power Factor — Research Notes

## What is Power Factor?
Power Factor (PF) is the ratio of **real power** (watts) to **apparent power** (volt-amperes).
PF = Real Power (W) / Apparent Power (VA)

## Why Does It Matter?
- A PF of 1.0 means 100% of drawn current does useful work (resistive loads)
- A PF of 0.75 means 25% of current is wasted as reactive power (inductive loads)
- Low PF = higher current draw = higher bills + risk of overloading wiring

## Resistive vs Inductive
| Type       | Example          | Typical PF |
|------------|------------------|------------|
| Resistive  | Heater, Bulb     | 1.00       |
| Inductive  | AC, Motor, Fan   | 0.70–0.90  |

## Capacitor Bank Correction
Adding capacitors in parallel with inductive loads offsets reactive power,
bringing PF closer to 1.0 — reducing wasted energy by up to 20%.

## Our Threshold
This system flags any appliance with PF < 0.85 as inefficient,
which aligns with Bureau of Energy Efficiency (BEE) India guidelines.