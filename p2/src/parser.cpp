#include "parser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>


static std::string trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

static std::vector<RangePoint> parsePoints(const std::string &points_str)
{
    std::vector<RangePoint> points;
    std::istringstream ss(points_str);
    std::string token;
    while (std::getline(ss, token, ','))
    {
        token = trim(token);
        if (token.empty())
            continue;

        RangePoint rp{0, false, false};
        if (!token.empty() && token.back() == '+')
        {
            rp.def = true;
            token.pop_back();
        }
        else if (!token.empty() && token.back() == '-')
        {
            rp.last = true;
            token.pop_back();
        }

        try
        {
            rp.instruction = std::stoi(token);
        }
        catch (const std::exception &)
        {
            throw std::runtime_error("Invalid instruction number in live range: '" + token + "'");
        }
        points.push_back(rp);
    }
    return points;
}

static void parseLiveRangesFile(const std::string &path, std::vector<LiveRange> &live_ranges)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open ranges file: " + path);

    std::string line;
    while (std::getline(file, line))
    {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#')
            continue;

        size_t colon = trimmed.find(':');
        if (colon == std::string::npos)
            throw std::runtime_error("Expected 'name: points' format, got: " + trimmed);

        LiveRange lr;
        lr.name = trim(trimmed.substr(0, colon));
        lr.points = parsePoints(trimmed.substr(colon + 1));
        live_ranges.push_back(std::move(lr));
    }
}

static Config parseConfigFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file: " + path);

    Config config;
    std::string line;
    while (std::getline(file, line))
    {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#')
            continue;

        size_t colon = trimmed.find(':');
        if (colon == std::string::npos)
            throw std::runtime_error("Expected 'key: value' format, got: " + trimmed);

        std::string key = trim(trimmed.substr(0, colon));
        std::string value = trim(trimmed.substr(colon + 1));

        if (key == "registers")
        {
            try
            {
                config.num_registers = std::stoi(value);
                if (config.num_registers < 1)
                    throw std::runtime_error("registers must be >= 1");
            }
            catch (const std::runtime_error &)
            {
                throw;
            }
            catch (const std::exception &)
            {
                throw std::runtime_error("registers value must be an integer");
            }
        }
        else if (key == "algorithm")
        {
            std::istringstream ss(value);
            std::string kind;
            ss >> kind;

            if (kind == "basic")
            {
                config.algorithm = AlgorithmKind::Basic;
                config.algorithm_param = 0;
            }
            else if (kind == "splitting,")
            {
                config.algorithm = AlgorithmKind::Splitting;
                if (!(ss >> config.algorithm_param)){
                    throw std::runtime_error("algorithm 'splitting' requires a numeric parameter");
                }
            }
            else if (kind == "spilling,")
            {
                config.algorithm = AlgorithmKind::Spilling;
                if (!(ss >> config.algorithm_param)){
                    throw std::runtime_error("algorithm 'spilling' requires a numeric parameter");
                }
            }
            else if (kind == "free")
            {
                config.algorithm = AlgorithmKind::Free;
                config.algorithm_param = 0;
            }
            else
            {
                throw std::runtime_error("Unknown algorithm: '" + kind + "'. Expected: basic, splitting, spilling, free");
            }
        }
        else
        {
        }
    }
    return config;
}

Config parseInputFiles(const std::string &ranges_path,
                       const std::string &config_path,
                       std::vector<LiveRange> &live_ranges)
{
    parseLiveRangesFile(ranges_path, live_ranges);
    return parseConfigFile(config_path);
}