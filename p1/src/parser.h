#pragma once
#include <string>
#include <vector>

struct Submission
{
    int id;
    std::string title;
    std::string authors;
    std::string email;
    int primary_topic;
    int secondary_topic;
};

struct Reviewer
{
    int id;
    std::string name;
    std::string email;
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
    std::vector<Submission> submissions;
    std::vector<Reviewer> reviewers;
    Parameters parameters;
    Control control;
};

ParsedInput parseInputFile(const std::string &path);