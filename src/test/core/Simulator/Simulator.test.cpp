#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "../../../core/Simulator/Simulator.h"
#include "../../../math/Quaternion/Quaternion.h"
#include "../../../state/AirplaneState.h"

// NOTE IMPORTANTE :
// computeGroundSpeed(), computeVerticalSpeed() et computeAirDensity()
// sont private -> non testables directement sans modifier Simulator
// (friend class de test, ou extraction en fonctions libres). Les tests
// ci-dessous ne couvrent donc que la surface publique + un test
// d'intégration temporel (thread réel, pas un pur test unitaire).

namespace
{
    AirplaneState makeDefaultAirplane()
    {
        return AirplaneState(150.0, 0, 0, 150, Vec3(0, 175, 0), 0, Quaternion(), 1, {0.2, 15000}, 70000, 0.03, 0.42, 122);
    }
}  // namespace

// ---------------------------------------------------------------
// Constructeur / getters
// ---------------------------------------------------------------

TEST(SimulatorConstructorTest, NewSimulatorIsNotRunning)
{
    AirplaneState airplane = makeDefaultAirplane();
    Simulator simulator(&airplane);
    EXPECT_FALSE(simulator.getSimRunning());
}

TEST(SimulatorConstructorTest, TickTimeIsFixedAt16Milliseconds)
{
    AirplaneState airplane = makeDefaultAirplane();
    Simulator simulator(&airplane);
    EXPECT_EQ(simulator.getTickTime(), 16);
}

TEST(SimulatorConstructorTest, DestructorWithoutStartingDoesNotCrash)
{
    // std::thread par défaut n'est pas joinable -> le destructeur
    // (qui vérifie joinable() avant .join()) ne fait rien. Pas
    // d'assertion à proprement parler : le test réussit s'il ne
    // plante pas / ne bloque pas.
    AirplaneState airplane = makeDefaultAirplane();
    {
        Simulator simulator(&airplane);
    }
    SUCCEED();
}

// ---------------------------------------------------------------
// Start / stop
// ---------------------------------------------------------------

TEST(SimulatorAccessorTest, StartSimSetsRunningToTrue)
{
    AirplaneState airplane = makeDefaultAirplane();
    Simulator simulator(&airplane);
    simulator.startSim();
    EXPECT_TRUE(simulator.getSimRunning());
    simulator.stopSim();
}

TEST(SimulatorAccessorTest, StopSimSetsRunningToFalse)
{
    AirplaneState airplane = makeDefaultAirplane();
    Simulator simulator(&airplane);
    simulator.startSim();
    simulator.stopSim();
    EXPECT_FALSE(simulator.getSimRunning());
}

TEST(SimulatorAccessorTest, SimLoopActuallyMutatesAirplaneState)
{
    // Test d'intégration (thread réel, dépendant du temps -- pas un
    // pur test unitaire déterministe) : on laisse tourner quelques
    // ticks et on vérifie qu'une grandeur a bien évolué. Ici la
    // vitesse sol décélère forcément puisqu'aucun moteur n'est allumé
    // (getTotalThrust() vaut toujours 0 sans commande pilote) : la
    // traînée domine largement la petite composante avant que prend
    // la portance quand la trajectoire s'incline.
    AirplaneState airplane = makeDefaultAirplane();
    double initialGroundSpeed = airplane.getGroundSpeed();
    Simulator simulator(&airplane);
    simulator.startSim();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    simulator.stopSim();
    EXPECT_LT(airplane.getGroundSpeed(), initialGroundSpeed);
}

TEST(SimulatorAccessorTest, SimLoopMakesUnpoweredAirplaneSink)
{
    // Au départ AOA = 0 -> portance nulle, seul le poids agit sur z :
    // l'avion doit commencer à s'enfoncer (vz < 0) et perdre de
    // l'altitude. La portance réapparaît ensuite avec l'AOA, mais
    // reste bien inférieure au poids sur ces quelques ticks.
    AirplaneState airplane = makeDefaultAirplane();
    double initialAltitude = airplane.getAltitude();
    Simulator simulator(&airplane);
    simulator.startSim();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    simulator.stopSim();
    EXPECT_LT(airplane.getVerticalSpeed(), 0.0);
    EXPECT_LT(airplane.getAltitude(), initialAltitude);
}

TEST(SimulatorAccessorTest, SimLoopKeepsStraightFlightInVerticalPlane)
{
    // Orientation identité, aucune vitesse latérale : aucune force
    // n'a de composante sur x (traînée colinéaire à v, portance dans
    // le plan (y,z), poids sur z) -> xPos et vx doivent rester nuls.
    AirplaneState airplane = makeDefaultAirplane();
    Simulator simulator(&airplane);
    simulator.startSim();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    simulator.stopSim();
    EXPECT_NEAR(airplane.getVelocity().getX(), 0.0, 1e-9);
    EXPECT_NEAR(airplane.getXPos(), 0.0, 1e-9);
    EXPECT_GT(airplane.getYPos(), 0.0);
}

// ---------------------------------------------------------------
// Cas limites
// ---------------------------------------------------------------

TEST(SimulatorEdgeCaseTest, CallingStopSimTwiceIsIdempotent)
{
    AirplaneState airplane = makeDefaultAirplane();
    Simulator simulator(&airplane);
    simulator.startSim();
    simulator.stopSim();
    EXPECT_NO_THROW(simulator.stopSim());
    EXPECT_FALSE(simulator.getSimRunning());
}

TEST(SimulatorEdgeCaseTest, CallingStartSimTwiceRestartsCleanlyInsteadOfCrashing)
{
    AirplaneState airplane = makeDefaultAirplane();
    Simulator simulator(&airplane);
    simulator.startSim();
    simulator.startSim();
    EXPECT_TRUE(simulator.getSimRunning());
    simulator.stopSim();
    EXPECT_FALSE(simulator.getSimRunning());
}
