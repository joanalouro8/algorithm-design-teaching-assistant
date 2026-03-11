#pragma once
#include <string>
#include <vector>

struct Topic
{
    int id;
    int primary_topic;
    int secondary_topic;
};

struct Parameters
{
    int min_reviews_per_submission = 3;
    int max_reviews_per_reviewer = 5;
    int primary_reviewer_expertise = 1;
    int secondary_reviewer_expertise = 0;
    int primary_submission_domain = 1;
    int secondary_submission_domain = 1;
};

struct Control
{
    int generate_assignments = 0;
    int risk_analysis = 1;        
    std::string output_file_name = "output.csv";
};

struct ParsedInput
{
    std::vector<Topic> submissions;
    std::vector<Topic> reviewers;
    Parameters parameters;
    Control control;
};

ParsedInput parseInputFile(const std::string &path);