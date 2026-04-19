#include "algorithm.h"


// bool sameInstruction(const RangePoint &a, const RangePoint &b)
// {
//     if (a.instruction != b.instruction)
//     {
//         return false;
//     }
//     if ((a.def && b.last) || (a.last && b.def))
//     {
//         return false;
//     }
//     return true;
// }

// bool has_interference(const Web &web1, const Web &web2)
// {
//     auto it_web1 = web1.points.begin();
//     auto it_web2 = web2.points.begin();

//     while (it_web1 != web1.points.end() && it_web2 != web2.points.end())
//     {
//         if (sameInstruction(*it_web1, *it_web2))
//         {
//             return true;
//         }
//         else if (*it_web1 < *it_web2)
//         {
//             ++it_web1;
//         }
//         else
//         {
//             ++it_web2;
//         }
//     }
//     return false;
// }

void runBasic(const std::vector<LiveRange> &live_ranges,
              const Config &config,
              std::ofstream &output_file)
{
    // std::cout << "[algorithm] basic  |  registers=" << config.num_registers << "\n";
    // printLiveRanges(live_ranges);

    // TODO: implement basic register allocation
    output_file << "# Basic register allocation (placeholder)\n";
    output_file << "# registers: " << config.num_registers << "\n";
    output_file << "# live ranges: " << live_ranges.size() << "\n";
}

void runSplitting(const std::vector<LiveRange> &live_ranges,
                  const Config &config,
                  std::ofstream &output_file)
{
    // std::cout << "[algorithm] splitting  |  registers=" << config.num_registers
    //           << "  param=" << config.algorithm_param << "\n";
    // printLiveRanges(live_ranges);

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
    // std::cout << "[algorithm] spilling  |  registers=" << config.num_registers
    //           << "  param=" << config.algorithm_param << "\n";
    // printLiveRanges(live_ranges);

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
    // std::cout << "[algorithm] free  |  registers=" << config.num_registers << "\n";

    // TODO: implement free register allocation
    output_file << "# Free register allocation (placeholder)\n";
    output_file << "# registers: " << config.num_registers << "\n";
    output_file << "# live ranges: " << live_ranges.size() << "\n";
}

void add_interference(const std::vector<Web> &webs, Graph<int> &g)
{
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

    std::vector<Web> webs;
    make_webs(live_ranges, webs);
    print_webs(webs, output_file);

    Graph<int> g;
    for (int i = 0; i < webs.size(); ++i)
    {
        g.addVertex(i);
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