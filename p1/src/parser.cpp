#include "parser.h"
#include <fstream>
#include <stdexcept>

static std::string trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    if (first == std::string::npos)
    {
        return "";
    }
    return str.substr(first, last - first + 1);
}

static std::string stripQuotes(const std::string &str)
{
    size_t first = str.find_first_not_of("\"");
    size_t last = str.find_last_not_of("\"");
    if (first == std::string::npos)
    {
        return "";
    }
    return str.substr(first, last - first + 1);
}

static std::vector<std::string> split(const std::string &line)
{
    std::vector<std::string> fields;
    std::string current;
    bool in_quotes = false;
    for (char c : line)
    {
        if (c == '"')
        {
            in_quotes = !in_quotes;
        }
        else if (c == ',' && !in_quotes)
        {
            fields.push_back(trim(current));
            current.clear();
        }
        else
        {
            current += c;
        }
    }
    fields.push_back(trim(current));
    return fields;
}

ParsedInput parseInputFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open: " + path);
    }

    ParsedInput result;
    std::string section, line;

    while (std::getline(file, line))
    {
        std::string trimmed = trim(line);
        if (trimmed.empty())
        {
            continue;
        }

        if (trimmed[0] == '#')
        {
            std::string tag = trim(trimmed.substr(1));
            if (tag == "Submissions")
            {
                section = "Submissions";
            }
            else if (tag == "Reviewers")
            {
                section = "Reviewers";
            }
            else if (tag == "Parameters")
            {
                section = "Parameters";
            }
            else if (tag == "Control")
            {
                section = "Control";
            }
            continue;
        }

        std::vector<std::string> fields = split(trimmed);

        if (section == "Submissions" && fields.size() >= 5)
        {
            Submission submission;
            submission.id = std::stoi(fields[0]);
            submission.title = fields[1];
            submission.authors = fields[2];
            submission.email = fields[3];
            submission.primary_topic = std::stoi(fields[4]);
            submission.secondary_topic = (fields.size() > 5 && !fields[5].empty()) ? std::stoi(fields[5]) : -1;
            result.submissions.push_back(submission);
        }
        else if (section == "Reviewers" && fields.size() >= 4)
        {
            Reviewer reviewer;
            reviewer.id = std::stoi(fields[0]);
            reviewer.name = fields[1];
            reviewer.email = fields[2];
            reviewer.primary_topic = std::stoi(fields[3]);
            reviewer.secondary_topic = (fields.size() > 4 && !fields[4].empty()) ? std::stoi(fields[4]) : -1;
            result.reviewers.push_back(reviewer);
        }
        else if (section == "Parameters" && fields.size() >= 2)
        {
            int value = std::stoi(fields[1]);
            if (fields[0] == "MinReviewsPerSubmission")
            {
                result.parameters.min_reviews_per_submission = value;
            }
            else if (fields[0] == "MaxReviewsPerReviewer")
            {
                result.parameters.max_reviews_per_reviewer = value;
            }
            else if (fields[0] == "PrimaryReviewerExpertise")
            {
                result.parameters.primary_reviewer_expertise = value;
            }
            else if (fields[0] == "SecondaryReviewerExpertise")
            {
                result.parameters.secondary_reviewer_expertise = value;
            }
            else if (fields[0] == "PrimarySubmissionDomain")
            {
                result.parameters.primary_submission_domain = value;
            }
            else if (fields[0] == "SecondarySubmissionDomain")
            {
                result.parameters.secondary_submission_domain = value;
            }
        }
        else if (section == "Control" && fields.size() >= 2)
        {
            if (fields[0] == "GenerateAssignments")
            {
                result.control.generate_assignments = (std::stoi(fields[1]) != 0);
            }
            else if (fields[0] == "RiskAnalysis")
            {
                result.control.risk_analysis = (std::stoi(fields[1]) != 0);
            }
            else if (fields[0] == "OutputFileName")
            {
                result.control.output_file_name = stripQuotes(fields[1]);
            }
        }
    }
    return result;
}