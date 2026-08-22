# Atmospheric-Reentry-Simulator

[![Language](https://img.shields.io/badge/C++17-%2300599C?style=flat-square&logo=c%2B%2B&logoColor=white)](https://github.com/skytruong90/Atmospheric-Reentry-Simulator)
[![Domain](https://img.shields.io/badge/Domain-Hypersonic_M%26S-red?style=flat-square)](https://github.com/skytruong90/Atmospheric-Reentry-Simulator)
[![Status](https://img.shields.io/badge/Status-Active-00FF88?style=flat-square)](https://github.com/skytruong90/Atmospheric-Reentry-Simulator)

## Project Overview
## Atmospheric Reentry Simulator

A documented C++17 point-mass atmospheric-entry simulator. It combines a layered
1976 Standard Atmosphere approximation, Mach-dependent drag, convective
stagnation-point heating, and fourth-order Runge–Kutta integration. Results are
written as analysis-ready CSV.

> **Scope:** This is an educational preliminary-analysis tool. It omits lift,
> winds, Earth rotation, ablation, radiative heating, and coupled thermal/structural
> response. Do not use its output for flight certification or safety decisions.

## Build and test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

The project has no third-party runtime dependencies. Set
`-DREENTRY_BUILD_TESTS=OFF` when only the library and CLI are needed.

## Run

```bash
./build/reentry-sim --output trajectory.csv
./build/reentry-sim --altitude 100000 --velocity 7000 --angle -8 \
  --mass 1800 --area 2.5 --nose-radius 0.6 --dt 0.02 --duration 1200 \
  --output trajectory.csv
```

Use `--output -` to stream CSV to standard output and `--help` for all options.
Each row contains time, altitude, speed, flight-path angle, downrange distance,
Mach number, density, drag coefficient, drag acceleration, and heat flux.

## Model overview

- **Atmosphere:** geopotential layers through 84.852 km use the US Standard
  Atmosphere 1976 lapse rates. A continuous exponential extension allows entry
  initialization above the tabulated region.
- **Aerodynamics:** a documented default blunt-body drag curve is linearly
  interpolated by Mach. Library users can supply a custom table to `DragModel`.
- **Heating:** the Sutton–Graves Earth-air correlation estimates convective
  stagnation-point heat flux. Nose radius is configurable.
- **Dynamics:** non-lifting point-mass equations over a spherical, non-rotating
  Earth are integrated with fixed-step RK4. Ground contact is interpolated.

Public interfaces live in `include/reentry/`; implementations are separated in
`src/`, the CLI is in `app/`, and dependency-free regression tests are in
`tests/`. Comments focus on assumptions and non-obvious numerical choices so
future contributors can extend the model without having to reverse-engineer it.

## Library example

```cpp
#include "reentry/simulation.hpp"

reentry::Vehicle vehicle{2000.0, 3.0, 0.5};
reentry::InitialConditions initial{120000.0, 7500.0, -6.0};
reentry::SimulationOptions options{0.05, 2000.0, 1.0};
auto samples = reentry::Simulator{}.run(vehicle, initial, options);
```

---

<div align="center">

[![View Repository](https://img.shields.io/badge/View_Repository-%E2%86%97-00C8FF?style=for-the-badge&labelColor=050A0F)](https://github.com/skytruong90/Atmospheric-Reentry-Simulator)

</div>
