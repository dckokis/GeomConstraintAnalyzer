#include "EquationSystem.h"
#include "Analyzer.h"

#include <iostream>
#include <cstdlib>

#include "Utils.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n"
            << "Input file must contain a system of equations.\n"
            << "Format details:\n"
            << "  variables\n"
            << "    id type x y fixed    (for point or vector)\n"
            << "    id scalar val fixed  (for scalar)\n"
            << "  end\n"
            << "  equations\n"
            << "    type id1 id2 ...\n"
            << "  end\n"
            << "Equation types: norm, cross, dot, distance, orient, project.\n"
            << "The program will output the diagnosis of the equation system.\n";
        return 1;
    }

    EquationSystem cs;
    if (!loadFromFile(argv[1], cs))
    {
        std::cerr << "Error: failed to load equation system from file '" << argv[1] << "'.\n";
        return 2;
    }

    Analyzer analyzer(cs);
    std::cout << "System diagnosis: " << diagnosisToString(analyzer.diagnose()) << std::endl;
    return 0;
}
