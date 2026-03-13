#include "Graph.h"
#include "parser.h"
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

template <class T>
void fordFulkerson(Graph<T> *g, Vertex<T> *s, Vertex<T> *t)
{
    if (s == nullptr || t == nullptr || s == t)
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
    while (dfsFindAugmentingPath(s, t, f))
    {
        f = INF;
        for (auto v : g->getVertexSet())
        {
            v->setVisited(false);
        }
    }
}

void buildGraph(Graph<int> &graph,
                const std::vector<Topic> &submissions_topic,
                const std::vector<Topic> &reviewers_topic,
                const Parameters &data,
                std::vector<Vertex<int> *> &submissions,
                int skip_reviewer_index = -1)
{
    const int reviewers_offset = SUBMISSIONS_OFFSET + (int)submissions_topic.size();

    graph.addVertex(SOURCE);
    graph.addVertex(TERMINAL);

    Vertex<int> *source_vertex = graph.findVertex(SOURCE);
    Vertex<int> *terminal_vertex = graph.findVertex(TERMINAL);

    submissions.resize(submissions_topic.size());
    for (int submission_index = 0; submission_index < (int)submissions_topic.size(); ++submission_index)
    {
        int graph_index = SUBMISSIONS_OFFSET + submission_index;
        graph.addVertex(graph_index);
        Vertex<int> *sub_vertex = graph.findVertex(graph_index);
        sub_vertex->setId(submissions_topic[submission_index].id);
        sub_vertex->setExpertise(submissions_topic[submission_index].primary_topic);
        source_vertex->addEdge(sub_vertex, data.min_reviews_per_submission);
        submissions[submission_index] = sub_vertex;
    }

    for (int reviewer_index = 0; reviewer_index < (int)reviewers_topic.size(); ++reviewer_index)
    {
        if (reviewer_index == skip_reviewer_index)
        {
            continue;
        }

        int graph_index = reviewers_offset + reviewer_index;
        graph.addVertex(graph_index);
        Vertex<int> *rev_vertex = graph.findVertex(graph_index);
        rev_vertex->setId(reviewers_topic[reviewer_index].id);
        rev_vertex->setExpertise(reviewers_topic[reviewer_index].primary_topic);
        rev_vertex->addEdge(terminal_vertex, data.max_reviews_per_reviewer);

        for (Vertex<int> *sub_vertex : submissions)
        {
            if (sub_vertex->getExpertise() == rev_vertex->getExpertise())
            {
                sub_vertex->addEdge(rev_vertex, 1);
            }
        }
    }
}

std::vector<Assignment> collectAssignments(const std::vector<Vertex<int> *> &submissions)
{
    std::vector<Assignment> assignments;
    for (Vertex<int> *vertex : submissions)
    {
        for (auto edge : vertex->getAdj())
        {
            if (edge->getFlow() == 1.0)
            {
                assignments.push_back({vertex->getId(), edge->getDest()->getId(), vertex->getExpertise()});
            }
        }
    }
    return assignments;
}

std::vector<UndercoveredSubmission> collectUndercovered(const std::vector<Vertex<int> *> &submissions)
{
    std::vector<UndercoveredSubmission> undercovered;
    for (Vertex<int> *vertex : submissions)
    {
        for (auto edge : vertex->getIncoming())
        {
            int missing_reviews = (int)(edge->getWeight() - edge->getFlow());
            if (missing_reviews > 0)
            {
                undercovered.push_back({vertex->getId(), vertex->getExpertise(), missing_reviews});
            }
        }
    }
    return undercovered;
}

bool isRisky(const std::vector<Topic> &submissions_topic,
             const std::vector<Topic> &reviewers_topic,
             const Parameters &data,
             int skip_index,
             const std::vector<UndercoveredSubmission> &baseline_undercovered)
{
    Graph<int> trial_graph;
    std::vector<Vertex<int> *> trial_submissions;
    buildGraph(trial_graph, submissions_topic, reviewers_topic, data, trial_submissions, skip_index);
    fordFulkerson(&trial_graph, trial_graph.findVertex(SOURCE), trial_graph.findVertex(TERMINAL));
    return collectUndercovered(trial_submissions).size() > baseline_undercovered.size();
}

std::vector<int> collectRiskyReviewers(const std::vector<Topic> &submissions_topic,
                                       const std::vector<Topic> &reviewers_topic,
                                       const Parameters &data,
                                       const std::vector<UndercoveredSubmission> &baseline_undercovered)
{
    std::vector<int> risky_reviewers;
    for (int index = 0; index < (int)reviewers_topic.size(); ++index)
    {
        if (isRisky(submissions_topic, reviewers_topic, data, index, baseline_undercovered))
        {
            risky_reviewers.push_back(reviewers_topic[index].id);
        }
    }
    std::sort(risky_reviewers.begin(), risky_reviewers.end());
    return risky_reviewers;
}

void writeAssignmentsBySubmission(std::ofstream &output_file, std::vector<Assignment> assignments)
{
    std::sort(assignments.begin(), assignments.end(), [](const Assignment &lhs, const Assignment &rhs)
              { return lhs.submissionId < rhs.submissionId; });

    output_file << "#SubmissionId,ReviewerId,Match\n";
    for (const auto &assignment : assignments)
    {
        output_file << assignment.submissionId << ", " << assignment.reviewerId << ", " << assignment.topic << "\n";
    }
}

void writeAssignmentsByReviewer(std::ofstream &output_file, std::vector<Assignment> assignments)
{
    std::sort(assignments.begin(), assignments.end(), [](const Assignment &lhs, const Assignment &rhs)
              { return lhs.reviewerId < rhs.reviewerId; });

    output_file << "#ReviewerId,SubmissionId,Match\n";
    for (const auto &assignment : assignments)
    {
        output_file << assignment.reviewerId << ", " << assignment.submissionId << ", " << assignment.topic << "\n";
    }
}

void writeUndercovered(std::ofstream &output_file, std::vector<UndercoveredSubmission> undercovered)
{
    std::sort(undercovered.begin(), undercovered.end(), [](const UndercoveredSubmission &lhs, const UndercoveredSubmission &rhs)
              { return lhs.submissionId < rhs.submissionId; });

    output_file << "#SubmissionId,Domain,MissingReviews\n";
    for (const auto &sub_entry : undercovered)
    {
        output_file << sub_entry.submissionId << ", " << sub_entry.topic << ", " << sub_entry.missingReviews << "\n";
    }
}

void writeRiskyReviewers(std::ofstream &output_file, const std::vector<int> &risky_reviewers, int risk_param)
{
    output_file << "#Risk Analysis: " << risk_param << "\n";
    for (int index = 0; index < (int)risky_reviewers.size(); ++index)
    {
        if (index > 0)
        {
            output_file << ", ";
        }
        output_file << risky_reviewers[index];
    }
    output_file << "\n";
}

void generateAssignments(std::vector<Topic> &submissions_topic,
                         std::vector<Topic> &reviewers_topic,
                         Parameters data,
                         std::ofstream &risk_out)
{
    Graph<int> main_graph;
    std::vector<Vertex<int> *> submissions;
    buildGraph(main_graph, submissions_topic, reviewers_topic, data, submissions);
    fordFulkerson(&main_graph, main_graph.findVertex(SOURCE), main_graph.findVertex(TERMINAL));

    auto assignments = collectAssignments(submissions);
    auto baseline_undercovered = collectUndercovered(submissions);

    std::ofstream output_file(data.output_file_name);
    if (!output_file.is_open())
    {
        std::cerr << "Could not open output file: " << data.output_file_name << "\n";
        return;
    }

    writeAssignmentsBySubmission(output_file, assignments);
    writeAssignmentsByReviewer(output_file, assignments);
    output_file << "#Total: " << assignments.size() << "\n";
    writeUndercovered(output_file, baseline_undercovered);

    if (data.risk_analysis > 0)
    {
        auto risky_reviewers = collectRiskyReviewers(submissions_topic, reviewers_topic, data, baseline_undercovered);
        writeRiskyReviewers(output_file, risky_reviewers, data.risk_analysis);
        if (risk_out.is_open())
        {
            writeRiskyReviewers(risk_out, risky_reviewers, data.risk_analysis);
        }
    }

    output_file.close();
}