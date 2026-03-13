#include <iostream>
#include <fstream>
#include "parser.h"
#include "Graph.h"
#include "algorithm.cpp"

int main(int argc, char *argv[])
{
    if (argc != 4 || std::string(argv[1]) != "-b")
    {
        std::cerr << "Usage: myProg -b input.csv risk.csv\n";
        return 1;
    }

    std::vector<Topic> submissions;
    std::vector<Topic> reviewers;
    Parameters data = parseInputFile(argv[2], submissions, reviewers);

    if(data.generate_assignments > 0){
        generateAssignments(submissions,reviewers,data);
    }

    std::ofstream risk(argv[3]);
    if (!risk)
    {
        std::cerr << "Cannot open risk file: " << argv[3] << "\n";
        return 1;
    }
    risk << "RiskAnalysis, " << data.risk_analysis << "\n ";

    return 0;
}