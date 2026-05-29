#pragma once

#include <string>

enum class Diagnosis;
class EquationSystem;


bool loadFromFile(const std::string& filename, EquationSystem& es);

std::string diagnosisToString(Diagnosis d);
