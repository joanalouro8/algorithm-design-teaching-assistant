#pragma once

#include "parser.h"
#include <vector>
#include <string>
#include <set>
#include <stack>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "Graph.h"
#include "web.h"
#include "interference.h"

std::vector<std::pair<int, int>> greedy_graph_coloring_algorithm(Graph<int> &g, int n);

void run_algorithm(std::vector<LiveRange> &live_ranges,
                  const Config &config,
                  const std::string &output_path);