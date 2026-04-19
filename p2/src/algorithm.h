#pragma once

#include "parser.h"
#include <vector>
#include <string>
#include <set>
#include <fstream>
#include <iostream>
#include "Graph.h"
#include "web.h"



void runBasic(const std::vector<LiveRange> &live_ranges,
              const Config &config,
              std::ofstream &output_file);

void runSplitting(const std::vector<LiveRange> &live_ranges,
                  const Config &config,
                  std::ofstream &output_file);

void runSpilling(const std::vector<LiveRange> &live_ranges,
                 const Config &config,
                 std::ofstream &output_file);

void runFree(const std::vector<LiveRange> &live_ranges,
             const Config &config,
             std::ofstream &output_file);

void runAlgorithm(const std::vector<LiveRange> &live_ranges,
                  const Config &config,
                  const std::string &output_path);