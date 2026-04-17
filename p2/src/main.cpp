#include <iostream>
#include <vector>
#include "parser.h"
#include "algorithm.cpp"

int main(int argc, char *argv[])
{
    if (argc != 5 || std::string(argv[1]) != "-b")
    {
        std::cerr << "Usage: myProg -b ranges.txt registers.txt allocation.txt\n";
        return 1;
    }

    const std::string ranges_path = argv[2];
    const std::string config_path = argv[3];
    const std::string output_path = argv[4];

    std::vector<LiveRange> live_ranges;
    Config config;

    try
    {
        config = parseInputFiles(ranges_path, config_path, live_ranges);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Parse error: " << e.what() << "\n";
        return 1;
    }

    runAlgorithm(live_ranges, config, output_path);

    return 0;
}