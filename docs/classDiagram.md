# Diagramme de classes

Généré depuis l'état actuel du code (`src/`). Contrairement à `docs/classDiagram.svg` (export figé, périmé), ce fichier est la source Mermaid — GitHub le rend nativement, et il se diff/modifie comme du texte normal.

Structure : une vue d'ensemble (juste les classes et leurs relations, pour voir l'architecture d'un coup d'œil), puis chaque classe détaillée séparément (attributs/méthodes complets, sans être écrasé par les autres).

---

## Vue d'ensemble

```mermaid
classDiagram
    Simulator "1" --> "1" AirplaneState : pilote
    Window "1" --> "1" AirplaneState : affiche
    Display "1" --> "1" AirplaneState : affiche
    Window ..> DataColor : construit
    AirplaneState "1" *-- "0..*" Engine : possède
    AirplaneState "1" *-- "1" Quaternion : orientation
    AirplaneState ..> EngineConfig : construit depuis
    Quaternion ..> Vec3 : construit depuis (axe), rotate()
```

- `Simulator`, `Window` et `Display` détiennent chacun un **pointeur** (non-propriétaire) vers un unique `AirplaneState` partagé — l'état central protégé par mutex, lu/écrit depuis plusieurs threads.
- `AirplaneState` **possède** ses `Engine` (composition, via `std::unique_ptr` — durée de vie liée) et son `Quaternion` d'orientation (`_orientation`, remplace l'ancien `_heading` scalaire).
- `EngineConfig` et `DataColor` sont de simples structs (pas de logique), utilisés respectivement pour construire les moteurs et pour transporter texte+couleur à afficher dans `Window`.
- `Vec3`/`Quaternion` sont branchés dans `AirplaneState` pour l'orientation (`getForward()`, `getRight()`, `getOrientation()`) — voir `docs/math.md`, section « Flux d'utilisation ». Les forces (poussée, traînée, portance, poids) sont désormais calculées en `Vec3` dans `Simulator` et intègrent directement `AirplaneState::_velocity` — voir `docs/simulator.md`, section « Forces vectorielles ».

---

## `AirplaneState`

```mermaid
classDiagram
    class AirplaneState {
        -double _altitude
        -double _xPos
        -double _yPos
        -double _airSpeed
        -Vec3 _velocity
        -double _AOA
        -Quaternion _orientation
        -int _engineCount
        -int _masse
        -double _dragCoef
        -double _liftCoef
        -double _surface
        -vector~unique_ptr~Engine~~ _engines
        -mutex mutexAirplaneState
        +AirplaneState(altitude, xPos, yPos, airSpeed, velocity, AOA, orientation, engineCount, EngineConfig, masse, dragCoef, liftCoef, surface)
        +AirplaneState(AirplaneState* airplane)
        +getAltitude() double
        +getXPos() double
        +getYPos() double
        +getAirSpeed() double
        +getGroundSpeed() double
        +getVerticalSpeed() double
        +getVelocity() Vec3
        +getAOA() double
        +getForward() Vec3
        +getRight() Vec3
        +getOrientation() Quaternion
        +getHeading() double
        +getEngineRPM(int index) double
        +getEnginesCount() int
        +getMasse() int
        +getDragCoef() double
        +getSurface() double
        +getLiftCoef() double
        +getTotalThrust() int
        +setAltitude(double) void
        +setXPos(double) void
        +setYPos(double) void
        +setVelocity(Vec3) void
        +computeIAS(double speed) void
        +computeAOA(Vec3 forwardVec, Vec3 velocityVec, Vec3 right) void
    }
```

## `EngineConfig`

```mermaid
classDiagram
    class EngineConfig {
        +double spoolRate
        +int maxThrust
    }
```

## `Engine`

```mermaid
classDiagram
    class Engine {
        -double _spoolRate
        -double _engineRPM
        -int _commandRPM
        -int _maxThrust
        -bool _state
        -mutex _mutexEngine
        +Engine(spoolRate, engineRPM, commandThrust, maxThrust, state)
        +getSpoolRate() double
        +getEngineRPM() double
        +getCommandRPM() int
        +getMaxThrust() int
        +getState() bool
        +setSpoolRate(double) void
        +setEngineRPM(double) void
        +setCommandRPM(int) void
        +setState(bool) void
        +computeThrust() int
        +updateRPM() void
    }
```

## `Simulator`

```mermaid
classDiagram
    class Simulator {
        -AirplaneState* _airplane
        -atomic~bool~ _simRunning
        -thread _simThread
        -double _timeScale$
        -int _tickTime$
        -double _g$
        -simLoop() void
        -computeAirDensity(double altitude) double
        -computeThrust(const Vec3& forward) Vec3
        -computeDrag(const Vec3& v, double rho) Vec3
        -computeLift(const Vec3& v, const Vec3& right, double rho) Vec3
        -computeWeight(double m) Vec3
        +Simulator(AirplaneState* newAirplane)
        +getSimRunning() bool
        +getTickTime() int
        +startSim() void
        +stopSim() void
    }
```

## `Window`

```mermaid
classDiagram
    class Window {
        -TTF_Font* _font
        -SDL_Window* _window
        -SDL_Renderer* _renderer
        -AirplaneState* _airplane
        -int _tickTime$
        -displayInfos(info, lineIndex, color) void
        +int _height
        +int _width
        +Window(AirplaneState*, width, height, title)
        +run() void
    }
```

## `DataColor`

```mermaid
classDiagram
    class DataColor {
        +string data
        +SDL_Color color
    }
```

## `Display`

```mermaid
classDiagram
    class Display {
        -AirplaneState* _airplane
        -atomic~bool~ _displayRunning
        -thread _displayThread
        -int _tickTime$
        -displayLoop() void
        +Display(AirplaneState* newAirplane)
        +getTickTime() int
        +startDisplay() void
        +stopDisplay() void
    }
```

## `Vec3`

```mermaid
classDiagram
    class Vec3 {
        -double _x
        -double _y
        -double _z
        +Vec3(x, y, z)
        +Vec3(const Vec3&)
        +Vec3()
        +getX() double
        +getY() double
        +getZ() double
        +setX(double) void
        +setY(double) void
        +setZ(double) void
        +normalize() Vec3
        +dot(const Vec3&) double
        +cross(const Vec3&) Vec3
    }
```

## `Quaternion`

```mermaid
classDiagram
    class Quaternion {
        -double _x
        -double _y
        -double _z
        -double _w
        +Quaternion(x, y, z, w)
        +Quaternion(angle, Vec3* axis)
        +Quaternion(const Quaternion&)
        +Quaternion()
        +operator=(const Quaternion&) Quaternion&
        +getX() double
        +getY() double
        +getZ() double
        +getW() double
        +setX(double) void
        +setY(double) void
        +setZ(double) void
        +setW(double) void
        +multiply(const Quaternion&) Quaternion
        +rotate(const Vec3&) Vec3
    }
```

---
- `Simulator`, `Window` et `Display` détiennent chacun un **pointeur** (non-propriétaire) vers un unique `AirplaneState` partagé — c'est l'état central protégé par mutex, lu/écrit depuis plusieurs threads.
- `AirplaneState` **possède** ses `Engine` (composition, via `std::unique_ptr` — durée de vie liée) et son `Quaternion` d'orientation (`_orientation`).
- `EngineConfig` et `DataColor` sont de simples structs (pas de logique), utilisés respectivement pour construire les moteurs et pour transporter texte+couleur à afficher dans `Window`.
- `Vec3`/`Quaternion` sont branchés dans `AirplaneState` pour l'orientation — voir `docs/math.md`, section « Flux d'utilisation ». Les forces (poussée, traînée, portance, poids), calculées en `Vec3` dans `Simulator`, intègrent directement `_velocity` — voir `docs/simulator.md`, section « Forces vectorielles ».
