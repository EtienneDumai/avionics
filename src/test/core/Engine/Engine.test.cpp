#include <gtest/gtest.h>

#include "../../../core/Engine/Engine.h"

TEST(EngineTest, StoppedEngineProducesNoThrustRegardlessOfRPM)
{
    Engine engine(5.0, 80.0, 100, 10000, /*state=*/false);
    EXPECT_EQ(engine.computeThrust(), 0);
}

TEST(EngineTest, ThrustIsProportionalToRPMBelow100Percent)
{
    Engine engine(5.0, 50.0, 50, 10000, /*state=*/true);
    // 50% de 10000 N = 5000 N
    EXPECT_EQ(engine.computeThrust(), 5000);
}

TEST(EngineTest, ThrustGetsPostCombustionBonusAbove100Percent)
{
    Engine engine(5.0, 120.0, 120, 10000, /*state=*/true);
    // 120% * 10000 * 1.25 = 15000 N (plage de surrégime)
    EXPECT_EQ(engine.computeThrust(), 15000);
}

TEST(EngineTest, UpdateRPMIncreasesTowardsCommandBySpoolRate)
{
    Engine engine(5.0, 50.0, 80, 10000, /*state=*/true);
    engine.updateRPM();
    EXPECT_DOUBLE_EQ(engine.getEngineRPM(), 55.0);
}

TEST(EngineTest, UpdateRPMDecreasesTowardsCommandBySpoolRate)
{
    Engine engine(5.0, 50.0, 30, 10000, /*state=*/true);
    engine.updateRPM();
    EXPECT_DOUBLE_EQ(engine.getEngineRPM(), 45.0);
}

TEST(EngineTest, UpdateRPMDoesNotOvershootCommand)
{
    // Spool rate (10) plus grand que l'écart restant (3) : on doit
    // s'arrêter pile sur la commande, pas la dépasser.
    Engine engine(10.0, 77.0, 80, 10000, /*state=*/true);
    engine.updateRPM();
    EXPECT_DOUBLE_EQ(engine.getEngineRPM(), 80.0);
}

TEST(EngineTest, UpdateRPMDoesNothingWhenEngineIsOff)
{
    Engine engine(5.0, 50.0, 80, 10000, /*state=*/false);
    engine.updateRPM();
    EXPECT_DOUBLE_EQ(engine.getEngineRPM(), 50.0);
}

// ---------------------------------------------------------------
// Constructeur / getters / setters
// ---------------------------------------------------------------

TEST(EngineConstructorTest, ConstructorSetsAllFieldsViaGetters)
{
    Engine engine(2.5, 45.0, 60, 12000, /*state=*/true);
    EXPECT_DOUBLE_EQ(engine.getSpoolRate(), 2.5);
    EXPECT_DOUBLE_EQ(engine.getEngineRPM(), 45.0);
    EXPECT_EQ(engine.getCommandRPM(), 60);
    EXPECT_EQ(engine.getMaxThrust(), 12000);
    EXPECT_TRUE(engine.getState());
}

TEST(EngineConstructorTest, ConstructorWithStateFalse)
{
    Engine engine(1.0, 0.0, 0, 5000, /*state=*/false);
    EXPECT_FALSE(engine.getState());
}

TEST(EngineAccessorTest, SetSpoolRateUpdatesGetter)
{
    Engine engine(1.0, 0.0, 0, 5000, true);
    engine.setSpoolRate(9.0);
    EXPECT_DOUBLE_EQ(engine.getSpoolRate(), 9.0);
}

TEST(EngineAccessorTest, SetEngineRPMUpdatesGetter)
{
    Engine engine(1.0, 0.0, 0, 5000, true);
    engine.setEngineRPM(42.0);
    EXPECT_DOUBLE_EQ(engine.getEngineRPM(), 42.0);
}

TEST(EngineAccessorTest, SetCommandRPMUpdatesGetter)
{
    Engine engine(1.0, 0.0, 0, 5000, true);
    engine.setCommandRPM(77);
    EXPECT_EQ(engine.getCommandRPM(), 77);
}

TEST(EngineAccessorTest, SetStateUpdatesGetter)
{
    Engine engine(1.0, 0.0, 0, 5000, false);
    engine.setState(true);
    EXPECT_TRUE(engine.getState());
    engine.setState(false);
    EXPECT_FALSE(engine.getState());
}

// ---------------------------------------------------------------
// Cas limites
// ---------------------------------------------------------------

TEST(EngineEdgeCaseTest, ComputeThrustAtExactly100PercentUsesNormalBranch)
{
    // La condition du code est `<= 100`, donc 100% pile ne doit PAS
    // déclencher le bonus x1.25 (branche réservée au surrégime > 100%).
    Engine engine(1.0, 100.0, 100, 10000, true);
    EXPECT_EQ(engine.computeThrust(), 10000);  // pas 12500
}

TEST(EngineEdgeCaseTest, ComputeThrustJustAbove100PercentTriggersBonus)
{
    Engine engine(1.0, 100.0001, 100, 10000, true);
    // (100.0001/100) * 10000 * 1.25 ≈ 12500.0125 -> tronqué en int (12500)
    EXPECT_EQ(engine.computeThrust(), 12500);
}

TEST(EngineEdgeCaseTest, ComputeThrustWithZeroMaxThrustIsAlwaysZero)
{
    Engine engine(1.0, 150.0, 150, 0, true);
    EXPECT_EQ(engine.computeThrust(), 0);
}

TEST(EngineEdgeCaseTest, ComputeThrustWithNegativeRPMProducesNegativeThrust)
{
    // Aucune validation dans Engine::computeThrust() : un RPM négatif
    // (jamais censé arriver en usage normal, mais rien ne l'empêche)
    // passe par la branche "<=100" et produit une poussée négative.
    Engine engine(1.0, -50.0, -50, 10000, true);
    EXPECT_EQ(engine.computeThrust(), -5000);
}

TEST(EngineEdgeCaseTest, UpdateRPMWithZeroSpoolRateNeverReachesCommand)
{
    // Spool rate nul : le moteur reste bloqué indéfiniment, même après
    // plusieurs ticks -- aucune détection de ce cas dans le code.
    Engine engine(0.0, 50.0, 90, 10000, true);
    engine.updateRPM();
    engine.updateRPM();
    engine.updateRPM();
    EXPECT_DOUBLE_EQ(engine.getEngineRPM(), 50.0);
}

TEST(EngineEdgeCaseTest, UpdateRPMWithNegativeCommandSpoolsBelowZeroUnclamped)
{
    // Aucun plancher à 0% : le régime peut devenir négatif si la
    // commande l'est, ce qui n'a pas de sens physique mais n'est
    // jamais vérifié par Engine::updateRPM().
    Engine engine(20.0, 10.0, -30, 10000, true);
    engine.updateRPM();
    EXPECT_DOUBLE_EQ(engine.getEngineRPM(), -10.0);  // max(10-20, -30) = -10
}

TEST(EngineEdgeCaseTest, UpdateRPMWhenAlreadyAtCommandDoesNotOscillate)
{
    // Branche "égal" du code : affectation directe, pas de calcul.
    Engine engine(5.0, 60.0, 60, 10000, true);
    engine.updateRPM();
    EXPECT_DOUBLE_EQ(engine.getEngineRPM(), 60.0);
}

TEST(EngineEdgeCaseTest, ComputeThrustWithVeryHighOverspeedRPM)
{
    // RPM très largement > 100% : le facteur 1.25 s'applique quand
    // même sans aucun plafond dans le code.
    Engine engine(1.0, 500.0, 500, 10000, true);
    // (500/100) * 10000 * 1.25 = 62500
    EXPECT_EQ(engine.computeThrust(), 62500);
}
