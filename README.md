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
├── state/
│   └── AirplaneState/  # État partagé de l'avion (thread-safe)
└── ui/
    ├── Display/        # Affichage terminal (thread dédié)
    └── Window/         # Fenêtre SDL2
```

**Flux de données :**
```
[Thread Simulator 60Hz] ──► AirplaneState (mutex) ◄── [Thread UI / SDL2]
                                    │
                              [Engine(s) (mutex)]
```

---

## Physique simulée

- **Traînée aérodynamique** : `F_drag = 0.5 * ρ * v² * Cx * S` avec densité atmosphérique variable selon l'altitude (modèle ISA)
- **Vitesse indiquée (IAS)** : calculée depuis la vitesse sol et l'altitude
- **Poussée moteur** : modélisée avec un spool rate (inertie de montée en régime)
- **Position** : intégration cap + vitesse sol → coordonnées X/Y

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
    0.03,         // coefficient de traînée
    122           // surface alaire (m²)
);
```

---

## Roadmap

- [x] Boucle de simulation physique multi-thread
- [x] Modèle moteur avec inertie (spool rate)
- [x] Affichage SDL2
- [ ] Ajout d'un GIF de démonstration
- [ ] API WebSocket C++ pour exposer l'état en temps réel
- [ ] Frontend de visualisation connecté à l'API
- [ ] Commandes de vol interactives (clavier)