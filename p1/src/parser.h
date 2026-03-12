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
    int generate_assignments = 0;
    int risk_analysis = 1;
    std::string output_file_name = "output.csv";
};



Parameters parseInputFile(const std::string &path, std::vector<Topic> &submissions,
                           std::vector<Topic> &reviewers);