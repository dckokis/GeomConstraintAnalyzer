#include "Utils.h"
#include "EquationSystem.h"
#include "Analyzer.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

static std::unordered_map<std::string, VarType> strToType = {
    {"point", VarType::Point},
    {"vector", VarType::Vector},
    {"scalar", VarType::Scalar}
};

static std::unordered_map<std::string, EquationType> strToConstr = {
    {"norm", EquationType::Norm},
    {"cross", EquationType::Cross},
    {"dot", EquationType::Dot},
    {"distance", EquationType::Distance},
    {"orient", EquationType::Orientation},
    {"project", EquationType::Projection}
};


bool loadFromFile(const std::string& filename, EquationSystem& es)
{
    std::ifstream in(filename);
    if (!in)
    {
        return false;
    }

    std::string line;
    enum class Section { None, Vars, Eqs }
    section = Section::None;

    while (std::getline(in, line))
    {
        auto pos = line.find('#');
        if (pos != std::string::npos)
        {
            line = line.substr(0, pos);
        }

        if (line.find_first_not_of(" \t") == std::string::npos)
        {
            continue;
        }

        std::istringstream iss(line);
        std::string token;
        if (!(iss >> token))
        {
            continue;
        }

        if (token == "variables")
        {
            section = Section::Vars;
            continue;
        }
        if (token == "equations")
        {
            section = Section::Eqs;
            continue;
        }
        if (token == "end")
        {
            section = Section::None;
            continue;
        }

        std::istringstream dataStream(line);
        switch (section)
        {
        case Section::Vars:
            {
                int id;
                std::string typeName;
                double v1, v2 = 0.0;
                int fixedFlag;
                if (!(dataStream >> id >> typeName))
                {
                    return false;
                }
                if (strToType.count(typeName) == 0)
                {
                    return false;
                }
                VarType t = strToType.at(typeName);
                if (t == VarType::Scalar)
                {
                    if (!(dataStream >> v1 >> fixedFlag))
                    {
                        return false;
                    }
                    es.addVariable(Variable(id, t, v1, fixedFlag != 0));
                }
                else
                {
                    if (!(dataStream >> v1 >> v2 >> fixedFlag))
                    {
                        return false;
                    }
                    es.addVariable(Variable(id, t, v1, v2, fixedFlag != 0));
                }
                break;
            }
        case Section::Eqs:
            {
                std::string ctype;
                if (!(dataStream >> ctype))
                {
                    return false;
                }
                if (strToConstr.count(ctype) == 0)
                {
                    return false;
                }
                EquationType ct = strToConstr.at(ctype);
                std::vector<int> ids;
                int vid;
                while (dataStream >> vid)
                {
                    ids.push_back(vid);
                }
                es.addEquation(Equation(ct, ids));
                break;
            }
        case Section::None:
            break;
        }
    }
    return true;
}


std::string diagnosisToString(Diagnosis d)
{
    switch (d)
    {
    case Diagnosis::UnderDefined:
        return "UnderDefined (DOF > 0)";
    case Diagnosis::WellDefined:
        return "WellDefined (DOF = 0)";
    case Diagnosis::OverDefined:
        return "OverDefined (redundant equations)";
    case Diagnosis::Unknown:
        return "Unknown (exact diagnosis not established)";
    default:
        throw std::invalid_argument("Unknown diagnostic status");
    }
}
