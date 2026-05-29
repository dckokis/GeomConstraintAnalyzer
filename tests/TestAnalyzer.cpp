#include "EquationSystem.h"
#include "Analyzer.h"

#include <gtest/gtest.h>

TEST(AnalyzerTest, WellDefinedTriangle) {
    EquationSystem equationSystem;
    equationSystem.addVariable(Variable(0, VarType::Point,  0.0, 0.0, true));  // A
    equationSystem.addVariable(Variable(1, VarType::Point,  3.0, 0.0, true));  // B
    equationSystem.addVariable(Variable(2, VarType::Point,  0.0, 4.0, false)); // C
    equationSystem.addVariable(Variable(4, VarType::Scalar, 25.0, true));      // r2_BC
    equationSystem.addVariable(Variable(5, VarType::Scalar, 16.0, true));      // r2_AC
    equationSystem.addEquation(Equation(EquationType::Distance, {1,2,4}));
    equationSystem.addEquation(Equation(EquationType::Distance, {0,2,5}));

    Analyzer a(equationSystem);
    EXPECT_EQ(a.diagnose(), Diagnosis::WellDefined);
}

TEST(AnalyzerTest, UnderDefinedOnePointFree) {
    EquationSystem equationSystem;
    equationSystem.addVariable(Variable(0, VarType::Point, 0.0, 0.0, false));
    Analyzer a(equationSystem);
    EXPECT_EQ(a.diagnose(), Diagnosis::UnderDefined);
}

TEST(AnalyzerTest, OverDefinedRedundantEquation) {
    EquationSystem equationSystem;
    equationSystem.addVariable(Variable(0, VarType::Point, 0.0, 0.0, false)); // P
    equationSystem.addVariable(Variable(1, VarType::Point, 1.0, 0.0, true));  // A
    equationSystem.addVariable(Variable(2, VarType::Point, 0.0, 1.0, true));  // B
    equationSystem.addVariable(Variable(3, VarType::Point, 2.0, 0.0, true));  // C
    equationSystem.addVariable(Variable(4, VarType::Scalar, 1.0, true)); // dist^2 PA
    equationSystem.addVariable(Variable(5, VarType::Scalar, 1.0, true)); // dist^2 PB
    equationSystem.addVariable(Variable(6, VarType::Scalar, 4.0, true)); // dist^2 PC
    equationSystem.addEquation(Equation(EquationType::Distance, {0,1,4}));
    equationSystem.addEquation(Equation(EquationType::Distance, {0,2,5}));
    equationSystem.addEquation(Equation(EquationType::Distance, {0,3,6}));
    Analyzer a(equationSystem);
    EXPECT_EQ(a.diagnose(), Diagnosis::OverDefined);
}

TEST(AnalyzerTest, EmptySystem) {
    EquationSystem equationSystem;
    Analyzer a(equationSystem);
    EXPECT_EQ(a.diagnose(), Diagnosis::Unknown);
}