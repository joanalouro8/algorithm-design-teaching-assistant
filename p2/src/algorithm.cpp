#include "parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


static std::string algorithmName(const Config &config)
{
    switch (config.algorithm)
    {
    case AlgorithmKind::Basic:
        return "basic";
    case AlgorithmKind::Splitting:
        return "splitting (param=" + std::to_string(config.algorithm_param) + ")";
    case AlgorithmKind::Spilling:
        return "spilling (param=" + std::to_string(config.algorithm_param) + ")";
    case AlgorithmKind::Free:
        return "free (param=" + std::to_string(config.algorithm_param) + ")";
    }
    return "unknown";
}

static void printLiveRanges(const std::vector<LiveRange> &live_ranges)
{
    std::cout << "Live ranges (" << live_ranges.size() << "):\n";
    for (const auto &lr : live_ranges)
    {
        std::cout << "  " << lr.name << ":";
        for (const auto &p : lr.points)
        {
            std::cout << " " << p.instruction;
            if (p.def)
                std::cout << "+";
            if (p.last)
                std::cout << "-";
        }
        std::cout << "\n";
    }
}


void runBasic(const std::vector<LiveRange> &live_ranges,
              const Config &config,
              std::ofstream &output_file)
{
    std::cout << "[algorithm] basic  |  registers=" << config.num_registers << "\n";
    printLiveRanges(live_ranges);

    // TODO: implement basic register allocation
    output_file << "# Basic register allocation (placeholder)\n";
    output_file << "# registers: " << config.num_registers << "\n";
    output_file << "# live ranges: " << live_ranges.size() << "\n";
}

void runSplitting(const std::vector<LiveRange> &live_ranges,
                  const Config &config,
                  std::ofstream &output_file)
{
    std::cout << "[algorithm] splitting  |  registers=" << config.num_registers
              << "  param=" << config.algorithm_param << "\n";
    printLiveRanges(live_ranges);

    // TODO: implement splitting register allocation
    output_file << "# Splitting register allocation (placeholder)\n";
    output_file << "# registers: " << config.num_registers << "\n";
    output_file << "# param: " << config.algorithm_param << "\n";
    output_file << "# live ranges: " << live_ranges.size() << "\n";
}

void runSpilling(const std::vector<LiveRange> &live_ranges,
                 const Config &config,
                 std::ofstream &output_file)
{
    std::cout << "[algorithm] spilling  |  registers=" << config.num_registers
              << "  param=" << config.algorithm_param << "\n";
    printLiveRanges(live_ranges);

    // TODO: implement spilling register allocation
    output_file << "# Spilling register allocation (placeholder)\n";
    output_file << "# registers: " << config.num_registers << "\n";
    output_file << "# param: " << config.algorithm_param << "\n";
    output_file << "# live ranges: " << live_ranges.size() << "\n";
}

void runFree(const std::vector<LiveRange> &live_ranges,
             const Config &config,
             std::ofstream &output_file)
{
    std::cout << "[algorithm] free  |  registers=" << config.num_registers << "\n";
    printLiveRanges(live_ranges);

    // TODO: implement free register allocation
    output_file << "# Free register allocation (placeholder)\n";
    output_file << "# registers: " << config.num_registers << "\n";
    output_file << "# live ranges: " << live_ranges.size() << "\n";
}


void runAlgorithm(const std::vector<LiveRange> &live_ranges,
                  const Config &config,
                  const std::string &output_path)
{
    std::ofstream output_file(output_path);
    if (!output_file.is_open())
    {
        std::cerr << "Could not open output file: " << output_path << "\n";
        return;
    }

    switch (config.algorithm)
    {
    case AlgorithmKind::Basic:
        runBasic(live_ranges, config, output_file);
        break;
    case AlgorithmKind::Splitting:
        runSplitting(live_ranges, config, output_file);
        break;
    case AlgorithmKind::Spilling:
        runSpilling(live_ranges, config, output_file);
        break;
    case AlgorithmKind::Free:
        runFree(live_ranges, config, output_file);
        break;
    default:
        std::cerr << "Unknown algorithm variant.\n";
        return;
    }

    std::cout << "Output written to: " << output_path << "\n";
    output_file.close();
}