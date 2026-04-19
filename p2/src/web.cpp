#include "web.h"

void add_live_range_to_web(Web &web, const LiveRange &LiveRange)
{
    for (const RangePoint &point : LiveRange.points)
    {
        web.points.insert(point);
    }
}

void make_webs(const std::vector<LiveRange> &live_ranges, std::vector<Web> &webs)
{
    int web_id = 0;

    for (const LiveRange &live_range : live_ranges)
    {
        bool addded_to_web = false;
        int live_range_use = live_range.points.back().instruction;

        for (Web &web : webs)
        {
            if (live_range_use == web.use)
            {
                add_live_range_to_web(web, live_range);
                addded_to_web = true;
                break;
            }
        }

        if (!addded_to_web)
        {
            Web web;
            web.id = web_id;
            web.use = live_range_use;
            add_live_range_to_web(web, live_range);
            webs.push_back(web);
            ++web_id;
        }
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
