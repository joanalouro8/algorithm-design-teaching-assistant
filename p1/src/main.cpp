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

    std::ofstream out(data.output_file_name);
    if (!out)
    {
        std::cerr << "Cannot open output file: " << data.output_file_name << "\n";
        return 1;
    }

    out << "Submissions: " << submissions.size() << "\n";
    for (const Topic &s : submissions)
    {
        out << "  [" << s.id << "] "
            << " primary: " << s.primary_topic;
        if (s.secondary_topic != -1)
        {
            out << " secondary: " << s.secondary_topic;
        }
        out << "\n";
    }

    out << "\nReviewers: " << reviewers.size() << "\n";
    for (const Topic &r : reviewers)
    {
        out << "  [" << r.id << "] "
            << " primary: " << r.primary_topic;
        if (r.secondary_topic != -1)
        {
            out << " secondary: " << r.secondary_topic;
        }
        out << "\n";
    }

    out << "\nParameters:\n";
    out << "  MinReviewsPerSubmission:    " << data.min_reviews_per_submission << "\n";
    out << "  MaxReviewsPerReviewer:      " << data.max_reviews_per_reviewer << "\n";

    out << "\nControl:\n";
    out << "  GenerateAssignments: " << data.generate_assignments << "\n";
    out << "  RiskAnalysis:        " << data.risk_analysis << "\n";
    out << "  OutputFileName:      " << data.output_file_name << "\n";

    std::ofstream risk(argv[3]);
    if (!risk)
    {
        std::cerr << "Cannot open risk file: " << argv[3] << "\n";
        return 1;
    }
    risk << "RiskAnalysis, " << data.risk_analysis << "\n ";
    std::cerr << "[INFO] Risk file written to: " << argv[3] << "\n";

    std::cerr << "[INFO] Output written to: " << data.output_file_name << "\n";
    return 0;
}