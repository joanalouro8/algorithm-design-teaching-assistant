#include "algorithm.h"

std::vector<std::pair<int, int>> greedy_graph_coloring_algorithm(Graph<int> &g,
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

std::vector<std::pair<int, int>> spilling_graph_coloring(Graph<int> &g,
                                                                 int n, int spill_threshold)
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
            Vertex<int> *highest = nullptr;
            int max_degree = -1;
            for (Vertex<int> *v : vertices)
            {
                if (!v->isDisabled() && v->getDegree() > max_degree)
                {
                    max_degree = v->getDegree();
                    highest = v;
                }
            }
            if (highest)
            {
                highest->setDisabled(true);
                allocation.push_back({highest->getInfo(), -1}); // spill
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

void run_basic(Graph<int> &g, int n, std::ofstream &output_file)
{
    auto allocation = greedy_graph_coloring_algorithm(g, n);

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

void run_spilling(Graph<int> &g, int n, std::ofstream &output_file, int spill_threshold)
{
    auto allocation = spilling_graph_coloring(g, n, spill_threshold);

    std::sort(allocation.begin(), allocation.end(),
              [](const auto &a, const auto &b)
              { return a.first < b.first; });

    std::set<int> used_registers;
    int spill_count = 0;
    for (const auto &[web_id, reg] : allocation)
    {
        if (reg != -1)
            used_registers.insert(reg);
        else
            spill_count++;
    }

    output_file << "# Total number of registers used, followed by assignment to webs\n";
    output_file << "registers: " << used_registers.size() << "\n";

    if (spill_count <= spill_threshold)
        output_file << "spills: " << spill_count << "\n";

    for (const auto &[web_id, reg] : allocation)
    {
        if (reg == -1)
            output_file << "S: web" << web_id << "\n";
        else
            output_file << "r" << reg << ": web" << web_id << "\n";
    }
}

void run_algorithm(std::vector<LiveRange> &live_ranges,
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
        run_basic(g, n, output_file);
        break;
    case AlgorithmKind::Splitting:
        std::cout << "Splitting algorithm not implemented. Running basic algorithm instead.\n";
        run_basic(g, n, output_file);
        break;
    case AlgorithmKind::Spilling:
        run_spilling(g, n, output_file, config.algorithm_param);
        break;
    case AlgorithmKind::Free:
        std::cout << "Free algorithm not implemented. Running basic algorithm.\n";
        run_basic(g, n, output_file);
        break;
    default:
        std::cerr << "Unknown algorithm variant.\n";
        return;
    }

    std::cout << "Output written to: " << output_path << "\n";
    output_file.close();
}