#pragma once
#include <string>
#include <vector>

struct RangePoint
{
    int instruction;
    bool def;  // marked with '+'
    bool last; // marked with '-'

    bool operator<(const RangePoint &other) const
    {
        if (instruction != other.instruction){
            return instruction < other.instruction;
        }
            
        auto rank = [](const RangePoint &p) -> int
        {
            if (p.last){
                return 0;
            }
            if (p.def){
                return 2;
            }
            return 1;     
        };

        return rank(*this) < rank(other);
    }

};

struct LiveRange
{
    std::string name;
    std::vector<RangePoint> points;
};

enum class AlgorithmKind
{
    Basic,
    Splitting,
    Spilling,
    Free
};

struct Config
{
    int num_registers = 1;
    AlgorithmKind algorithm = AlgorithmKind::Basic;
    int algorithm_param = 0; // numeric parameter for splitting/spilling (unused for basic/free)
};

Config parseInputFiles(const std::string &ranges_path,
                       const std::string &config_path,
                       std::vector<LiveRange> &live_ranges);