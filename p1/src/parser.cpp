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

Parameters parseInputFile(const std::string &path, std::vector<Topic> &submissions,
                           std::vector<Topic> &reviewers)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open: " + path);
    }

    Parameters result;
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
                Topic submission;
                submission.id = std::stoi(fields[0]);
                submission.primary_topic = std::stoi(fields[4]);
                submission.secondary_topic = (fields.size() > 5 && !fields[5].empty()) ? std::stoi(fields[5]) : -1;
                submissions.push_back(submission);
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
                Topic reviewer;
                reviewer.id = std::stoi(fields[0]);
                reviewer.primary_topic = std::stoi(fields[3]);
                reviewer.secondary_topic = (fields.size() > 4 && !fields[4].empty()) ? std::stoi(fields[4]) : -1;
                reviewers.push_back(reviewer);
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
                    result.min_reviews_per_submission = value;
                }
                else if (fields[0] == "MaxReviewsPerReviewer")
                {
                    if (value < 1)
                    {
                        throw std::runtime_error("MaxReviewsPerReviewer must be >= 1");
                    }
                    result.max_reviews_per_reviewer = value;
                }
                else if (fields[0] == "PrimaryReviewerExpertise" ||
                         fields[0] == "SecondaryReviewerExpertise" ||
                         fields[0] == "PrimarySubmissionDomain" ||
                         fields[0] == "SecondarySubmissionDomain")
                {
                    continue;
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
                    if (value < 0)
                    {
                        throw std::runtime_error("GenerateAssignments must be >= 0");
                    }
                    result.generate_assignments = value;
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
                    result.risk_analysis = value;
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
                result.output_file_name = stripQuotes(fields[1]);
            }
        }
    }
    return result;
}