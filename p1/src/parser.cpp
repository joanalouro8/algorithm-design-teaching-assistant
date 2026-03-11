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

static bool isQuotedString(const std::string &str)
{
    return str.size() >= 2 && str.front() == '"' && str.back() == '"';
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

        if (section == "Submissions")
        {
            if (fields.size() < 5)
            {
                throw std::runtime_error("Submission is missing fields (id, title, authors, email, primary_topic)");
            }
            try
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
            catch (const std::exception &)
            {
                throw std::runtime_error("Submission has invalid integer fields");
            }
        }
        else if (section == "Reviewers")
        {
            if (fields.size() < 4)
            {
                throw std::runtime_error("Reviewer is missing fields (id, name, email, primary_topic)");
            }
            try
            {
                Reviewer reviewer;
                reviewer.id = std::stoi(fields[0]);
                reviewer.name = fields[1];
                reviewer.email = fields[2];
                reviewer.primary_topic = std::stoi(fields[3]);
                reviewer.secondary_topic = (fields.size() > 4 && !fields[4].empty()) ? std::stoi(fields[4]) : -1;
                result.reviewers.push_back(reviewer);
            }
            catch (const std::exception &)
            {
                throw std::runtime_error("Reviewer has invalid integer fields");
            }
        }
        else if (section == "Parameters" && fields.size() >= 2)
        {
            try
            {
                int value = std::stoi(fields[1]);
                if (fields[0] == "MinReviewsPerSubmission")
                {
                    if (value < 1)
                    {
                        throw std::runtime_error("MinReviewsPerSubmission must be >= 1");
                    }
                    result.parameters.min_reviews_per_submission = value;
                }
                else if (fields[0] == "MaxReviewsPerReviewer")
                {
                    if (value < 1)
                    {
                        throw std::runtime_error("MaxReviewsPerReviewer must be >= 1");
                    }
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
            catch (const std::runtime_error &)
            {
                throw;
            }
            catch (const std::exception &)
            {
                throw std::runtime_error("Parameter value is not an integer: " + fields[0]);
            }
        }
        else if (section == "Control" && fields.size() >= 2)
        {
            if (fields[0] == "GenerateAssignments")
            {
                try
                {
                    int value = std::stoi(fields[1]);
                    if (value == 0)
                    {
                        result.control.generate_assignments = 0;
                    }
                    else if (value == 1)
                    {
                        result.control.generate_assignments = 1;
                    }
                    else if (value == 2)
                    {
                        result.control.generate_assignments = 2;
                    }
                    else if (value == 3)
                    {
                        result.control.generate_assignments = 3;
                    }
                    else
                    {
                        throw std::runtime_error("GenerateAssignments must be 0, 1, 2 or 3");
                    }
                }
                catch (const std::runtime_error &)
                {
                    throw;
                }
                catch (const std::exception &)
                {
                    throw std::runtime_error("GenerateAssignments must be an integer");
                }
            }
            else if (fields[0] == "RiskAnalysis")
            {
                try
                {
                    int value = std::stoi(fields[1]);
                    if (value < 0)
                    {
                        throw std::runtime_error("RiskAnalysis must be >= 0");
                    }
                    result.control.risk_analysis = value;
                }
                catch (const std::runtime_error &)
                {
                    throw;
                }
                catch (const std::exception &)
                {
                    throw std::runtime_error("RiskAnalysis must be an integer");
                }
            }
            else if (fields[0] == "OutputFileName")
            {
                std::string value_raw = trim(line.substr(line.find(',') + 1));
                if (!isQuotedString(value_raw))
                {
                    throw std::runtime_error("OutputFileName must be a quoted string, e.g. \"assign.csv\"");
                }
                result.control.output_file_name = stripQuotes(fields[1]);
            }
        }
    }
    return result;
}