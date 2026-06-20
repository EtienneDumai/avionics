# Mathématiques — Vec3 et Quaternion

## Vec3

Un vecteur à 3 composantes `(x, y, z)` en `double`. Utilisé pour représenter des axes de rotation, des positions, des vitesses, des forces — tout ce qui vit dans l'espace 3D.

### Norme (longueur)

$$\|v\| = \sqrt{x^2 + y^2 + z^2}$$

### Normalisation

Diviser chaque composante par la norme pour obtenir un vecteur unitaire (longueur = 1) :

$$\hat{v} = \frac{v}{\|v\|} = \left(\frac{x}{\|v\|},\ \frac{y}{\|v\|},\ \frac{z}{\|v\|}\right)$$

Si `‖v‖ < 1e-9`, on retourne le vecteur nul `(0, 0, 0)` pour éviter une division par zéro. La comparaison utilise un seuil (epsilon) plutôt que `== 0.0` car les `double` ont des erreurs de précision en virgule flottante.

---

## Quaternion

Un quaternion est un nombre hypercomplexe de la forme :

$$q = w + xi + yj + zk$$

où `w` est la partie scalaire et `(x, y, z)` est la partie vectorielle.

### Quaternion identité

$$q_{id} = (w=1,\ x=0,\ y=0,\ z=0)$$

Représente l'absence de rotation. C'est la valeur par défaut du constructeur.

### Quaternion de rotation

Pour représenter une rotation d'angle `θ` (en radians) autour d'un axe unitaire `u = (ux, uy, uz)` :

$$q = \cos\!\left(\frac{\theta}{2}\right) + \sin\!\left(\frac{\theta}{2}\right)\,(u_x\,i + u_y\,j + u_z\,k)$$

Ce qui donne :

$$w = \cos\!\left(\frac{\theta}{2}\right), \quad x = \sin\!\left(\frac{\theta}{2}\right)\cdot u_x, \quad y = \sin\!\left(\frac{\theta}{2}\right)\cdot u_y, \quad z = \sin\!\left(\frac{\theta}{2}\right)\cdot u_z$$

**L'axe doit être normalisé** avant d'appliquer la formule — sinon le quaternion résultant n'est plus unitaire et la rotation est faussée. Le constructeur `Quaternion(double angle, Vec3* axis)` normalise automatiquement l'axe.

### Pourquoi diviser l'angle par 2 ?

Un quaternion de rotation encode l'angle en deux moitiés réparties entre `w` et la partie vectorielle. C'est une conséquence de la structure algébrique des quaternions : appliquer la rotation à un vecteur `v` se fait via `q * v * q⁻¹`, ce qui "consomme" l'angle deux fois.

### Multiplication (composition de rotations)

Pour appliquer une rotation `q2` après une rotation `q1` :

$$q_{résultat} = q_1 \times q_2$$

Formule avec `q1 = (w_1, x_1, y_1, z_1)` et `q2 = (w_2, x_2, y_2, z_2)` :

$$w = w_1 w_2 - x_1 x_2 - y_1 y_2 - z_1 z_2$$
$$x = w_1 x_2 + x_1 w_2 + y_1 z_2 - z_1 y_2$$
$$y = w_1 y_2 - x_1 z_2 + y_1 w_2 + z_1 x_2$$
$$z = w_1 z_2 + x_1 y_2 - y_1 x_2 + z_1 w_2$$

**Attention : la multiplication de quaternions n'est pas commutative.** `q1 * q2 ≠ q2 * q1`. L'ordre compte — appliquer pitch puis yaw n'est pas la même rotation qu'yaw puis pitch.

### Pourquoi les quaternions plutôt que les angles d'Euler ?

Les angles d'Euler (pitch, roll, yaw séparés) souffrent du **gimbal lock** : quand un axe s'aligne avec un autre, un degré de liberté est perdu et certaines rotations deviennent impossibles à représenter. Pour un chasseur capable de manœuvres extrêmes (looping, tonneau), c'est un problème réel.

Les quaternions représentent une rotation comme un **axe + un angle** et composent les rotations par multiplication — pas de singularité, pas de gimbal lock.

### Flux d'utilisation prévu

```
orientation (Quaternion)
    → vecteur avant dans le monde (Vec3)
    → vitesse_monde = airspeed × vecteur_avant
```

Pour appliquer du pitch/yaw/roll à chaque frame :

```
Quaternion delta = Quaternion(anglePitch, axePitch);
orientation = orientation.multiply(delta);
```
