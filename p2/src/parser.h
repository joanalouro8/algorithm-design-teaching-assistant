#pragma once
#include <string>
#include <vector>

struct Topic
{
    int id;
    int primary_topic;
    int secondary_topic;
};

struct Config
{
    int min_reviews_per_submission = 1;
    int max_reviews_per_reviewer = 1;
    int primary_reviewer_expertise = 1;
    int secondary_reviewer_expertise = 0;
    int primary_submission_domain = 1;
    int secondary_submission_domain = 0;

    int generate_assignments = 0;
    int risk_analysis = 0;
    std::string output_file_name = "output.csv";
};

Config parseInputFile(const std::string &path, std::vector<Topic> &submissions,
                          std::vector<Topic> &reviewers);