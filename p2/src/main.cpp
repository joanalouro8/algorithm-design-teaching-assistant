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
    Config config = parseInputFile(argv[2], submissions, reviewers);

    std::ofstream riskOut;
    if (argc == 4 && config.risk_analysis > 0)
    {
        riskOut.open(argv[3]);
        if (!riskOut)
        {
            std::cerr << "Cannot open risk file: " << argv[3] << "\n";
            return 1;
        }
    }

    if (config.primary_reviewer_expertise == 1 && config.primary_submission_domain == 1)
    {
        if (config.generate_assignments == 1)
        {
            generateAssignments(submissions, reviewers, config, false, riskOut);
        }
        else if (config.generate_assignments == 2)
        {            
            generateAssignments(submissions, reviewers, config, config.secondary_submission_domain, riskOut);
        }
        else
        {
            std::cerr << "Not implemented this generate_assignment!" << "\n";
        }
    }
    else
    {
        std::cerr << "Impossible to make assignment, the primary reviewer/submission domain has to be true." << "\n";
    }

    return 0;
}