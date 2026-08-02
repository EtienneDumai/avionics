# Avionics Simulator

Simulateur de commandes de vol en C++20, développé comme projet d'apprentissage des concepts système bas niveau : programmation concurrente, gestion mémoire, boucles temps réel.

> **Objectif** : maîtriser les fondamentaux C++ système (threads, mutex, smart pointers, boucle temps réel) à travers un domaine concret — la simulation physique d'un aéronef.

![Demo](assets/demo.gif) <!-- à ajouter -->

---

## Concepts mis en œuvre

| Concept | Implémentation |
|---|---|
| **Programmation concurrente** | Thread dédié pour la boucle de simulation (60 Hz) découplé du thread UI |
| **Synchronisation** | `std::mutex` + `std::lock_guard` sur chaque classe pour accès thread-safe |
| **Arrêt propre** | `std::atomic<bool>` pour signaler l'arrêt sans data race |
| **Gestion mémoire** | `std::unique_ptr` pour les moteurs, pas de `new`/`delete` brut |
| **Boucle temps réel** | Tick fixe à 16ms (~60 Hz) pour la simulation physique |
| **Rendu graphique** | SDL2 + SDL2_ttf pour l'affichage des données de vol |

---

## Architecture

```
src/
├── main.cpp
├── core/
│   ├── Engine/         # Modèle moteur (RPM, poussée, spool rate)
│   └── Simulator/      # Boucle de simulation physique (thread dédié)
├── math/
│   ├── Vec3/           # Vecteur 3D (dot, cross, normalize)
│   └── Quaternion/     # Rotation 3D (composition ; rotation d'un Vec3 en cours)
├── state/
│   └── AirplaneState.h/.cpp  # État partagé de l'avion (thread-safe)
└── ui/
    ├── Display/        # Affichage terminal (thread dédié)
    └── Window/         # Fenêtre SDL2
```

> `math/` n'est pas encore branché dans `AirplaneState` (toujours un cap scalaire `_heading` en degrés) — voir `docs/math.md` et la roadmap.

**Flux de données :**
```
[Thread Simulator 60Hz] ──► AirplaneState (mutex) ◄── [Thread UI / SDL2]
                                    │
                              [Engine(s) (mutex)]
```

---

## Physique simulée

- **Densité atmosphérique (ISA simplifié)** : `ρ(h)` décroît avec l'altitude, utilisée à la fois pour la traînée et la portance
- **Traînée aérodynamique** : `F_drag = 0.5 * ρ * v_sol² * Cx * S`
- **Portance** : `F_lift = 0.5 * ρ * v_air² * Cl * S` — `Cl` constant pour l'instant (pas encore dérivé de l'angle d'attaque)
- **Gravité** : intégrée à la vitesse verticale (`g = 9.81 m/s²`), combinée à la portance pour la force nette
- **Vitesse indiquée (IAS)** : calculée depuis la vitesse sol et l'altitude
- **Poussée moteur** : modélisée avec un spool rate (inertie de montée en régime) — **pas encore branchée** dans la boucle de simulation (`Engine::updateRPM()`/`setState()` ne sont jamais appelées), donc la poussée totale reste nulle en l'état
- **Position** : intégration cap + vitesse sol → coordonnées X/Y ; altitude intégrée depuis la vitesse verticale

> Détail des équations et de l'ordre exact des appels dans `simLoop()` : voir `docs/simulator.md`. Pour `Vec3`/`Quaternion` : `docs/math.md`.

---

## Stack technique

- **Langage** : C++20
- **Build** : Make (`-Wall -Wextra -Werror`)
- **Rendu** : SDL2, SDL2_ttf
- **Style** : clang-format (Google style, indent 4)
- **OS** : Linux

---

## Build & lancement

**Dépendances :**
```bash
# Debian/Ubuntu/Arch
sudo apt install libsdl2-dev libsdl2-ttf-dev   # Debian
sudo pacman -S sdl2 sdl2_ttf                   # Arch
```

**Compiler et lancer :**
```bash
./run.sh
# ou manuellement :
make
./avionics
```

**Nettoyer :**
```bash
make clean
```

---

## Paramètres de simulation

Les paramètres initiaux sont définis dans `main.cpp` :

```cpp
AirplaneState airplane(
    150,          // altitude (m)
    0, 0,         // position X, Y
    150,          // vitesse air initiale (m/s)
    175,          // vitesse sol initiale (m/s)
    0,            // vitesse verticale (m/s)
    0,            // angle d'attaque (°)
    0,            // cap (°)
    1,            // nombre de moteurs
    {0.2, 15000}, // spool rate, poussée max (N)
    70000,        // masse (kg)
    0.03,         // coefficient de traînée (Cx)
    0.42,         // coefficient de portance (Cl)
    122           // surface alaire (m²)
);
```

---

## Roadmap

- [x] Boucle de simulation physique multi-thread
- [x] Modèle moteur avec inertie (spool rate)
- [x] Affichage SDL2
- [x] Gravité et portance (Cl constant, force purement verticale)
- [x] `Vec3` (dot, cross, normalize) et `Quaternion` (composition de rotations)
- [ ] Rotation d'un `Vec3` par un `Quaternion` (`Quaternion::rotate`, en cours)
- [ ] AOA réel dérivé de l'orientation (quaternion) et du vecteur vitesse, remplaçant le `Cl` constant
- [ ] Forces vectorielles 3D (actuellement scalaires sur un seul axe à la fois)
- [ ] Dynamique angulaire (couple, moment d'inertie)
- [ ] Branchement des moteurs dans `simLoop()` (`updateRPM`/`setState` jamais appelées actuellement)
- [ ] Tests unitaires C++
- [ ] Ajout d'un GIF de démonstration
- [ ] Commandes de vol interactives (clavier)
- [ ] Intégration d'OpenGL pour une représentation plus fidèle et immersive
