# Young's Double-Slit Interference Simulation (C++)

## Overview

This project simulates **Young's double-slit light interference** using a physically accurate wave model written in C++. It computes the complex amplitude (phasor) of each wave source at every point on a 1-D observation screen, superposes them via the **principle of superposition**, and exports the resulting intensity pattern to CSV.

The implementation includes:

* Abstract polymorphic `Wave` hierarchy (ideal point source and finite-width slit)
* Single-slit **diffraction envelope** modulation via sinc²(β)
* **Superposition** of N arbitrary wave sources via `CompositeWave`
* Transparent **unit normalisation** — configure in physical SI units (metres, nanometres), simulate internally in dimensionless scene units
* 1-D screen detector with ASCII bar-chart output and CSV export
* Analytical fringe position table with missing-order detection
* Michelson visibility metric for the polymorphic wave survey

The `Wave` interface is intentionally generic: the same class hierarchy maps directly to **Feynman path-integral / option-pricing** models, where `phasor()` represents a path propagator and `CompositeWave` performs the sum over paths.

---

## Features

### Wave Model

#### Abstract base class: `Wave`

Every wave source derives from a single interface:

```cpp
class Wave {
public:
    virtual std::complex<double> phasor(double x, double y) const = 0;
    virtual double               intensity(double x, double y) const;
    virtual const char*          name() const = 0;
};
```

`intensity()` is implemented once in the base as `|phasor|²` and is inherited by all derived types.

---

#### `PointSource` — ideal Huygens wavelet

Models an infinitesimally thin slit as a circular outgoing wave:

```text
phasor(x,y) = A · exp(i·k·r) / sqrt(r)
```

where `r` is the distance from the source to the evaluation point. The `1/sqrt(r)` factor gives the correct 2-D amplitude fall-off.

---

#### `SlitSource` — finite-width slit with diffraction envelope

Extends `PointSource` by multiplying by the **single-slit diffraction envelope**:

```text
phasor(x,y) = A · sinc(β) · exp(i·k·r) / sqrt(r)

β = π · (a/d) · m(θ)
```

where:

```text
a     = slit width
d     = slit separation
m(θ)  = continuous interference order at angle θ  (= d·sinθ/λ)
```

✔️ Envelope is computed in terms of the dimensionless ratio `a/d`, making it independent of absolute physical units  
✔️ Missing orders occur at `m = n·(d/a)` for integer `n` — e.g. `a/d = 0.2` suppresses orders `±5, ±10, ...`  
✔️ Setting `slitWidthRatio = 0` recovers the ideal `PointSource` behaviour

---

#### `CompositeWave` — superposition principle

Owns an arbitrary collection of `Wave` objects and sums their complex amplitudes:

```text
phasor_total(x,y) = Σ phasor_i(x,y)
```

Interference emerges naturally: constructive where phases align, destructive where they cancel. Adding more sources (double → quad → grating) requires no changes to the interface.

---

### Experiment Configuration

All parameters are specified in **physical SI units**. Internal normalisation by `d` is handled transparently by `InterferenceExperiment`.

```cpp
struct ExperimentConfig {
    double slitSeparation   = 0.10e-3;  // d  [m]
    double slitWidthRatio   = 0.2;      // a/d  (0 = ideal point source)
    double wavelength       = 550e-9;   // λ  [m]
    double screenDistance   = 1.0;      // L  [m]
    double screenHalfHeight = 0.02;     // ±H  [m]
    int    numSamples       = 200;
};
```

| Parameter | Description |
|---|---|
| `slitSeparation` | Centre-to-centre slit distance `d` |
| `slitWidthRatio` | Slit width as fraction of separation `a/d` |
| `wavelength` | Wavelength of light `λ` |
| `screenDistance` | Slit-to-screen distance `L` |
| `screenHalfHeight` | Half-height of observation screen `±H` |
| `numSamples` | Number of sample points on screen |

---

### Unit Normalisation

Physical inputs are normalised internally so the wave solver always operates with `d = 1`:

```text
k_scene = 2π · d / λ
L_scene = L / d
H_scene = H / d
```

Fringe spacing in scene units is `λ_scene · L_scene = (λ/d)·(L/d)`, which converts back to the correct physical formula `λL/d` when multiplied by `d`. CSV output and the ASCII chart are always written in physical metres / millimetres.

---

### Screen Detector

`Screen` samples the intensity of any `Wave` at evenly spaced y-positions and provides:

* **ASCII bar-chart** of the intensity pattern with physical mm labels
* **CSV export** with columns `y [m], intensity`
* **Analytical fringe table** — positions `y_m = m·λ·L/d` with missing-order annotation

---

### Polymorphic Wave Survey

`main.cpp` demonstrates virtual dispatch by storing `PointSource`, double-slit `CompositeWave`, and quad-slit `CompositeWave` in the same `vector<unique_ptr<Wave>>` and querying each through the base pointer. Output includes:

* Peak and trough intensity in scientific notation
* **Michelson visibility** `V = (I_max − I_min) / (I_max + I_min)` — scale-independent fringe quality metric

```
[PointSource]
  peak intensity : 1.0000e-04
  trough         : 9.9980e-05
  visibility V   : 0.0001  (1.0 = perfect fringes)

[CompositeWave]   <- double slit
  peak intensity : 3.9978e-04
  trough         : 2.9090e-09
  visibility V   : 1.0000  (1.0 = perfect fringes)

[CompositeWave]   <- quad slit
  peak intensity : 1.5995e-03
  trough         : 3.0471e-09
  visibility V   : 1.0000  (1.0 = perfect fringes)
```

The quad-slit peak is `4² = 16×` the single-source peak, consistent with coherent superposition.

---

## Class Structure

```
Wave                   (abstract)
 ├── PointSource        circular wavelet  A·exp(ikr)/√r
 └── SlitSource         PointSource × sinc diffraction envelope

CompositeWave          (owns N Wave objects, sums phasors)

Screen                 (1-D detector: samples, prints, exports)
InterferenceExperiment (facade: maps physical config → scene units → Wave objects → Screen)
```

---

## How It Works

### 1. Configuration

Set physical parameters in `ExperimentConfig` — wavelength in nm, slit dimensions in mm, screen distance in m.

### 2. Normalisation

`InterferenceExperiment` computes `sceneScale = 1/d` and constructs all wave sources and the screen in dimensionless scene units.

### 3. Phasor evaluation

For each screen sample point `(L_scene, y_scene)`, each source computes its complex phasor. `CompositeWave` sums them.

### 4. Intensity

`Wave::intensity()` returns `|phasor|²` — the physically observed quantity.

### 5. Output

The screen converts y-coordinates back to millimetres, prints the ASCII pattern, and exports the CSV in metres.

---

## How to Compile

**With CMake (recommended):**

```bash
mkdir build && cd build
cmake ..
cmake --build .
./interference_sim
```

**Direct compilation:**

```bash
g++ -std=c++17 -O2 \
    lightinterference.cpp Wave.cpp PointSource.cpp SlitSource.cpp \
    CompositeWave.cpp Screen.cpp InterferenceExperiment.cpp \
    -o interference_sim
./interference_sim
```

Requires C++17 or later. No external dependencies.

---

## Output Files

Running the simulation produces four CSV files:

| File | Description |
|---|---|
| `double_slit_550nm_envelope.csv` | Green light (550 nm), finite slit `a/d = 0.2` |
| `double_slit_550nm_ideal.csv` | Green light, ideal point sources |
| `double_slit_700nm_envelope.csv` | Red light (700 nm), finite slit `a/d = 0.2` |
| `double_slit_550nm_wide.csv` | Green light, wide slit `a/d = 0.5` (every even order missing) |

Each CSV has two columns: `y` (screen position in metres) and `intensity`.

---

## Notes

* Model assumes **monochromatic, coherent** illumination
* The `1/sqrt(r)` amplitude fall-off is correct for 2-D cylindrical waves; 3-D propagation would use `1/r`
* Fringe visibility is sensitive to slit width ratio — `a/d = 0.5` suppresses every second fringe entirely
* The polymorphic survey reports raw intensity in scientific notation; use Michelson visibility for scale-independent comparison across different geometries
