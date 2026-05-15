#include "validator.h"
#include "web.h"
#include "interference.h"
#include "algorithm.h"
#include "Graph.h"
#include <algorithm>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

static std::string trim(const std::string &s)
{
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
}

static std::set<RangePoint> parsePointSet(const std::string &points_str)
{
    std::set<RangePoint> result;
    std::istringstream ss(points_str);
    std::string token;
    while (std::getline(ss, token, ','))
    {
        token = trim(token);
        if (token.empty())
            continue;

        RangePoint rp{0, false, false};
        if (token.back() == '+')
        {
            rp.def = true;
            token.pop_back();
        }
        else if (token.back() == '-')
        {
            rp.last = true;
            token.pop_back();
        }

        try
        {
            rp.instruction = std::stoi(token);
        }
        catch (...)
        {
            throw std::runtime_error("Invalid point: '" + token + "'");
        }

        result.insert(rp);
    }
    return result;
}

ParsedOutput parseOutputFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open output file: " + path);

    std::string section = "";

    ParsedOutput out;
    std::string line;

    while (std::getline(file, line))
    {
        std::string t = trim(line);
        if (t.empty() || t[0] == '#')
            continue;

        size_t colon = t.find(':');
        if (colon == std::string::npos)
            continue;

        std::string key = trim(t.substr(0, colon));
        std::string value = trim(t.substr(colon + 1));

        if (key == "webs")
        {
            section = key;
            continue;
        }

        if (key == "registers")
        {
            try
            {
                out.declared_register_count = std::stoi(value);
            }
            catch (...)
            {
                // Give an error like it should be registers: number of registers used
            }
            section = key;
            continue;
        }

        if (section == "webs")
        {
            ParsedWeb pw;
            pw.label = key;
            pw.points = parsePointSet(value);
            out.webs.push_back(std::move(pw));
            continue;
        }

        if (section == "registers")
        {
            if (key == "M" || key == "S")
            {
                out.assignments[value] = -1;
            }
            else if (!key.empty() && key[0] == 'r')
            {
                try
                {
                    int reg = std::stoi(key.substr(1));
                    out.assignments[value] = reg;
                }
                catch (...)
                {
                }
            }
        }
    }
    return out;
}

void validateWebs(const ParsedOutput &output, const std::vector<Web> &truth_webs)
{
    bool ok = true;

    if ((int)output.webs.size() != (int)truth_webs.size())
    {
        std::cout << "FAIL (webs): expected " << truth_webs.size()
                  << " webs, got " << output.webs.size() << "\n";
        ok = false;
    }

    std::vector<bool> matched(output.webs.size(), false);

    for (const Web &tw : truth_webs)
    {
        bool found = false;
        for (size_t j = 0; j < output.webs.size(); ++j)
        {
            if (!matched[j] && output.webs[j].points == tw.points)
            {
                matched[j] = true;
                found = true;
                break;
            }
        }
        if (!found)
        {
            std::string pts;
            bool first = true;
            for (const auto &p : tw.points)
            {
                if (!first)
                    pts += ",";
                pts += std::to_string(p.instruction);
                if (p.def)
                    pts += "+";
                if (p.last)
                    pts += "-";
                first = false;
            }
            std::cout << "FAIL (webs): no match for truth web '" << tw.name
                      << "' (points: " << pts << ")\n";
            ok = false;
        }
    }

    for (size_t j = 0; j < output.webs.size(); ++j)
    {
        if (!matched[j])
        {
            std::cout << "FAIL (webs): web '" << output.webs[j].label
                      << "' has no corresponding truth web\n";
            ok = false;
        }
    }

    if (ok)
    {
        std::cout << "OK: webs are correct\n";
    }
}

void validateRegisters(const ParsedOutput &output, int num_registers)
{
    bool ok = true;

    for (const ParsedWeb &pw : output.webs)
    {
        if (output.assignments.find(pw.label) == output.assignments.end())
        {
            std::cout << "FAIL (registers): web '" << pw.label << "' has no assignment\n";
            ok = false;
        }
    }

    for (size_t i = 0; i < output.webs.size(); ++i)
    {
        if (output.assignments.count(output.webs[i].label) == 0)
        {
            continue;
        }

        int reg_i = output.assignments.at(output.webs[i].label);
        if (reg_i == -1)
        {
            continue;
        }

        for (size_t j = i + 1; j < output.webs.size(); ++j)
        {
            if (output.assignments.count(output.webs[j].label) == 0)
            {
                continue;
            }

            if (reg_i == output.assignments.at(output.webs[j].label))
            {
                Web wi;
                wi.points = output.webs[i].points;
                Web wj;
                wj.points = output.webs[j].points;
                if (has_interference(wi, wj))
                {
                    std::cout << "FAIL (registers): webs '" << output.webs[i].label
                              << "' and '" << output.webs[j].label
                              << "' interfere but are both assigned to r" << reg_i << "\n";
                    ok = false;
                }
            }
        }
    }

    std::set<int> used_registers;
    for (const auto &[label, reg] : output.assignments)
    {
        if (reg != -1)
        {
            used_registers.insert(reg);
        }
    }

    if ((int)used_registers.size() > num_registers)
    {
        std::cout << "FAIL (registers): used " << used_registers.size()
                  << " registers but only " << num_registers << " allowed\n";
        ok = false;
    }

    if (ok)
    {
        std::cout << "OK: register assignment is correct\n";
    }
}