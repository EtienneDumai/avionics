# Simulateur — physique et boucle de simulation

Documentation de `Simulator` et `AirplaneState` : les équations physiques utilisées et le déroulement d'un tick de simulation.

---

## Vue d'ensemble

`Simulator::simLoop()` tourne dans un thread dédié, à intervalle fixe (`_tickTime = 16 ms`, soit ~60 Hz). À chaque tick, il met à jour l'état de l'avion (`AirplaneState`, protégé par mutex) : vitesses, position, altitude.

`_timeScale` (`0.016` s) est le pas de temps physique utilisé dans les intégrations — il correspond au `_tickTime` exprimé en secondes plutôt qu'en millisecondes.

---

## Modèle atmosphérique (ISA simplifié)

La densité de l'air `ρ` diminue avec l'altitude. `computeAirDensity(altitude)` utilise une approximation du modèle ISA (troposphère, jusqu'à ~11 km) :

$$\rho(h) = 1{,}225 \times \left(1 - \frac{0{,}0065 \, h}{288{,}15}\right)^{4{,}256}$$

où `h` est l'altitude en mètres, `1,225 kg/m³` la densité de l'air au niveau de la mer, et `288,15 K` la température standard au niveau de la mer.

Cette fonction est appelée à chaque tick, aussi bien pour la traînée que pour la portance — c'est la seule source de `ρ` dans le simulateur (pas de duplication du calcul).

---

## Vitesse indiquée (IAS)

`AirplaneState::computeIAS(groundSpeed)` dérive la vitesse air (`_airSpeed`) à partir de la vitesse sol et de l'altitude :

$$v_{air} = v_{sol} \times \left(1 - \frac{0{,}0065 \, h}{288{,}15}\right)^{2{,}128}$$

C'est une approximation du même effet de raréfaction de l'air que pour `ρ`, mais avec un exposant différent (lié à la relation entre densité et vitesse indiquée plutôt que densité et altitude seule). Plus l'avion monte, plus `airSpeed` diminue par rapport à `groundSpeed` à poussée égale.

---

## Vitesse sol — poussée et traînée

`Simulator::computeGroundSpeed()` calcule la force de traînée aérodynamique :

$$F_{drag} = \frac{1}{2} \, \rho \, v_{sol}^2 \, C_x \, S$$

où `Cx` (`_dragCoef`) est le coefficient de traînée (sans dimension) et `S` (`_surface`) la surface alaire (m²).

La force nette longitudinale est :

$$F_{nette} = F_{poussée} - F_{drag}$$

`F_poussée` vient de `AirplaneState::getTotalThrust()`, somme de la poussée de chaque moteur (voir plus bas).

Intégration explicite (Euler) :

$$a = \frac{F_{nette}}{m} \qquad v_{sol} \mathrel{+}= a \times \Delta t$$

avec `m` la masse (`_masse`) et `Δt = _timeScale`.

---

## Vitesse verticale — portance et poids

`Simulator::computeVerticalSpeed()` calcule la portance :

$$F_{lift} = \frac{1}{2} \, \rho \, v_{air}^2 \, C_l \, S$$

où `Cl` (`_liftCoef`) est le coefficient de portance (sans dimension). Contrairement à la traînée, la portance utilise `v_air` (vitesse air) et non `v_sol` : la portance résulte de l'écoulement de l'air sur la voilure, donc de la vitesse relative à l'air, pas au sol.

Le poids :

$$P = m \, g \qquad (g = 9{,}81 \ m/s^2)$$

Force nette verticale et intégration, même schéma que pour la vitesse sol :

$$F_{vert} = F_{lift} - P \qquad a_{vert} = \frac{F_{vert}}{m} \qquad v_{vert} \mathrel{+}= a_{vert} \times \Delta t$$

**Simplification actuelle** : la portance est traitée comme purement verticale (pas de décomposition selon l'assiette/le roulis). `Cl` est une constante fixée à la construction de `AirplaneState` — elle ne dépend pas encore de l'angle d'attaque (`_AOA`), qui existe dans `AirplaneState` mais n'est pour l'instant utilisé nulle part. Une aile réelle a un `Cl` qui varie (approximativement linéairement, hors décrochage) avec l'AOA.

---

## Poussée moteur

Chaque `Engine` a un régime courant (`_engineRPM`, en %) qui converge vers un régime commandé (`_commandRPM`) à une vitesse `_spoolRate` par tick (`Engine::updateRPM()`) — modélise l'inertie mécanique d'un réacteur (le régime ne change pas instantanément).

$$F_{poussée} = \begin{cases} \dfrac{RPM}{100} \times T_{max} & \text{si } RPM \le 100 \\[6pt] \dfrac{RPM}{100} \times T_{max} \times 1{,}25 & \text{si } RPM > 100 \end{cases}$$

où `T_max` (`_maxThrust`) est la poussée maximale nominale (N). Le facteur `1,25` au-delà de 100 % modélise une plage de surrégime (type post-combustion/reheat).

Un moteur à l'arrêt (`_state = false`) produit une poussée nulle, quel que soit son RPM.

> **Limitation actuelle** : rien n'appelle `Engine::setState()` ni `Engine::updateRPM()` dans `simLoop()`. Les moteurs sont créés à l'arrêt (`state = false`, RPM = 0) et le restent — `getTotalThrust()` renvoie donc toujours 0 tant qu'aucune commande pilote n'est branchée. La vitesse sol évolue uniquement sous l'effet de la traînée (décélération) dans l'état actuel du projet.

---

## Position

Intégration de la position à partir du cap (`_heading`, en degrés) et de la vitesse sol :

$$x \mathrel{+}= \sin(\theta) \, v_{sol} \, \Delta t \qquad y \mathrel{+}= \cos(\theta) \, v_{sol} \, \Delta t$$

avec `θ = heading × π / 180` (conversion degrés → radians). Convention : cap 0° pointe selon `+y`, cap 90° selon `+x` (repère type boussole, pas trigonométrique standard).

L'altitude s'intègre directement depuis la vitesse verticale :

$$h \mathrel{+}= v_{vert} \times \Delta t$$

---

## Déroulement d'un tick (`simLoop`)

Ordre exact des opérations dans `Simulator::simLoop()`, à chaque itération (~toutes les 16 ms) :

1. **`computeGroundSpeed()`** — traînée (fonction de `v_sol` courante) → nouvelle `v_sol`
2. **`computeIAS(v_sol)`** — recalcule `v_air` à partir de la `v_sol` **fraîchement mise à jour** à l'étape 1
3. **`computeVerticalSpeed()`** — portance (fonction de `v_air` **fraîchement mise à jour** à l'étape 2) et poids → nouvelle `v_vert`
4. **Position** : `yPos`, `xPos` mis à jour à partir de `heading` et `v_sol` (étape 1)
5. **Altitude** : mise à jour à partir de `v_vert` (étape 3)
6. `sleep(_tickTime)` avant le prochain tick

L'ordre est important : chaque grandeur dérivée (IAS, portance, position) utilise la valeur **déjà mise à jour** dans le même tick, pas celle du tick précédent — la boucle applique donc une intégration de type Euler semi-implicite plutôt qu'explicite pure.

---

## Intégration numérique — limites connues

Le schéma utilisé est un **Euler explicite/semi-implicite simple** à pas fixe (`Δt = 16 ms`). Suffisant pour l'instant, mais :

- Pas de gestion de sol/plancher : sans portance suffisante, l'altitude peut devenir négative (l'avion « tombe » indéfiniment).
- Toutes les forces sont scalaires, appliquées sur un seul axe à la fois (`v_sol` longitudinal, `v_vert` vertical) — pas encore de forces vectorielles 3D combinées (voir `docs/math.md` pour l'infrastructure `Quaternion`/`Vec3` prévue à cet effet, pas encore branchée dans `AirplaneState`).
- Si les forces deviennent grandes (par ex. `Cl` mal calibré), un pas de temps fixe en Euler simple peut accumuler de l'erreur plus vite qu'un schéma d'ordre supérieur (RK4) — à surveiller si des instabilités apparaissent.
