# Atmospheric-Reentry-Simulator

[![Language](https://img.shields.io/badge/C++17-%2300599C?style=flat-square&logo=c%2B%2B&logoColor=white)](https://github.com/skytruong90/Atmospheric-Reentry-Simulator)
[![Domain](https://img.shields.io/badge/Domain-Hypersonic_M%26S-red?style=flat-square)](https://github.com/skytruong90/Atmospheric-Reentry-Simulator)
[![Status](https://img.shields.io/badge/Status-Active-00FF88?style=flat-square)](https://github.com/skytruong90/Atmospheric-Reentry-Simulator)

## Project Overview

High-fidelity C++ atmospheric reentry simulation for hypersonic vehicle modeling. Implements variable-density atmospheric models, aerodynamic heating, and drag-modulated reentry dynamics — applicable to ICBM reentry vehicle simulation and hypersonic glide vehicle (HGV) analysis.

---

## Features

- **US Standard Atmosphere 1976** — Altitude-varying density, pressure, temperature, and speed of sound.
- **Aerodynamic Heating** — Stagnation point heat flux computation using Fay-Riddell model.
- **Hypersonic Drag** — Mach-dependent drag coefficients from 0 to Mach 25.
- **RK4 Integration** — 4th-order Runge-Kutta numerical integration of reentry equations of motion.
- **Ballistic Coefficient** — Configurable BC for reentry vehicle sizing analysis.

---

## Technical Background

| Concept | Implementation |
|--------|----------------|
| Atmosphere Model | US Standard Atmosphere 1976 |
| Heat Transfer | Fay-Riddell stagnation heating |
| Drag Model | Mach-dependent CD table lookup |
| Integration | RK4 variable time-step |
| Output | Altitude, velocity, heat rate vs. time |

---

<div align="center">

[![View Repository](https://img.shields.io/badge/View_Repository-%E2%86%97-00C8FF?style=for-the-badge&labelColor=050A0F)](https://github.com/skytruong90/Atmospheric-Reentry-Simulator)

</div>

