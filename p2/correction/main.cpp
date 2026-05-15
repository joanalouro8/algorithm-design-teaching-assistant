#include <iostream>
#include <vector>
#include "parser.h"
#include "validator.h"



int main(int argc, char *argv[])
{
    if (argc != 5 || std::string(argv[1]) != "-c")
    {
        std::cerr << "Usage: ./myCorrection -c ranges.txt registers.txt allocation.txt\n";
        return 1;
    }

    const std::string ranges_path = argv[2];
    const std::string registers_path = argv[3];
    const std::string output_path = argv[4];
    
    std::vector<LiveRange> live_ranges;
    Config config;
    try
    {
        config = parseInputFiles(ranges_path, registers_path, live_ranges);
        if(config.algorithm != AlgorithmKind::Basic){
            std::cerr << "The correction only works for the Basic algorithm.\n";
            return 0;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Parse error: " << e.what() << "\n";
        return 1;
    }

    ParsedOutput parsed;
    try
    {
        parsed = parseOutputFile(output_path);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error reading output file: " << e.what() << "\n";
        return 1;
    }

    std::vector<Web> truth_webs;
    make_webs(live_ranges, truth_webs);

    validateWebs(parsed, truth_webs);
    validateRegisters(parsed, config.num_registers);

    return 0;
}