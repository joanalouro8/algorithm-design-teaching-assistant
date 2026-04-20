#include "algorithm.h"

std::vector<std::pair<int, int>> greedyGraphColoringAlgorithm(Graph<int> &g,
                                                              int n)
{
    std::vector<std::pair<int, int>> allocation; // (web_id, register_id)
    std::stack<Vertex<int> *> s;

    // 1. Clearing cliques
    while (true)
    {
        std::vector<Vertex<int> *> vertices = g.getVertexSet();
        if (vertices.empty())
            break;

        bool active_vertex = false;
        for (Vertex<int> *v : vertices)
        {
            if (!v->isDisabled())
            {
                active_vertex = true;
                break;
            }
        }

        if (!active_vertex)
            break;

        for (Vertex<int> *v : vertices)
        {
            if (!v->isDisabled())
            {
                if (v->getDegree() < n)
                {
                    s.push(v);
                    v->setDisabled(true);
                }
            }
        }

        bool all_disabled = true;
        bool all_degree_big = true;

        for (Vertex<int> *v : vertices)
        {
            if (!v->isDisabled())
            {
                all_disabled = false;
                if (v->getDegree() < n)
                {
                    all_degree_big = false;
                    break;
                }
            }
        }

        if (all_disabled)
            break;
        if (all_degree_big)
        {
            for (Vertex<int> *v : vertices)
            {
                if (!v->isDisabled())
                {
                    v->setDisabled(true);
                    allocation.push_back({v->getInfo(), -1}); // spill
                    break;
                }
            }
        }
    }

    // 2. Coloring phase
    while (!s.empty())
    {
        Vertex<int> *v = s.top();
        s.pop();
        int color = v->assign_color();
        allocation.push_back({v->getInfo(), color});
    }

    return allocation;
}

void runBasic(Graph<int> &g, int n, std::ofstream &output_file)
{
    auto allocation = greedyGraphColoringAlgorithm(g, n);

    std::sort(allocation.begin(), allocation.end(),
              [](const auto &a, const auto &b)
              { return a.first < b.first; });

    std::set<int> used_registers;
    for (const auto &[web_id, reg] : allocation)
        if (reg != -1)
            used_registers.insert(reg);

    output_file << "# Total number of registers used, followed by assignment to webs\n";
    output_file << "registers: " << used_registers.size() << "\n";
    for (const auto &[web_id, reg] : allocation)
    {
        if (reg == -1)
            output_file << "M: web" << web_id << "\n";
        else
            output_file << "r" << reg << ": web" << web_id << "\n";
    }
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

    int n = config.num_registers;

    std::vector<Web> webs;
    make_webs(live_ranges, webs);
    print_webs(webs, output_file);

    Graph<int> g;
    for (int i = 0; i < webs.size(); ++i)
    {
        g.addVertex(i);
        for (int j = 0; j < i; ++j)
        {
            if (has_interference(webs[i], webs[j]))
            {
                g.addEdge(i, j, 1);
                g.addEdge(j, i, 1);
            }
        }
    }

    switch (config.algorithm)
    {
    case AlgorithmKind::Basic:
        runBasic(g, n, output_file);
        break;
    case AlgorithmKind::Splitting:
        runSplitting(live_ranges, config, output_file);
        break;
    case AlgorithmKind::Spilling:
        runSpilling(live_ranges, config, output_file);
        break;
    case AlgorithmKind::Free:
        runBasic(g, n, output_file);
        break;
    default:
        std::cerr << "Unknown algorithm variant.\n";
        return;
    }

    std::cout << "Output written to: " << output_path << "\n";
    output_file.close();
}