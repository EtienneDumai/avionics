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
AirplaneState makeDefaultAirplane(double altitude = 0.0)
{
    return AirplaneState(altitude, 0, 0, 150, 175, 0, 0, Quaternion(), 1, {0.2, 15000}, 70000, 0.03, 0.42, 122);
}
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
    // En altitude, l'air se raréfie : IAS < vitesse sol à poussée égale.
    EXPECT_LT(airplane.getAirSpeed(), 200.0);
    EXPECT_GT(airplane.getAirSpeed(), 0.0);
}

TEST(AirplaneStateTest, ComputeAOAIsZeroWhenForwardMatchesVelocity)
{
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward(0, 1, 0);
    Vec3 velocity(0, 1, 0);
    airplane.computeAOA(forward, velocity);
    EXPECT_NEAR(airplane.getAOA(), 0.0, 1e-6);
}

TEST(AirplaneStateTest, ComputeAOAIsPositiveWhenNoseIsHigherThanTrajectory)
{
    // forward.z (1) >= velocity.z (0) -> pas de negation -> AOA positif
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward(0, 0, 1);
    Vec3 velocity(0, 1, 0);
    airplane.computeAOA(forward, velocity);
    EXPECT_NEAR(airplane.getAOA(), 90.0, 1e-6);
}

TEST(AirplaneStateTest, ComputeAOAIsNegativeWhenNoseIsLowerThanTrajectory)
{
    // forward.z (-1) < velocity.z (0) -> negation -> AOA negatif
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward(0, 0, -1);
    Vec3 velocity(0, 1, 0);
    airplane.computeAOA(forward, velocity);
    EXPECT_NEAR(airplane.getAOA(), -90.0, 1e-6);
}

// ---------------------------------------------------------------
// Constructeurs / getters / setters
// ---------------------------------------------------------------

TEST(AirplaneStateConstructorTest, ConstructorSetsAllScalarFields)
{
    AirplaneState airplane(150.0, 10.0, 20.0, 150.0, 175.0, 5.0, 2.0, Quaternion(), 1, {0.2, 15000}, 70000, 0.03,
                            0.42, 122.0);
    EXPECT_DOUBLE_EQ(airplane.getAltitude(), 150.0);
    EXPECT_DOUBLE_EQ(airplane.getXPos(), 10.0);
    EXPECT_DOUBLE_EQ(airplane.getYPos(), 20.0);
    EXPECT_DOUBLE_EQ(airplane.getAirSpeed(), 150.0);
    EXPECT_DOUBLE_EQ(airplane.getGroundSpeed(), 175.0);
    EXPECT_DOUBLE_EQ(airplane.getVerticalSpeed(), 5.0);
    EXPECT_DOUBLE_EQ(airplane.getAOA(), 2.0);
    EXPECT_EQ(airplane.getMasse(), 70000);
    EXPECT_DOUBLE_EQ(airplane.getDragCoef(), 0.03);
    EXPECT_DOUBLE_EQ(airplane.getLiftCoef(), 0.42);
    EXPECT_DOUBLE_EQ(airplane.getSurface(), 122.0);
    EXPECT_EQ(airplane.getEnginesCount(), 1);
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
    AirplaneState airplane(0, 0, 0, 0, 0, 0, 0, orientation, 1, {0.2, 15000}, 70000, 0.03, 0.42, 122);
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
    AirplaneState original(200.0, 1.0, 2.0, 150.0, 175.0, 3.0, 4.0, Quaternion(), 1, {0.2, 15000}, 70000, 0.03, 0.42,
                            122.0);
    AirplaneState copy(&original);
    EXPECT_DOUBLE_EQ(copy.getAltitude(), 200.0);
    EXPECT_DOUBLE_EQ(copy.getXPos(), 1.0);
    EXPECT_DOUBLE_EQ(copy.getYPos(), 2.0);
    EXPECT_DOUBLE_EQ(copy.getAirSpeed(), 150.0);
    EXPECT_DOUBLE_EQ(copy.getGroundSpeed(), 175.0);
    EXPECT_DOUBLE_EQ(copy.getVerticalSpeed(), 3.0);
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
    EXPECT_DOUBLE_EQ(original.getAltitude(), 0.0);
    EXPECT_DOUBLE_EQ(copy.getAltitude(), 999.0);
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

TEST(AirplaneStateAccessorTest, SetGroundSpeedUpdatesGetter)
{
    AirplaneState airplane = makeDefaultAirplane();
    airplane.setGroundSpeed(250.0);
    EXPECT_DOUBLE_EQ(airplane.getGroundSpeed(), 250.0);
}

TEST(AirplaneStateAccessorTest, SetVerticalSpeedUpdatesGetter)
{
    AirplaneState airplane = makeDefaultAirplane();
    airplane.setVerticalSpeed(-3.5);
    EXPECT_DOUBLE_EQ(airplane.getVerticalSpeed(), -3.5);
}

TEST(AirplaneStateAccessorTest, SetAOAUpdatesGetter)
{
    AirplaneState airplane = makeDefaultAirplane();
    airplane.setAOA(6.5);
    EXPECT_DOUBLE_EQ(airplane.getAOA(), 6.5);
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
    // quelle que soit la vitesse sol. Cas limite valide (pas de NaN),
    // contrairement au cas juste au-dessus.
    double boundaryAltitude = 288.15 / 0.0065;  // ≈ 44330.77 m
    AirplaneState airplane = makeDefaultAirplane(boundaryAltitude);
    airplane.computeIAS(300.0);
    EXPECT_NEAR(airplane.getAirSpeed(), 0.0, 1e-6);
}

TEST(AirplaneStateEdgeCaseTest, ComputeIASWithNegativeGroundSpeedPropagatesSign)
{
    // Aucune validation : une vitesse sol négative (jamais censée
    // arriver) se propage telle quelle dans l'IAS.
    AirplaneState airplane = makeDefaultAirplane(0.0);
    airplane.computeIAS(-100.0);
    EXPECT_NEAR(airplane.getAirSpeed(), -100.0, 1e-9);
}

TEST(AirplaneStateEdgeCaseTest, ComputeAOAWithOppositeVectorsReachesAcosDomainBoundary)
{
    // dot(forward, velocity) = -1 exactement (vecteurs opposés) :
    // pile à la borne du domaine de acos([-1,1]). Vérifie qu'on obtient
    // 180° et pas un NaN (qui apparaîtrait si un souci de précision
    // flottante faisait déborder le dot product hors de [-1,1]).
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 forward(0, 1, 0);
    Vec3 velocity(0, -1, 0);
    airplane.computeAOA(forward, velocity);
    EXPECT_FALSE(std::isnan(airplane.getAOA()));
    EXPECT_NEAR(airplane.getAOA(), 180.0, 1e-6);
}

TEST(AirplaneStateEdgeCaseTest, ComputeAOAWithBothVectorsZeroIsDegenerateButDoesNotCrash)
{
    // normalize() sur deux vecteurs nuls renvoie (0,0,0) des deux
    // côtés -> dot = 0 -> acos(0) = 90°. Un angle "90°" qui ne veut
    // rien dire physiquement, mais le code ne détecte pas ce cas
    // dégénéré (aucune vitesse/orientation réelle en entrée).
    AirplaneState airplane = makeDefaultAirplane();
    Vec3 zeroForward;
    Vec3 zeroVelocity;
    airplane.computeAOA(zeroForward, zeroVelocity);
    EXPECT_FALSE(std::isnan(airplane.getAOA()));
    EXPECT_NEAR(airplane.getAOA(), 90.0, 1e-6);
}

TEST(AirplaneStateEdgeCaseTest, ZeroEngineCountMeansEveryIndexIsOutOfBounds)
{
    AirplaneState airplane(0, 0, 0, 0, 0, 0, 0, Quaternion(), /*engineCount=*/0, {0.2, 15000}, 70000, 0.03, 0.42,
                            122);
    EXPECT_DOUBLE_EQ(airplane.getEngineRPM(0), -1.0);
    EXPECT_EQ(airplane.getTotalThrust(), 0);
}

TEST(AirplaneStateEdgeCaseTest, ConstructorWithZeroMassStoresItWithoutValidation)
{
    // AirplaneState ne divise jamais par _masse elle-même (c'est
    // Simulator qui le fait) : une masse nulle est acceptée telle
    // quelle ici, la bombe à retardement est ailleurs.
    AirplaneState airplane(0, 0, 0, 0, 0, 0, 0, Quaternion(), 1, {0.2, 15000}, /*masse=*/0, 0.03, 0.42, 122);
    EXPECT_EQ(airplane.getMasse(), 0);
}
