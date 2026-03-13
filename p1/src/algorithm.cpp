#include "Graph.h"
#include "parser.h"
#include <limits>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

static const int SOURCE = 0;
static const int TERMINAL = 1;
static const int SUBMISSIONS_OFFSET = 2;

struct Assignment
{
    int submissionId;
    int reviewerId;
    int topic;
};

struct UndercoveredSubmission
{
    int submissionId;
    int topic;
    int missingReviews;
};

template <class T>
bool dfsFindAugmentingPath(Vertex<T> *v, Vertex<T> *t, double &flow)
{
    v->setVisited(true);
    if (v == t)
        return true;

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

template <class T>
void fordFulkerson(Graph<T> *g, Vertex<T> *s, Vertex<T> *t)
{
    if (s == nullptr || t == nullptr || s == t)
        return;

    for (auto v : g->getVertexSet())
    {
        v->setVisited(false);
        for (auto e : v->getAdj())
            e->setFlow(0);
    }

    double f = INF;
    while (dfsFindAugmentingPath(s, t, f))
    {
        f = INF;
        for (auto v : g->getVertexSet())
            v->setVisited(false);
    }
}

void buildGraph(Graph<int> &graph,
                const std::vector<Topic> &submissions_topic,
                const std::vector<Topic> &reviewers_topic,
                const Parameters &data,
                std::vector<Vertex<int> *> &submissions,
                std::vector<Vertex<int> *> &reviewers,
                int skipReviewerIndex = -1)
{
    const int REVIEWERS_OFFSET = SUBMISSIONS_OFFSET + (int)submissions_topic.size();

    graph.addVertex(SOURCE);
    graph.addVertex(TERMINAL);

    Vertex<int> *source = graph.findVertex(SOURCE);
    Vertex<int> *terminal = graph.findVertex(TERMINAL);

    submissions.resize(submissions_topic.size());
    for (int i = 0; i < (int)submissions_topic.size(); ++i)
    {
        int index = SUBMISSIONS_OFFSET + i;
        graph.addVertex(index);
        Vertex<int> *v = graph.findVertex(index);
        v->setId(submissions_topic[i].id);
        v->setExpertise(submissions_topic[i].primary_topic);
        source->addEdge(v, data.min_reviews_per_submission);
        submissions[i] = v;
    }

    for (int i = 0; i < (int)reviewers_topic.size(); ++i)
    {
        if (i == skipReviewerIndex)
        {
            continue;
        }

        int index = REVIEWERS_OFFSET + i;
        graph.addVertex(index);
        Vertex<int> *v = graph.findVertex(index);
        v->setId(reviewers_topic[i].id);
        v->setExpertise(reviewers_topic[i].primary_topic);
        v->addEdge(terminal, data.max_reviews_per_reviewer);

        for (Vertex<int> *sub : submissions)
            if (sub->getExpertise() == v->getExpertise())
                sub->addEdge(v, 1);
    }
}

std::vector<Assignment> collectAssignments(const std::vector<Vertex<int> *> &submissions)
{
    std::vector<Assignment> assignments;
    for (Vertex<int> *sub : submissions)
        for (auto edge : sub->getAdj())
            if (edge->getFlow() == 1.0)
                assignments.push_back({sub->getId(), edge->getDest()->getId(), sub->getExpertise()});
    return assignments;
}

std::vector<UndercoveredSubmission> collectUndercovered(const std::vector<Vertex<int> *> &submissions)
{
    std::vector<UndercoveredSubmission> undercovered;
    for (Vertex<int> *sub : submissions)
    {
        for (auto edge : sub->getIncoming())
        {
            int missing = (int)(edge->getWeight() - edge->getFlow());
            if (missing > 0)
                undercovered.push_back({sub->getId(), sub->getExpertise(), missing});
        }
    }
    return undercovered;
}

bool isRisky(const std::vector<Topic> &submissions_topic,
             const std::vector<Topic> &reviewers_topic,
             const Parameters &data,
             int skipIndex,
             const std::vector<UndercoveredSubmission> &baselineUndercovered)
{
    Graph<int> g;
    std::vector<Vertex<int> *> submissions, reviewers;
    buildGraph(g, submissions_topic, reviewers_topic, data, submissions, reviewers, skipIndex);
    fordFulkerson(&g, g.findVertex(SOURCE), g.findVertex(TERMINAL));

    return collectUndercovered(submissions).size() > baselineUndercovered.size();
}

std::vector<int> collectRiskyReviewers(const std::vector<Topic> &submissions_topic,
                                       const std::vector<Topic> &reviewers_topic,
                                       const Parameters &data,
                                       const std::vector<UndercoveredSubmission> &baselineUndercovered)
{
    std::vector<int> risky;
    for (int i = 0; i < (int)reviewers_topic.size(); ++i)
        if (isRisky(submissions_topic, reviewers_topic, data, i, baselineUndercovered))
            risky.push_back(reviewers_topic[i].id);

    std::sort(risky.begin(), risky.end());
    return risky;
}

void writeAssignmentsBySubmission(std::ofstream &out, std::vector<Assignment> assignments)
{
    std::sort(assignments.begin(), assignments.end(), [](const Assignment &a, const Assignment &b)
              { return a.submissionId < b.submissionId; });

    out << "#SubmissionId,ReviewerId,Match\n";
    for (const auto &a : assignments)
        out << a.submissionId << ", " << a.reviewerId << ", " << a.topic << "\n";
}

void writeAssignmentsByReviewer(std::ofstream &out, std::vector<Assignment> assignments)
{
    std::sort(assignments.begin(), assignments.end(), [](const Assignment &a, const Assignment &b)
              { return a.reviewerId < b.reviewerId; });

    out << "#ReviewerId,SubmissionId,Match\n";
    for (const auto &a : assignments)
        out << a.reviewerId << ", " << a.submissionId << ", " << a.topic << "\n";
}

void writeUndercovered(std::ofstream &out, std::vector<UndercoveredSubmission> undercovered)
{
    std::sort(undercovered.begin(), undercovered.end(), [](const UndercoveredSubmission &a, const UndercoveredSubmission &b)
              { return a.submissionId < b.submissionId; });

    out << "#SubmissionId,Domain,MissingReviews\n";
    for (const auto &u : undercovered)
        out << u.submissionId << ", " << u.topic << ", " << u.missingReviews << "\n";
}

void writeRiskyReviewers(std::ofstream &out, const std::vector<int> &risky, int M)
{
    out << "#Risk Analysis: " << M << "\n";
    for (int i = 0; i < (int)risky.size(); ++i)
    {
        if (i > 0)
            out << ", ";
        out << risky[i];
    }
    out << "\n";
}

void generateAssignments(std::vector<Topic> &submissions_topic,
                         std::vector<Topic> &reviewers_topic,
                         Parameters data)
{
    Graph<int> my_graph;
    std::vector<Vertex<int> *> submissions, reviewers;
    buildGraph(my_graph, submissions_topic, reviewers_topic, data, submissions, reviewers);
    fordFulkerson(&my_graph, my_graph.findVertex(SOURCE), my_graph.findVertex(TERMINAL));

    auto assignments = collectAssignments(submissions);
    auto baselineUndercovered = collectUndercovered(submissions);

    std::ofstream out(data.output_file_name);
    if (!out.is_open())
    {
        std::cerr << "Could not open output file: " << data.output_file_name << "\n";
        return;
    }
    writeAssignmentsBySubmission(out, assignments);
    writeAssignmentsByReviewer(out, assignments);
    out << "#Total: " << assignments.size() << "\n";
    writeUndercovered(out, baselineUndercovered);

    if (data.risk_analysis > 0)
    {
        std::vector<int> risky = collectRiskyReviewers(submissions_topic, reviewers_topic, data, baselineUndercovered);
        writeRiskyReviewers(out, risky, data.risk_analysis);
    }

    out.close();
}