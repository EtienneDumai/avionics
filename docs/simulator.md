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

`AirplaneState::computeIAS(speed)` dérive la vitesse air (`_airSpeed`) à partir d'une vitesse scalaire et de l'altitude :

$$v_{air} = v \times \left(1 - \frac{0{,}0065 \, h}{288{,}15}\right)^{2{,}128}$$

C'est une approximation du même effet de raréfaction de l'air que pour `ρ`, mais avec un exposant différent (lié à la relation entre densité et vitesse indiquée plutôt que densité et altitude seule).

**Depuis le passage aux forces vectorielles**, `simLoop()` appelle `computeIAS(velocity.length())` — la norme **3D complète** du vecteur vitesse, et non plus seulement sa composante horizontale (`v_sol`). En montée/descente marquée, l'IAS inclut donc désormais la composante verticale de la vitesse, pas seulement le déplacement horizontal.

---

## Forces vectorielles

Le calcul de forces est entièrement en `Vec3` : plus de vitesse sol / vitesse verticale séparées, un seul `AirplaneState::_velocity` (`Vec3`) intégré à partir d'une **résultante** de quatre forces, elles-mêmes en `Vec3`. Chacune est calculée par une méthode privée dédiée de `Simulator` :

**Poussée** — `computeThrust(forward)` : alignée sur le vecteur avant de l'avion.

$$\vec{F}_{\text{poussée}} = T_{\text{total}} \cdot \hat{forward}$$

`T_total` vient de `AirplaneState::getTotalThrust()`, somme de la poussée de chaque moteur (voir plus bas).

**Traînée** — `computeDrag(v, ρ)` : opposée à la vitesse réelle (plus seulement à sa composante horizontale).

$$\vec{F}_{drag} = -\frac{1}{2} \, \rho \, \|\vec v\|^2 \, C_x \, S \cdot \hat{v}$$

où `Cx` (`_dragCoef`) et `S` (`_surface`) sont inchangés. Si `‖v‖` est quasi nulle (`< 1e-6`), on retourne le vecteur nul plutôt que de normaliser un vecteur quasi nul (même logique de garde que `Vec3::normalize()`, voir `docs/math.md`).

**Portance** — `computeLift(v, right, ρ)` : perpendiculaire à la fois à la vitesse et à l'axe latéral de l'avion.

$$\hat{dir}_{lift} = \widehat{\vec{right} \wedge \vec v} \qquad \vec{F}_{lift} = \frac{1}{2} \, \rho \, \|\vec v\|^2 \, C_l \, S \cdot \hat{dir}_{lift}$$

`Cl` reste dérivé linéairement de l'angle d'attaque, `Cl = _liftCoef × AOA` (voir section AOA ci-dessous) — `_liftCoef` est une **pente** (Cl par degré), pas un `Cl` constant.

**Différence clé avec l'ancien modèle** : la portance n'est plus forcée à être purement verticale. `right ∧ v` donne un vecteur perpendiculaire à l'axe latéral de l'avion et à la vitesse réelle — en vol normal (roulis nul, peu de sideslip) ce vecteur est proche de la verticale, mais il s'incline naturellement avec l'assiette/le roulis de l'avion, sans calcul supplémentaire.

**Poids** — `computeWeight(m)` : constant, vertical, vers le bas.

$$\vec{F}_{poids} = (0,\ 0,\ -m g) \qquad (g = 9{,}81\ m/s^2)$$

### Intégration

$$\vec F = \vec F_{\text{poussée}} + \vec F_{drag} + \vec F_{lift} + \vec F_{poids} \qquad \vec a = \frac{\vec F}{m} \qquad \vec v \mathrel{+}= \vec a \, \Delta t$$

avec `m` la masse (`_masse`) et `Δt = _timeScale`. C'est un Euler explicite classique, mais fait une seule fois sur le vecteur complet plutôt que séparément sur deux axes — voir `docs/math.md`, section « Accumulation (`+=`) et opposé » pour l'opérateur qui rend `velocity += force / m * dt` possible directement.

`AirplaneState::getGroundSpeed()` et `getVerticalSpeed()` existent toujours (pour ne pas casser `Window`/`Display`), mais sont désormais **dérivées** de `_velocity` plutôt que stockées : `getGroundSpeed()` renvoie `√(vx² + vy²)` (norme horizontale), `getVerticalSpeed()` renvoie `vz` directement. Il n'y a plus de setters correspondants — seul `setVelocity(Vec3)` existe.

### Angle d'attaque (AOA)

`AirplaneState::computeAOA(Vec3 forwardVec, Vec3 velocityVec, Vec3 right)` calcule `_AOA` (en degrés) avec un angle signé complet, via `atan2` plutôt que la simplification `acos` + comparaison de composantes utilisée auparavant :

$$AOA = \operatorname{atan2}\big((\hat v \wedge \hat f) \cdot \hat{right},\ \ \hat v \cdot \hat f\big) \times \frac{180}{\pi}$$

où `f = forwardVec.normalize()` et `v = velocityVec.normalize()`. Le produit scalaire donne le cosinus de l'angle, le produit vectoriel projeté sur `right` en donne le sinus signé — `atan2` combine les deux en un angle signé sur 360°, sans les discontinuités/ambiguïtés de signe de l'ancienne comparaison de composantes `Z`. C'est la version « plus rigoureuse » qui était envisagée comme amélioration future (voir `todo.txt`, étape 3bis) — elle nécessitait `AirplaneState::getRight()`, qui est désormais disponible (`_orientation.rotate((1,0,0))`).

**Limite qui subsiste** : `right` est l'axe latéral *de l'avion* (corps), pas un axe dérivé de la vitesse elle-même — en cas de sideslip important (vitesse qui dérive latéralement par rapport au nez), cet angle reste une approximation de l'AOA « pur tangage », pas une décomposition complète AOA/sideslip (angle β).

---

## Poussée moteur

Chaque `Engine` a un régime courant (`_engineRPM`, en %) qui converge vers un régime commandé (`_commandRPM`) à une vitesse `_spoolRate` par tick (`Engine::updateRPM()`) — modélise l'inertie mécanique d'un réacteur (le régime ne change pas instantanément).

$$F_{\text{poussée}} = \begin{cases} \dfrac{RPM}{100} \times T_{max} & \text{si } RPM \le 100 \\ \dfrac{RPM}{100} \times T_{max} \times 1{,}25 & \text{si } RPM > 100 \end{cases}$$

où `T_max` (`_maxThrust`) est la poussée maximale nominale (N). Le facteur `1,25` au-delà de 100 % modélise une plage de surrégime (type post-combustion/reheat).

Un moteur à l'arrêt (`_state = false`) produit une poussée nulle, quel que soit son RPM.

> **Limitation actuelle** : rien n'appelle `Engine::setState()` ni `Engine::updateRPM()` dans `simLoop()`. Les moteurs sont créés à l'arrêt (`state = false`, RPM = 0) et le restent — `getTotalThrust()` renvoie donc toujours 0 tant qu'aucune commande pilote n'est branchée. La vitesse évolue uniquement sous l'effet de la traînée et du poids (décélération, puis chute) dans l'état actuel du projet.

---

## Position

Intégration directe depuis les composantes du vecteur vitesse **après** mise à jour par les forces (voir section précédente) :

$$x \mathrel{+}= v_x \, \Delta t \qquad y \mathrel{+}= v_y \, \Delta t \qquad h \mathrel{+}= v_z \, \Delta t$$

**Changement important par rapport à l'ancien modèle** : la position ne passe plus par le vecteur avant (`getForward()`) — c'est `_velocity` qui la pilote directement, puisque c'est elle qui est désormais intégrée depuis les forces. `getForward()` (et `getRight()`) ne servent plus qu'à *orienter les forces elles-mêmes* (direction de la poussée, référentiel de l'AOA et de la portance) — pas à intégrer la position. Voir `docs/math.md`, section « Flux d'utilisation », pour ce changement de rôle.

`AirplaneState::getHeading()` reste disponible pour l'affichage (`Window`, `Display`) mais reste **dérivée** du vecteur avant : `atan2(forward.x, forward.y) × 180/π`, plutôt qu'un champ `_heading` stocké séparément. Voir `docs/math.md` (section « Convention d'axes ») pour le piège main droite / cap aéronautique à respecter quand on construira une orientation depuis un cap commandé.

---

## Déroulement d'un tick (`simLoop`)

Ordre exact des opérations dans `Simulator::simLoop()`, à chaque itération (~toutes les 16 ms) :

1. **Lecture de l'état courant** : `forward`, `right` (orientation), `velocity` (vitesse du tick précédent), `rho` (densité de l'air à l'altitude courante), `m` (masse) — tout est lu une fois en local au début du tick.
2. **`computeAOA(forward, velocity, right)`** — angle d'attaque, à partir de la vitesse **du tick précédent** (pas encore mise à jour ce tick) → nouvelle `_AOA`.
3. **`computeIAS(velocity.length())`** — vitesse air indiquée, à partir de la norme de cette même vitesse.
4. **Forces** : `computeThrust(forward) + computeDrag(velocity, rho) + computeLift(velocity, right, rho) + computeWeight(m)` → résultante `Vec3`. La portance utilise ici le `_AOA` **fraîchement recalculé à l'étape 2** (via `getLiftCoef() * getAOA()` dans `computeLift`).
5. **Intégration de la vitesse** : `velocity += force / m * Δt` → nouvelle `_velocity`, écrite via `setVelocity()`.
6. **Position et altitude** : `xPos`, `yPos`, `altitude` mis à jour directement depuis les composantes de la `velocity` **fraîchement intégrée** à l'étape 5.
7. `sleep(_tickTime)` avant le prochain tick.

L'ordre reste un Euler **semi-implicite** : l'AOA et l'IAS du tick utilisent encore la vitesse du tick précédent (étapes 2-3), mais la portance (étape 4) profite déjà de l'AOA recalculé la même itération, et la position (étape 6) utilise la vitesse déjà mise à jour (étape 5) — même logique de « chaque grandeur réutilise ce qui vient d'être recalculé dans le même tick » que l'ancien modèle, mais sur un seul vecteur de force plutôt que deux calculs scalaires séparés.

---

## Intégration numérique — limites connues

Le schéma utilisé est un **Euler explicite/semi-implicite simple** à pas fixe (`Δt = 16 ms`). Suffisant pour l'instant, mais :

- Pas de gestion de sol/plancher : sans portance suffisante, l'altitude peut devenir négative (l'avion « tombe » indéfiniment).
- La poussée reste toujours alignée sur le vecteur avant (pas de poussée vectorisée/orientable).
- `right` (référentiel latéral de l'AOA et de la direction de portance) est l'axe latéral *du corps* de l'avion, pas un axe recalculé à partir de la vitesse réelle — un sideslip important (vitesse qui dérive latéralement par rapport au nez) reste mal représenté ; il n'y a pas de véritable angle β de dérapage modélisé séparément de l'AOA.
- Pas de dynamique angulaire (vitesse de rotation, moment d'inertie, couple) — l'orientation (`_orientation`) ne change que si on la fixe explicitement, rien ne la fait encore évoluer tick après tick. Voir `todo.txt`, étape 6.
- Si les forces deviennent grandes (par ex. `Cl` mal calibré), un pas de temps fixe en Euler simple peut accumuler de l'erreur plus vite qu'un schéma d'ordre supérieur (RK4) — à surveiller si des instabilités apparaissent.
