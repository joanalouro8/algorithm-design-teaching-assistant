#include <iostream>
#include <fstream>
#include "parser.h"

int main(int argc, char *argv[])
{
    if (argc < 3 || std::string(argv[1]) != "-b")
    {
        std::cerr << "Usage: myProg -b input.csv [risk.csv]\n";
        return 1;
    }

    ParsedInput data = parseInputFile(argv[2]);



    std::ofstream out(data.control.output_file_name);
    if (!out)
    {
        std::cerr << "Cannot open output file: " << data.control.output_file_name << "\n";
        return 1;
    }

    out << "Submissions: " << data.submissions.size() << "\n";
    for (const Topic &s : data.submissions)
    {
        out << "  [" << s.id << "] "
            << " primary: " << s.primary_topic;
        if (s.secondary_topic != -1)
        {
            out << " secondary: " << s.secondary_topic;
        }
        out << "\n";
    }

    out << "\nReviewers: " << data.reviewers.size() << "\n";
    for (const Topic &r : data.reviewers)
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
    out << "  MinReviewsPerSubmission:    " << data.parameters.min_reviews_per_submission << "\n";
    out << "  MaxReviewsPerReviewer:      " << data.parameters.max_reviews_per_reviewer << "\n";
    out << "  PrimaryReviewerExpertise:   " << data.parameters.primary_reviewer_expertise << "\n";
    out << "  SecondaryReviewerExpertise: " << data.parameters.secondary_reviewer_expertise << "\n";
    out << "  PrimarySubmissionDomain:    " << data.parameters.primary_submission_domain << "\n";
    out << "  SecondarySubmissionDomain:  " << data.parameters.secondary_submission_domain << "\n";

    out << "\nControl:\n";
    out << "  GenerateAssignments: " << data.control.generate_assignments << "\n";
    out << "  RiskAnalysis:        " << data.control.risk_analysis << "\n";
    out << "  OutputFileName:      " << data.control.output_file_name << "\n";

    if (data.control.risk_analysis && argc >= 4)
    {
        std::ofstream risk(argv[3]);
        if (!risk)
        {
            std::cerr << "Cannot open risk file: " << argv[3] << "\n";
            return 1;
        }
        risk << "RiskAnalysis, 1\n";
        std::cerr << "[INFO] Risk file written to: " << argv[3] << "\n";
    }

    std::cerr << "[INFO] Output written to: " << data.control.output_file_name << "\n";
    return 0;
}