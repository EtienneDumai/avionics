#include <gtest/gtest.h>

#include <cmath>

#include "../../math/Quaternion/Quaternion.h"
#include "../../math/Vec3/Vec3.h"
#include "../../state/AirplaneState.h"

namespace
{
// Construit un AirplaneState avec des valeurs proches de main.cpp,
// mais altitude=0 pour neutraliser les facteurs de correction ISA
// dans les tests IAS (le facteur vaut alors exactement 1).
// Vitesse initiale : 175 m/s plein nord (+y), vitesse verticale nulle.
AirplaneState makeDefaultAirplane(double altitude = 0.0)
{
    return AirplaneState(altitude, 0, 0, 150, Vec3(0, 175, 0), 0, Quaternion(), 1, {0.2, 15000}, 70000, 0.03, 0.42, 122);
}

// Axe latéral de référence pour computeAOA() : celui de l'orientation
// identité (getRight() = rotate((1,0,0)) = (1,0,0)).
const Vec3 RIGHT(1, 0, 0);
}  // namespace

TEST(AirplaneStateTest, ComputeIASAtSeaLevelEqualsGroundSpeed)
{
    AirplaneState airplane = makeDefaultAirplane(0.0);
    airplane.computeIAS(200.0);
    EXPECT_NEAR(airplane.getAirSpeed(), 200.0, 1e-9);
}

TEST(AirplaneStateTest, ComputeIASDecreasesWithAltitude)
{
    AirplaneState airplane = makeDefaultAirplane(5000.0);
    airplane.computeIAS(200.0);
    // En altitude, l'air se raréfie : IAS < vitesse vraie.
    EXPECT_LT(airplane.getAirSpeed(), 200.0);
    EXPECT_GT(airplane.getAirSpeed(), 0.0);
}

// ---------------------------------------------------------------
// Angle d'attaque (atan2 signé par l'axe latéral)
// ---------------------------------------------------------------

TEST(AirplaneStateTest, ComputeAOAIsZeroWhenForwardMatchesVelocity)
{
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward(0, 1, 0);
    Vec3 velocity(0, 1, 0);
    airplane.computeAOA(forward, velocity, RIGHT);
    EXPECT_NEAR(airplane.getAOA(), 0.0, 1e-6);
}

TEST(AirplaneStateTest, ComputeAOAIsPositiveWhenNoseIsHigherThanTrajectory)
{
    // (v x f) . right = +1 -> nez au-dessus de la trajectoire -> AOA positif
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward(0, 0, 1);
    Vec3 velocity(0, 1, 0);
    airplane.computeAOA(forward, velocity, RIGHT);
    EXPECT_NEAR(airplane.getAOA(), 90.0, 1e-6);
}

TEST(AirplaneStateTest, ComputeAOAIsNegativeWhenNoseIsLowerThanTrajectory)
{
    // (v x f) . right = -1 -> nez sous la trajectoire -> AOA négatif
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward(0, 0, -1);
    Vec3 velocity(0, 1, 0);
    airplane.computeAOA(forward, velocity, RIGHT);
    EXPECT_NEAR(airplane.getAOA(), -90.0, 1e-6);
}

TEST(AirplaneStateTest, ComputeAOASmallPositiveAngle)
{
    // Nez cabré de 5° au-dessus d'une trajectoire horizontale.
    const double deg = 5.0 * M_PI / 180.0;
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward(0, std::cos(deg), std::sin(deg));
    Vec3 velocity(0, 1, 0);
    airplane.computeAOA(forward, velocity, RIGHT);
    EXPECT_NEAR(airplane.getAOA(), 5.0, 1e-6);
}

TEST(AirplaneStateTest, ComputeAOAIsPositiveWhenDescendingWithLevelNose)
{
    // Cas typique de simLoop() : nez à l'horizontale, l'avion s'enfonce
    // (vz < 0) -> la trajectoire passe sous le nez -> AOA positif.
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward(0, 1, 0);
    Vec3 velocity(0, 175, -10);
    airplane.computeAOA(forward, velocity, RIGHT);
    EXPECT_NEAR(airplane.getAOA(), std::atan2(10.0, 175.0) * 180.0 / M_PI, 1e-6);
}

TEST(AirplaneStateTest, ComputeAOAIsIndependentOfVelocityMagnitude)
{
    AirplaneState slow = makeDefaultAirplane();
    AirplaneState fast = makeDefaultAirplane();
    Vec3 forward(0, 1, 0);
    slow.computeAOA(forward, Vec3(0, 1.75, -0.1), RIGHT);
    fast.computeAOA(forward, Vec3(0, 175, -10), RIGHT);
    EXPECT_NEAR(slow.getAOA(), fast.getAOA(), 1e-9);
}

// ---------------------------------------------------------------
// Constructeurs / getters / setters
// ---------------------------------------------------------------

TEST(AirplaneStateConstructorTest, ConstructorSetsAllScalarFields)
{
    AirplaneState airplane(150.0, 10.0, 20.0, 150.0, Vec3(0, 175.0, 5.0), 2.0, Quaternion(), 1, {0.2, 15000}, 70000,
                           0.03, 0.42, 122.0);
    EXPECT_DOUBLE_EQ(airplane.getAltitude(), 150.0);
    EXPECT_DOUBLE_EQ(airplane.getXPos(), 10.0);
    EXPECT_DOUBLE_EQ(airplane.getYPos(), 20.0);
    EXPECT_DOUBLE_EQ(airplane.getAirSpeed(), 150.0);
    EXPECT_NEAR(airplane.getGroundSpeed(), 175.0, 1e-9);
    EXPECT_DOUBLE_EQ(airplane.getVerticalSpeed(), 5.0);
    EXPECT_DOUBLE_EQ(airplane.getAOA(), 2.0);
    EXPECT_EQ(airplane.getMasse(), 70000);
    EXPECT_DOUBLE_EQ(airplane.getDragCoef(), 0.03);
    EXPECT_DOUBLE_EQ(airplane.getLiftCoef(), 0.42);
    EXPECT_DOUBLE_EQ(airplane.getSurface(), 122.0);
    EXPECT_EQ(airplane.getEnginesCount(), 1);
}

TEST(AirplaneStateConstructorTest, ConstructorStoresGivenVelocity)
{
    AirplaneState airplane(0, 0, 0, 0, Vec3(1.0, 2.0, 3.0), 0, Quaternion(), 1, {0.2, 15000}, 70000, 0.03, 0.42, 122);
    Vec3 v = airplane.getVelocity();
    EXPECT_DOUBLE_EQ(v.getX(), 1.0);
    EXPECT_DOUBLE_EQ(v.getY(), 2.0);
    EXPECT_DOUBLE_EQ(v.getZ(), 3.0);
}

TEST(AirplaneStateConstructorTest, ConstructorCreatesEnginesStoppedAtZeroRPM)
{
    // Voir AirplaneState::AirplaneState() : chaque moteur est créé avec
    // state=false, RPM=0 -> poussée totale nulle au départ.
    AirplaneState airplane = makeDefaultAirplane();
    EXPECT_DOUBLE_EQ(airplane.getEngineRPM(0), 0.0);
    EXPECT_EQ(airplane.getTotalThrust(), 0);
}

TEST(AirplaneStateConstructorTest, ConstructorStoresGivenOrientation)
{
    Vec3 axis(0, 0, 1);
    Quaternion orientation(M_PI / 2.0, &axis);
    AirplaneState airplane(0, 0, 0, 0, Vec3(), 0, orientation, 1, {0.2, 15000}, 70000, 0.03, 0.42, 122);
    Quaternion stored = airplane.getOrientation();
    EXPECT_NEAR(stored.getW(), orientation.getW(), 1e-9);
    EXPECT_NEAR(stored.getX(), orientation.getX(), 1e-9);
    EXPECT_NEAR(stored.getY(), orientation.getY(), 1e-9);
    EXPECT_NEAR(stored.getZ(), orientation.getZ(), 1e-9);
}

TEST(AirplaneStateConstructorTest, DefaultIdentityOrientationGivesForwardAlongY)
{
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward = airplane.getForward();
    EXPECT_NEAR(forward.getX(), 0.0, 1e-9);
    EXPECT_NEAR(forward.getY(), 1.0, 1e-9);
    EXPECT_NEAR(forward.getZ(), 0.0, 1e-9);
}

TEST(AirplaneStateConstructorTest, DefaultIdentityOrientationGivesRightAlongX)
{
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 right = airplane.getRight();
    EXPECT_NEAR(right.getX(), 1.0, 1e-9);
    EXPECT_NEAR(right.getY(), 0.0, 1e-9);
    EXPECT_NEAR(right.getZ(), 0.0, 1e-9);
}

TEST(AirplaneStateConstructorTest, HeadingIsDerivedFromForwardVector)
{
    // Orientation identité -> forward=(0,1,0) -> atan2(0,1) = 0°
    AirplaneState airplane = makeDefaultAirplane();
    EXPECT_NEAR(airplane.getHeading(), 0.0, 1e-6);
}

TEST(AirplaneStateConstructorTest, GetEngineRPMOutOfBoundsReturnsMinusOne)
{
    AirplaneState airplane = makeDefaultAirplane();
    EXPECT_DOUBLE_EQ(airplane.getEngineRPM(5), -1.0);
    EXPECT_DOUBLE_EQ(airplane.getEngineRPM(-1), -1.0);
}

TEST(AirplaneStateConstructorTest, CopyConstructorFromPointerDuplicatesState)
{
    AirplaneState original(200.0, 1.0, 2.0, 150.0, Vec3(0, 175.0, 3.0), 4.0, Quaternion(), 1, {0.2, 15000}, 70000,
                           0.03, 0.42, 122.0);
    AirplaneState copy(&original);
    EXPECT_DOUBLE_EQ(copy.getAltitude(), 200.0);
    EXPECT_DOUBLE_EQ(copy.getXPos(), 1.0);
    EXPECT_DOUBLE_EQ(copy.getYPos(), 2.0);
    EXPECT_DOUBLE_EQ(copy.getAirSpeed(), 150.0);
    Vec3 v = copy.getVelocity();
    EXPECT_DOUBLE_EQ(v.getX(), 0.0);
    EXPECT_DOUBLE_EQ(v.getY(), 175.0);
    EXPECT_DOUBLE_EQ(v.getZ(), 3.0);
    EXPECT_DOUBLE_EQ(copy.getAOA(), 4.0);
    EXPECT_EQ(copy.getMasse(), original.getMasse());
    EXPECT_DOUBLE_EQ(copy.getDragCoef(), original.getDragCoef());
    EXPECT_DOUBLE_EQ(copy.getLiftCoef(), original.getLiftCoef());
    EXPECT_DOUBLE_EQ(copy.getSurface(), original.getSurface());
}

TEST(AirplaneStateConstructorTest, CopyConstructorIsIndependentFromOriginal)
{
    AirplaneState original = makeDefaultAirplane();
    AirplaneState copy(&original);
    copy.setAltitude(999.0);
    copy.setVelocity(Vec3(1, 1, 1));
    EXPECT_DOUBLE_EQ(original.getAltitude(), 0.0);
    EXPECT_DOUBLE_EQ(copy.getAltitude(), 999.0);
    EXPECT_DOUBLE_EQ(original.getVelocity().getY(), 175.0);
}

TEST(AirplaneStateAccessorTest, SetAltitudeUpdatesGetter)
{
    AirplaneState airplane = makeDefaultAirplane();
    airplane.setAltitude(1234.0);
    EXPECT_DOUBLE_EQ(airplane.getAltitude(), 1234.0);
}

TEST(AirplaneStateAccessorTest, SetXPosAndSetYPosUpdateGetters)
{
    AirplaneState airplane = makeDefaultAirplane();
    airplane.setXPos(11.0);
    airplane.setYPos(22.0);
    EXPECT_DOUBLE_EQ(airplane.getXPos(), 11.0);
    EXPECT_DOUBLE_EQ(airplane.getYPos(), 22.0);
}

TEST(AirplaneStateAccessorTest, SetVelocityUpdatesGetter)
{
    AirplaneState airplane = makeDefaultAirplane();
    airplane.setVelocity(Vec3(10.0, 250.0, -3.5));
    Vec3 v = airplane.getVelocity();
    EXPECT_DOUBLE_EQ(v.getX(), 10.0);
    EXPECT_DOUBLE_EQ(v.getY(), 250.0);
    EXPECT_DOUBLE_EQ(v.getZ(), -3.5);
}

TEST(AirplaneStateAccessorTest, GroundSpeedIsHorizontalNormOfVelocity)
{
    // getGroundSpeed() est dérivée : norme de (vx, vy), vz ignorée.
    AirplaneState airplane = makeDefaultAirplane();
    airplane.setVelocity(Vec3(3.0, 4.0, -2.0));
    EXPECT_NEAR(airplane.getGroundSpeed(), 5.0, 1e-9);
}

TEST(AirplaneStateAccessorTest, VerticalSpeedIsZComponentOfVelocity)
{
    AirplaneState airplane = makeDefaultAirplane();
    airplane.setVelocity(Vec3(3.0, 4.0, -2.0));
    EXPECT_DOUBLE_EQ(airplane.getVerticalSpeed(), -2.0);
}

TEST(AirplaneStateAccessorTest, SetAOAUpdatesGetter)
{
    AirplaneState airplane = makeDefaultAirplane();
    airplane.setAOA(6.5);
    EXPECT_DOUBLE_EQ(airplane.getAOA(), 6.5);
}

// ---------------------------------------------------------------
// Orientation : axe latéral
// ---------------------------------------------------------------

TEST(AirplaneStateOrientationTest, RightFollowsYawRotation)
{
    // Cap est (90°) : convention docs/math.md -> angle négatif autour de (0,0,1).
    // forward passe vers +x, right doit alors pointer vers le sud (-y).
    Vec3 axis(0, 0, 1);
    Quaternion east(-M_PI / 2.0, &axis);
    AirplaneState airplane(0, 0, 0, 0, Vec3(), 0, east, 1, {0.2, 15000}, 70000, 0.03, 0.42, 122);
    Vec3 forward = airplane.getForward();
    Vec3 right = airplane.getRight();
    EXPECT_NEAR(forward.getX(), 1.0, 1e-9);
    EXPECT_NEAR(forward.getY(), 0.0, 1e-9);
    EXPECT_NEAR(right.getX(), 0.0, 1e-9);
    EXPECT_NEAR(right.getY(), -1.0, 1e-9);
    EXPECT_NEAR(right.getZ(), 0.0, 1e-9);
}

TEST(AirplaneStateOrientationTest, RightIsUnitAndPerpendicularToForward)
{
    // Orientation quelconque : une rotation conserve les normes et les angles.
    Vec3 axis(1, 2, 3);
    Quaternion q(0.7, &axis);
    AirplaneState airplane(0, 0, 0, 0, Vec3(), 0, q, 1, {0.2, 15000}, 70000, 0.03, 0.42, 122);
    Vec3 forward = airplane.getForward();
    Vec3 right = airplane.getRight();
    EXPECT_NEAR(right.dot(forward), 0.0, 1e-9);
    EXPECT_NEAR(right.length(), 1.0, 1e-9);
}

// ---------------------------------------------------------------
// Cas limites
// ---------------------------------------------------------------

TEST(AirplaneStateEdgeCaseTest, ComputeIASBeyondTropopauseAltitudeProducesNaN)
{
    // La formule ISA (voir docs/simulator.md) utilise
    // pow(1 - 0.0065*h/288.15, 2.128). La base devient négative au-delà
    // de h ≈ 44 331 m (limite de validité du modèle troposphérique) --
    // pow(négatif, exposant non-entier) est indéfini en C++ et renvoie
    // NaN. Rien dans le code ne détecte ni ne limite ce cas.
    AirplaneState airplane = makeDefaultAirplane(50000.0);
    airplane.computeIAS(200.0);
    EXPECT_TRUE(std::isnan(airplane.getAirSpeed()));
}

TEST(AirplaneStateEdgeCaseTest, ComputeIASExactlyAtTropopauseBoundaryIsZero)
{
    // Pile à la limite (base = 0) : pow(0, 2.128) = 0 -> IAS nulle,
    // quelle que soit la vitesse. Cas limite valide (pas de NaN),
    // contrairement au cas juste au-dessus.
    double boundaryAltitude = 288.15 / 0.0065;  // ≈ 44330.77 m
    AirplaneState airplane = makeDefaultAirplane(boundaryAltitude);
    airplane.computeIAS(300.0);
    EXPECT_NEAR(airplane.getAirSpeed(), 0.0, 1e-6);
}

TEST(AirplaneStateEdgeCaseTest, ComputeIASWithNegativeSpeedPropagatesSign)
{
    // Aucune validation : une vitesse négative (impossible depuis
    // simLoop(), qui passe velocity.length() >= 0) se propage telle quelle.
    AirplaneState airplane = makeDefaultAirplane(0.0);
    airplane.computeIAS(-100.0);
    EXPECT_NEAR(airplane.getAirSpeed(), -100.0, 1e-9);
}

TEST(AirplaneStateEdgeCaseTest, ComputeAOAWithOppositeVectorsGivesHalfTurn)
{
    // Vecteurs opposés : v x f = 0 et v . f = -1 -> atan2(±0, -1) = ±180°.
    // Le signe dépend du signe du zéro flottant, on ne teste que la
    // magnitude. Contrairement à acos, atan2 ne peut pas produire de NaN.
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward(0, 1, 0);
    Vec3 velocity(0, -1, 0);
    airplane.computeAOA(forward, velocity, RIGHT);
    EXPECT_FALSE(std::isnan(airplane.getAOA()));
    EXPECT_NEAR(std::abs(airplane.getAOA()), 180.0, 1e-6);
}

TEST(AirplaneStateEdgeCaseTest, ComputeAOAWithBothVectorsZeroGivesZero)
{
    // normalize() sur deux vecteurs nuls renvoie (0,0,0) des deux côtés
    // -> atan2(0, 0) = 0°. L'ancien acos(0) donnait 90° ; 0° est plus
    // sain pour un avion immobile (pas de portance parasite).
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 zeroForward;
    Vec3 zeroVelocity;
    airplane.computeAOA(zeroForward, zeroVelocity, RIGHT);
    EXPECT_FALSE(std::isnan(airplane.getAOA()));
    EXPECT_NEAR(airplane.getAOA(), 0.0, 1e-6);
}

TEST(AirplaneStateEdgeCaseTest, ZeroEngineCountMeansEveryIndexIsOutOfBounds)
{
    AirplaneState airplane(0, 0, 0, 0, Vec3(), 0, Quaternion(), /*engineCount=*/0, {0.2, 15000}, 70000, 0.03, 0.42,
                           122);
    EXPECT_DOUBLE_EQ(airplane.getEngineRPM(0), -1.0);
    EXPECT_EQ(airplane.getTotalThrust(), 0);
}

TEST(AirplaneStateEdgeCaseTest, ConstructorWithZeroMassStoresItWithoutValidation)
{
    // AirplaneState ne divise jamais par _masse elle-même (c'est
    // Simulator qui le fait) : une masse nulle est acceptée telle
    // quelle ici, la bombe à retardement est ailleurs.
    AirplaneState airplane(0, 0, 0, 0, Vec3(), 0, Quaternion(), 1, {0.2, 15000}, /*masse=*/0, 0.03, 0.42, 122);
    EXPECT_EQ(airplane.getMasse(), 0);
}
