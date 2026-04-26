#pragma once

#include "parser.h"
#include <vector>
#include <string>
#include <set>
#include <fstream>
#include <iostream>
#include "interference.h"


struct Web
{
    int id;
    std::string name;
    std::set<RangePoint> points;
};

void make_webs(std::vector<LiveRange> &live_ranges, std::vector<Web> &webs);

void print_webs(const std::vector<Web> &webs, std::ofstream &output_file);