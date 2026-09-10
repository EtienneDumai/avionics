#include <gtest/gtest.h>

#include "../../../math/Vec3/Vec3.h"

TEST(Vec3Test, DotProductOrthogonalVectorsIsZero)
{
    Vec3 a(1, 0, 0);
    Vec3 b(0, 1, 0);
    EXPECT_DOUBLE_EQ(a.dot(b), 0.0);
}

TEST(Vec3Test, DotProductParallelVectors)
{
    Vec3 a(2, 0, 0);
    Vec3 b(3, 0, 0);
    EXPECT_DOUBLE_EQ(a.dot(b), 6.0);
}

TEST(Vec3Test, CrossProductOfUnitAxes)
{
    // x ^ y = z (règle de la main droite)
    Vec3 x(1, 0, 0);
    Vec3 y(0, 1, 0);
    Vec3 result = x.cross(y);
    EXPECT_DOUBLE_EQ(result.getX(), 0.0);
    EXPECT_DOUBLE_EQ(result.getY(), 0.0);
    EXPECT_DOUBLE_EQ(result.getZ(), 1.0);
}

TEST(Vec3Test, CrossProductIsAnticommutative)
{
    Vec3 a(1, 2, 3);
    Vec3 b(4, 5, 6);
    Vec3 ab = a.cross(b);
    Vec3 ba = b.cross(a);
    EXPECT_DOUBLE_EQ(ab.getX(), -ba.getX());
    EXPECT_DOUBLE_EQ(ab.getY(), -ba.getY());
    EXPECT_DOUBLE_EQ(ab.getZ(), -ba.getZ());
}

TEST(Vec3Test, NormalizeReturnsUnitLength)
{
    Vec3 v(3, 4, 0);  // norme = 5
    Vec3 n = v.normalize();
    EXPECT_DOUBLE_EQ(n.getX(), 0.6);
    EXPECT_DOUBLE_EQ(n.getY(), 0.8);
    EXPECT_DOUBLE_EQ(n.getZ(), 0.0);
}

TEST(Vec3Test, NormalizeNearZeroVectorReturnsZeroVector)
{
    // Comportement documenté dans Vec3.cpp : en dessous de 1e-9,
    // on retourne le vecteur nul plutôt que de diviser par ~0.
    Vec3 v(0, 0, 0);
    Vec3 n = v.normalize();
    EXPECT_DOUBLE_EQ(n.getX(), 0.0);
    EXPECT_DOUBLE_EQ(n.getY(), 0.0);
    EXPECT_DOUBLE_EQ(n.getZ(), 0.0);
}

// ---------------------------------------------------------------
// Constructeurs / getters / setters
// ---------------------------------------------------------------

TEST(Vec3ConstructorTest, DefaultConstructorIsZeroVector)
{
    Vec3 v;
    EXPECT_DOUBLE_EQ(v.getX(), 0.0);
    EXPECT_DOUBLE_EQ(v.getY(), 0.0);
    EXPECT_DOUBLE_EQ(v.getZ(), 0.0);
}

TEST(Vec3ConstructorTest, ParameterizedConstructorSetsComponents)
{
    Vec3 v(1.5, -2.5, 3.0);
    EXPECT_DOUBLE_EQ(v.getX(), 1.5);
    EXPECT_DOUBLE_EQ(v.getY(), -2.5);
    EXPECT_DOUBLE_EQ(v.getZ(), 3.0);
}

TEST(Vec3ConstructorTest, CopyConstructorDuplicatesComponents)
{
    Vec3 original(4.0, 5.0, 6.0);
    Vec3 copy(original);
    EXPECT_DOUBLE_EQ(copy.getX(), original.getX());
    EXPECT_DOUBLE_EQ(copy.getY(), original.getY());
    EXPECT_DOUBLE_EQ(copy.getZ(), original.getZ());
}

TEST(Vec3ConstructorTest, CopyIsIndependentFromOriginal)
{
    Vec3 original(1.0, 1.0, 1.0);
    Vec3 copy(original);
    copy.setX(99.0);
    EXPECT_DOUBLE_EQ(original.getX(), 1.0);
    EXPECT_DOUBLE_EQ(copy.getX(), 99.0);
}

TEST(Vec3AccessorTest, SettersUpdateCorrespondingGetters)
{
    Vec3 v;
    v.setX(7.0);
    v.setY(8.0);
    v.setZ(9.0);
    EXPECT_DOUBLE_EQ(v.getX(), 7.0);
    EXPECT_DOUBLE_EQ(v.getY(), 8.0);
    EXPECT_DOUBLE_EQ(v.getZ(), 9.0);
}

TEST(Vec3AccessorTest, SettersDoNotAffectOtherComponents)
{
    Vec3 v(1.0, 2.0, 3.0);
    v.setY(20.0);
    EXPECT_DOUBLE_EQ(v.getX(), 1.0);
    EXPECT_DOUBLE_EQ(v.getY(), 20.0);
    EXPECT_DOUBLE_EQ(v.getZ(), 3.0);
}

// ---------------------------------------------------------------
// Cas limites
// ---------------------------------------------------------------

TEST(Vec3EdgeCaseTest, NormalizeJustBelowEpsilonThresholdReturnsZeroVector)
{
    // Vec3::normalize() utilise un seuil de 1e-9 (voir Vec3.cpp).
    // Norme ~5e-10 : sous le seuil -> vecteur nul renvoyé.
    Vec3 v(3e-10, 4e-10, 0);  // norme = 5e-10
    Vec3 n = v.normalize();
    EXPECT_DOUBLE_EQ(n.getX(), 0.0);
    EXPECT_DOUBLE_EQ(n.getY(), 0.0);
    EXPECT_DOUBLE_EQ(n.getZ(), 0.0);
}

TEST(Vec3EdgeCaseTest, NormalizeJustAboveEpsilonThresholdNormalizesNormally)
{
    // Même vecteur, mis à l'échelle pour dépasser 1e-9 : cette fois
    // il DOIT être normalisé normalement, pas renvoyé nul.
    Vec3 v(3e-8, 4e-8, 0);  // norme = 5e-8
    Vec3 n = v.normalize();
    EXPECT_NEAR(n.getX(), 0.6, 1e-9);
    EXPECT_NEAR(n.getY(), 0.8, 1e-9);
}

TEST(Vec3EdgeCaseTest, NormalizeExtremeMagnitudeOverflowsToZeroVector)
{
    // Bug numérique réel : au carré, 1e200 dépasse la plage des double
    // (~1.8e308) -> overflow vers +inf -> la norme devient inf ->
    // chaque composante / inf = 0. Le vecteur "explose" silencieusement
    // en vecteur nul au lieu de rester unitaire. Pas géré par le code.
    Vec3 v(1e200, 1e200, 1e200);
    Vec3 n = v.normalize();
    EXPECT_DOUBLE_EQ(n.getX(), 0.0);
    EXPECT_DOUBLE_EQ(n.getY(), 0.0);
    EXPECT_DOUBLE_EQ(n.getZ(), 0.0);
}

TEST(Vec3EdgeCaseTest, DotProductWithZeroVectorIsZero)
{
    Vec3 v(5, -3, 2);
    Vec3 zero;
    EXPECT_DOUBLE_EQ(v.dot(zero), 0.0);
}

TEST(Vec3EdgeCaseTest, CrossProductOfParallelVectorsIsZero)
{
    // u ^ v = 0 quand u et v sont colinéaires (sin(0°) = 0).
    Vec3 a(2, 4, 6);
    Vec3 b(1, 2, 3);  // = a / 2
    Vec3 result = a.cross(b);
    EXPECT_NEAR(result.getX(), 0.0, 1e-9);
    EXPECT_NEAR(result.getY(), 0.0, 1e-9);
    EXPECT_NEAR(result.getZ(), 0.0, 1e-9);
}

TEST(Vec3EdgeCaseTest, CrossProductOfVectorWithItselfIsZero)
{
    Vec3 v(3, -2, 5);
    Vec3 result = v.cross(v);
    EXPECT_DOUBLE_EQ(result.getX(), 0.0);
    EXPECT_DOUBLE_EQ(result.getY(), 0.0);
    EXPECT_DOUBLE_EQ(result.getZ(), 0.0);
}

TEST(Vec3EdgeCaseTest, NormalizeWithNegativeComponents)
{
    Vec3 v(-3, -4, 0);  // norme = 5
    Vec3 n = v.normalize();
    EXPECT_NEAR(n.getX(), -0.6, 1e-9);
    EXPECT_NEAR(n.getY(), -0.8, 1e-9);
}
