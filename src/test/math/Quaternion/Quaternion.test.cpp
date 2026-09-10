#include <gtest/gtest.h>

#include "../../../math/Quaternion/Quaternion.h"
#include "../../../math/Vec3/Vec3.h"

TEST(QuaternionTest, DefaultConstructorIsIdentity)
{
    Quaternion q;
    EXPECT_DOUBLE_EQ(q.getW(), 1.0);
    EXPECT_DOUBLE_EQ(q.getX(), 0.0);
    EXPECT_DOUBLE_EQ(q.getY(), 0.0);
    EXPECT_DOUBLE_EQ(q.getZ(), 0.0);
}

TEST(QuaternionTest, IdentityRotationLeavesVectorUnchanged)
{
    Quaternion identity;
    Vec3 v(0, 1, 0);
    Vec3 rotated = identity.rotate(v);
    EXPECT_NEAR(rotated.getX(), 0.0, 1e-9);
    EXPECT_NEAR(rotated.getY(), 1.0, 1e-9);
    EXPECT_NEAR(rotated.getZ(), 0.0, 1e-9);
}

TEST(QuaternionTest, NinetyDegreeRotationAroundZAxis)
{
    // Piège documenté dans docs/math.md : la règle de la main droite
    // fait tourner (0,1,0) vers (-1,0,0) avec un angle POSITIF autour
    // de (0,0,1) -- pas vers +x comme le ferait un cap aéronautique.
    Vec3 axis(0, 0, 1);
    Quaternion q(M_PI / 2.0, &axis);
    Vec3 forward(0, 1, 0);
    Vec3 rotated = q.rotate(forward);
    EXPECT_NEAR(rotated.getX(), -1.0, 1e-9);
    EXPECT_NEAR(rotated.getY(), 0.0, 1e-9);
    EXPECT_NEAR(rotated.getZ(), 0.0, 1e-9);
}

TEST(QuaternionTest, NegativeAngleMatchesAeronauticalHeadingConvention)
{
    // Correction utilisée dans le projet (voir docs/math.md, section
    // "Convention d'axes") : angle négatif pour retomber sur un cap
    // "est" (90°) qui déplace bien vers +x.
    Vec3 axis(0, 0, 1);
    Quaternion q(-M_PI / 2.0, &axis);
    Vec3 forward(0, 1, 0);
    Vec3 rotated = q.rotate(forward);
    EXPECT_NEAR(rotated.getX(), 1.0, 1e-9);
    EXPECT_NEAR(rotated.getY(), 0.0, 1e-9);
    EXPECT_NEAR(rotated.getZ(), 0.0, 1e-9);
}

TEST(QuaternionTest, MultiplyByIdentityIsNoOp)
{
    Vec3 axis(0, 0, 1);
    Quaternion q(M_PI / 4.0, &axis);
    Quaternion identity;
    Quaternion result = q.multiply(identity);
    EXPECT_NEAR(result.getW(), q.getW(), 1e-9);
    EXPECT_NEAR(result.getX(), q.getX(), 1e-9);
    EXPECT_NEAR(result.getY(), q.getY(), 1e-9);
    EXPECT_NEAR(result.getZ(), q.getZ(), 1e-9);
}

TEST(QuaternionTest, MultiplicationIsNotCommutative)
{
    Vec3 axisZ(0, 0, 1);
    Vec3 axisX(1, 0, 0);
    Quaternion q1(M_PI / 2.0, &axisZ);
    Quaternion q2(M_PI / 2.0, &axisX);

    Quaternion q1q2 = q1.multiply(q2);
    Quaternion q2q1 = q2.multiply(q1);

    // Au moins une composante doit différer entre les deux ordres.
    bool identical = std::abs(q1q2.getW() - q2q1.getW()) < 1e-9 &&
                      std::abs(q1q2.getX() - q2q1.getX()) < 1e-9 &&
                      std::abs(q1q2.getY() - q2q1.getY()) < 1e-9 &&
                      std::abs(q1q2.getZ() - q2q1.getZ()) < 1e-9;
    EXPECT_FALSE(identical);
}

// ---------------------------------------------------------------
// Constructeurs / getters / setters
// ---------------------------------------------------------------

TEST(QuaternionConstructorTest, ComponentConstructorSetsAllFields)
{
    Quaternion q(1.0, 2.0, 3.0, 4.0);
    EXPECT_DOUBLE_EQ(q.getX(), 1.0);
    EXPECT_DOUBLE_EQ(q.getY(), 2.0);
    EXPECT_DOUBLE_EQ(q.getZ(), 3.0);
    EXPECT_DOUBLE_EQ(q.getW(), 4.0);
}

TEST(QuaternionConstructorTest, AngleAxisConstructorWithZeroAngleIsIdentity)
{
    // sin(0/2)=0, cos(0/2)=1 -> identité quel que soit l'axe.
    Vec3 axis(0, 0, 1);
    Quaternion q(0.0, &axis);
    EXPECT_DOUBLE_EQ(q.getW(), 1.0);
    EXPECT_DOUBLE_EQ(q.getX(), 0.0);
    EXPECT_DOUBLE_EQ(q.getY(), 0.0);
    EXPECT_DOUBLE_EQ(q.getZ(), 0.0);
}

TEST(QuaternionConstructorTest, AngleAxisConstructorNormalizesTheAxis)
{
    // Axe non-unitaire (norme 2) : le constructeur doit normaliser
    // avant d'appliquer la formule, sinon le quaternion résultant
    // ne serait pas unitaire.
    Vec3 nonUnitAxis(0, 0, 2);
    Vec3 unitAxis(0, 0, 1);
    Quaternion fromNonUnit(M_PI / 2.0, &nonUnitAxis);
    Quaternion fromUnit(M_PI / 2.0, &unitAxis);
    EXPECT_NEAR(fromNonUnit.getW(), fromUnit.getW(), 1e-9);
    EXPECT_NEAR(fromNonUnit.getX(), fromUnit.getX(), 1e-9);
    EXPECT_NEAR(fromNonUnit.getY(), fromUnit.getY(), 1e-9);
    EXPECT_NEAR(fromNonUnit.getZ(), fromUnit.getZ(), 1e-9);
}

TEST(QuaternionConstructorTest, CopyConstructorDuplicatesComponents)
{
    Quaternion original(1.0, 2.0, 3.0, 4.0);
    Quaternion copy(original);
    EXPECT_DOUBLE_EQ(copy.getX(), original.getX());
    EXPECT_DOUBLE_EQ(copy.getY(), original.getY());
    EXPECT_DOUBLE_EQ(copy.getZ(), original.getZ());
    EXPECT_DOUBLE_EQ(copy.getW(), original.getW());
}

TEST(QuaternionAccessorTest, SettersUpdateCorrespondingGetters)
{
    Quaternion q;
    q.setX(1.0);
    q.setY(2.0);
    q.setZ(3.0);
    q.setW(4.0);
    EXPECT_DOUBLE_EQ(q.getX(), 1.0);
    EXPECT_DOUBLE_EQ(q.getY(), 2.0);
    EXPECT_DOUBLE_EQ(q.getZ(), 3.0);
    EXPECT_DOUBLE_EQ(q.getW(), 4.0);
}

TEST(QuaternionAccessorTest, AssignmentOperatorCopiesAllFields)
{
    Quaternion source(1.0, 2.0, 3.0, 4.0);
    Quaternion destination;
    destination = source;
    EXPECT_DOUBLE_EQ(destination.getX(), 1.0);
    EXPECT_DOUBLE_EQ(destination.getY(), 2.0);
    EXPECT_DOUBLE_EQ(destination.getZ(), 3.0);
    EXPECT_DOUBLE_EQ(destination.getW(), 4.0);
}

TEST(QuaternionAccessorTest, SelfAssignmentDoesNotCorruptState)
{
    // Le code protège explicitement contre `q = q;` (voir operator=).
    Quaternion q(1.0, 2.0, 3.0, 4.0);
    q = q;
    EXPECT_DOUBLE_EQ(q.getX(), 1.0);
    EXPECT_DOUBLE_EQ(q.getY(), 2.0);
    EXPECT_DOUBLE_EQ(q.getZ(), 3.0);
    EXPECT_DOUBLE_EQ(q.getW(), 4.0);
}

TEST(QuaternionAccessorTest, AssignmentReturnsReferenceForChaining)
{
    Quaternion a(1.0, 0.0, 0.0, 0.0);
    Quaternion b, c;
    c = b = a;
    EXPECT_DOUBLE_EQ(b.getX(), 1.0);
    EXPECT_DOUBLE_EQ(c.getX(), 1.0);
}

// ---------------------------------------------------------------
// Cas limites
// ---------------------------------------------------------------

TEST(QuaternionEdgeCaseTest, ConstructorWithZeroAxisProducesDegenerateQuaternion)
{
    // Aucune validation dans le code : un axe nul est silencieusement
    // normalisé en (0,0,0) par Vec3::normalize(), donc la partie
    // vectorielle du quaternion est nulle -> quaternion non-unitaire
    // (norme = |cos(angle/2)|, pas 1), rotation mal définie.
    Vec3 zeroAxis(0, 0, 0);
    Quaternion q(M_PI, &zeroAxis);
    EXPECT_NEAR(q.getX(), 0.0, 1e-9);
    EXPECT_NEAR(q.getY(), 0.0, 1e-9);
    EXPECT_NEAR(q.getZ(), 0.0, 1e-9);
    EXPECT_NEAR(q.getW(), std::cos(M_PI / 2.0), 1e-9);
}

TEST(QuaternionEdgeCaseTest, RotationByZeroAxisQuaternionCollapsesVector)
{
    // Conséquence directe du cas ci-dessus : appliquer rotate() avec ce
    // quaternion dégénéré ne préserve PAS la norme du vecteur d'entrée
    // (perte d'information), contrairement à une vraie rotation.
    Vec3 zeroAxis(0, 0, 0);
    Quaternion degenerate(M_PI / 2.0, &zeroAxis);  // w=cos(45°)≈0.707, x=y=z=0
    Vec3 v(0, 1, 0);
    Vec3 rotated = degenerate.rotate(v);
    double originalLength = std::sqrt(v.dot(v));
    double rotatedLength = std::sqrt(rotated.dot(rotated));
    EXPECT_NE(originalLength, rotatedLength);
}

TEST(QuaternionEdgeCaseTest, FullTurnUsesDoubleCoverButRotatesIdentically)
{
    // Propriété de "double cover" des quaternions : q et -q représentent
    // la même rotation. Après 2π, q = (0,0,0,-1) au lieu de l'identité
    // (0,0,0,1) -- mais rotate() doit quand même donner le même résultat.
    Vec3 axis(0, 0, 1);
    Quaternion fullTurn(2.0 * M_PI, &axis);
    EXPECT_NEAR(fullTurn.getW(), -1.0, 1e-9);  // pas +1 !
    Vec3 v(0, 1, 0);
    Vec3 rotated = fullTurn.rotate(v);
    EXPECT_NEAR(rotated.getX(), 0.0, 1e-9);
    EXPECT_NEAR(rotated.getY(), 1.0, 1e-9);
    EXPECT_NEAR(rotated.getZ(), 0.0, 1e-9);
}

TEST(QuaternionEdgeCaseTest, MultiplyByZeroQuaternionAlwaysGivesZeroQuaternion)
{
    // Aucune validation : multiplier par un quaternion "nul" (norme 0,
    // pas une rotation valide) écrase le résultat, quel que soit q1.
    Quaternion q1(1.0, 2.0, 3.0, 4.0);
    Quaternion zeroQuat(0.0, 0.0, 0.0, 0.0);
    Quaternion result = q1.multiply(zeroQuat);
    EXPECT_DOUBLE_EQ(result.getW(), 0.0);
    EXPECT_DOUBLE_EQ(result.getX(), 0.0);
    EXPECT_DOUBLE_EQ(result.getY(), 0.0);
    EXPECT_DOUBLE_EQ(result.getZ(), 0.0);
}

TEST(QuaternionEdgeCaseTest, NegativeAngleIsEquivalentToOppositeRotationDirection)
{
    Vec3 axis(0, 0, 1);
    Quaternion positive(M_PI / 3.0, &axis);
    Quaternion negative(-M_PI / 3.0, &axis);
    // Partie vectorielle inversée, partie scalaire identique (cos pair).
    EXPECT_NEAR(positive.getW(), negative.getW(), 1e-9);
    EXPECT_NEAR(positive.getZ(), -negative.getZ(), 1e-9);
}
