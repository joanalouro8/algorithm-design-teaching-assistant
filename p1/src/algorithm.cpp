#include "Graph.h"
#include "parser.h"
#include <limits>

void generateAssignments(std::vector<Topic> &submissions_topic, std::vector<Topic> &reviewers_topic, Parameters data)
{
    Graph<int> my_graph;
    my_graph.addVertex(0);
    my_graph.addVertex(1);
    Vertex<int> *source = my_graph.findVertex(0);
    Vertex<int> *terminal = my_graph.findVertex(1);
    std::vector<Vertex<int>*> submissions(submissions_topic.size());
    int index = 0;
    for(Topic submission : submissions_topic){
        my_graph.addVertex(index);
        Vertex<int> *new_submission = my_graph.findVertex(index);
        new_submission->setId(submission.id);
        new_submission->setExpertise(submission.primary_topic);
        source->addEdge(new_submission, data.min_reviews_per_submission);
        submissions[index] = new_submission;
        ++index;
    }

    std::vector<Vertex<int>*> reviewers(reviewers_topic.size());
    index = 0;
    for (Topic reviewer : reviewers_topic)
    {
        my_graph.addVertex(index);
        Vertex<int> *new_reviewer = my_graph.findVertex(index);
        new_reviewer->setId(reviewer.id);
        new_reviewer->setExpertise(reviewer.primary_topic);
        new_reviewer->addEdge(terminal,data.max_reviews_per_reviewer);
        submissions[index] = new_reviewer;
        ++index;
        for(Vertex<int>* submission : submissions){
            if(submission->getExpertise() == reviewer.primary_topic){
                submission->addEdge(new_reviewer,1);
            }
        }
        
    }
}

template <class T>
bool dfsFindAugmentingPath(Vertex<T> *v, Vertex<T> *t, double &flow)
{
    v->setVisited(true);
    if (v == t)
    {
        return true;
    }
    for (auto edge : v->getAdj())
    {
        if (!edge->getDest()->isVisited() && (edge->getWeight() - edge->getFlow()) > 0)
        {
            flow = std::min(flow, edge->getWeight() - edge->getFlow());
            if (dfsFindAugmentingPath(edge->getDest(), t, flow))
            {
                edge->setFlow(edge->getFlow() + flow);
                return true;
            }
        }
    }
    for (auto edge : v->getIncoming())
    {
        if (!edge->getOrig()->isVisited() && edge->getFlow() > 0)
        {
            flow = std::min(flow, edge->getFlow());
            if (dfsFindAugmentingPath(edge->getOrig(), t, flow))
            {
                edge->setFlow(edge->getFlow() - flow);
                return true;
            }
        }
    }
    return false;
}

// Ford-Fulkerson algorithm
template <class T>
void fordFulkerson(Graph<T> *g, int source, int target)
{
    Vertex<T> *s = g->findVertex(source);
    Vertex<T> *t = g->findVertex(target);
    if (s == t || s == nullptr || t == nullptr)
    {
        return;
    }

    for (auto v : g->getVertexSet())
    {
        v->setVisited(false);
        for (auto e : v->getAdj())
        {
            e->setFlow(0);
        }
    }
    double f = INF;
    double max = 0;
    while (dfsFindAugmentingPath(s, t, f))
    {
        max += f;
        for (auto v : g->getVertexSet())
        {
            v->setVisited(false);
        }
    }
}

// /// TESTS ///
// #include <gtest/gtest.h>

// TEST(TP3_Ex6, test_fordFulkerson)
// {
//     Graph<int> myGraph;

//     for (int i = 1; i <= 6; i++)
//         myGraph.addVertex(i);

//     myGraph.addEdge(1, 2, 3);
//     myGraph.addEdge(1, 3, 2);
//     myGraph.addEdge(2, 5, 4);
//     myGraph.addEdge(2, 4, 3);
//     myGraph.addEdge(2, 3, 1);
//     myGraph.addEdge(3, 5, 2);
//     myGraph.addEdge(4, 6, 2);
//     myGraph.addEdge(5, 6, 3);

//     fordFulkerson(&myGraph, 1, 6);

//     std::stringstream ss;
//     for (auto v : myGraph.getVertexSet())
//     {
//         ss << v->getInfo() << "-> (";
//         for (const auto e : v->getAdj())
//             ss << (e->getDest())->getInfo() << "[Flow: " << e->getFlow() << "] ";
//         ss << ") || ";
//     }

//     std::cout << ss.str() << std::endl
//               << std::endl;

//     EXPECT_EQ("1-> (2[Flow: 3] 3[Flow: 2] ) || 2-> (5[Flow: 1] 4[Flow: 2] 3[Flow: 0] ) || 3-> (5[Flow: 2] ) || 4-> (6[Flow: 2] ) || 5-> (6[Flow: 3] ) || 6-> () || ", ss.str());
// }
