#include "web.h"

void add_live_range_to_web(Web &web, const LiveRange &LiveRange)
{
    // std::cout << "points: ";
    for (const RangePoint &point : LiveRange.points)
    {
        // std::cout<< point.instruction << ' ';
        web.points.insert(point);
    }
    // std::cout << '\n';
}

void make_webs(std::vector<LiveRange> &live_ranges, std::vector<Web> &webs)
{
    int web_id = 0;
    for (int i = 0; i < live_ranges.size(); ++i)
    {
        if (live_ranges[i].used)
        {
            continue;
        }

        // Create a new web
        Web new_web;
        new_web.id = web_id;
        new_web.name = live_ranges[i].name;
        add_live_range_to_web(new_web, live_ranges[i]);
        live_ranges[i].used = true;

        // Populate the web wiith all the live ranges of that web
        bool has_changed = true;
        while (has_changed)
        {
            has_changed = false;
            for (LiveRange &live_range: live_ranges)
            {
                if (!live_range.used && live_range.name == new_web.name && is_same_web(new_web, live_range))
                {
                    // std::cout << live_range.points[0].instruction << '\n';
                    has_changed = true;
                    live_range.used = true;
                    add_live_range_to_web(new_web, live_range);
                }
            }
        }

        webs.push_back(new_web);
        ++web_id;
    }
}

void print_webs(const std::vector<Web> &webs, std::ofstream &output_file)
{
    output_file << "# Total number of webs followed by the listing of the program points of each one\n";
    output_file << "# program points in each web are sorted in ascending order\n";
    output_file << "webs: " << webs.size() << "\n";
    for (const auto &web : webs)
    {
        output_file << "web" << web.id << ": ";
        bool first = true;
        for (const auto &p : web.points)
        {
            if (!first)
                output_file << ",";
            output_file << p.instruction;
            if (p.def)
                output_file << "+";
            if (p.last)
                output_file << "-";
            first = false;
        }
        output_file << "\n";
    }
}
