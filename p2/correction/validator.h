#pragma once

#include "parser.h"
#include "web.h"
#include <string>
#include <vector>
#include <set>
#include <map>

struct ParsedWeb
{
    std::string label;
    std::set<RangePoint> points;
};

struct ParsedOutput
{
    int declared_web_count = -1;
    int declared_register_count = -1;
    std::vector<ParsedWeb> webs;
    std::map<std::string, int> assignments;
};

ParsedOutput parseOutputFile(const std::string &path);

void validateWebs(const ParsedOutput &output, const std::vector<Web> &truth_webs);

void validateRegisters(const ParsedOutput &output, int num_registers);