#include <iostream>
#include <fstream>
#include "parser.h"
#include "Graph.h"
#include "algorithm.cpp"

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 4 || std::string(argv[1]) != "-b")
    {
        std::cerr << "Usage: myProg -b input.csv [risk.csv]\n";
        return 1;
    }

    std::vector<Topic> submissions;
    std::vector<Topic> reviewers;
    Parameters data = parseInputFile(argv[2], submissions, reviewers);

    std::ofstream riskOut;
    if (argc == 4)
    {
        riskOut.open(argv[3]);
        if (!riskOut)
        {
            std::cerr << "Cannot open risk file: " << argv[3] << "\n";
            return 1;
        }
    }

    if (data.generate_assignments > 0)
        generateAssignments(submissions, reviewers, data, riskOut);

    return 0;
}