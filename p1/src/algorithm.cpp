#include "Graph.h"
#include "parser.h"
#include <limits>
#include <iostream>
#include <fstream>

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
            v->setVisited(false);
    }
}

void generateAssignments(std::vector<Topic> &submissions_topic,
                         std::vector<Topic> &reviewers_topic,
                         Parameters data)
{
    Graph<int> my_graph;
    const int SOURCE = 0;
    const int TERMINAL = 1;
    const int SUBMISSIONS_OFFSET = 2;
    const int REVIEWERS_OFFSET = 2 + (int)submissions_topic.size();

    my_graph.addVertex(SOURCE);
    my_graph.addVertex(TERMINAL);

    Vertex<int> *source = my_graph.findVertex(SOURCE);
    Vertex<int> *terminal = my_graph.findVertex(TERMINAL);

    // Submission vertices
    std::vector<Vertex<int> *> submissions(submissions_topic.size());
    for (int i = 0; i < (int)submissions_topic.size(); ++i)
    {
        int index = SUBMISSIONS_OFFSET + i;
        my_graph.addVertex(index);
        Vertex<int> *new_submission = my_graph.findVertex(index);
        new_submission->setId(submissions_topic[i].id);
        new_submission->setExpertise(submissions_topic[i].primary_topic);
        source->addEdge(new_submission, data.min_reviews_per_submission);
        submissions[i] = new_submission;
    }

    // Reviewer vertices
    std::vector<Vertex<int> *> reviewers(reviewers_topic.size());
    for (int i = 0; i < (int)reviewers_topic.size(); ++i)
    {
        int index = REVIEWERS_OFFSET + i;
        my_graph.addVertex(index);
        Vertex<int> *new_reviewer = my_graph.findVertex(index);
        new_reviewer->setId(reviewers_topic[i].id);
        new_reviewer->setExpertise(reviewers_topic[i].primary_topic);
        new_reviewer->addEdge(terminal, data.max_reviews_per_reviewer);
        reviewers[i] = new_reviewer;

        for (Vertex<int> *submission : submissions)
        {
            if (submission->getExpertise() == new_reviewer->getExpertise())
            {
                submission->addEdge(new_reviewer, 1);
            }
        }
    }

    fordFulkerson(&my_graph, source, terminal);

    // --- Extract and output assignments ---
    std::ofstream out(data.output_file_name);
    if (!out.is_open())
    {
        std::cerr << "Could not open output file: " << data.output_file_name << "\n";
        return;
    }

    // Collect assignments
    struct Assignment
    {
        int submissionId;
        int reviewerId;
        int topic;
    };

    std::vector<Assignment> assignments;
    for (Vertex<int> *sub : submissions)
        for (auto edge : sub->getAdj())
            if (edge->getFlow() == 1.0)
                assignments.push_back({sub->getId(), edge->getDest()->getId(), sub->getExpertise()});

    // Sort by SubmissionId for first section
    std::sort(assignments.begin(), assignments.end(), [](const Assignment &a, const Assignment &b)
              { return a.submissionId < b.submissionId; });

    out << "#SubmissionId,ReviewerId,Match\n";
    for (const auto &a : assignments)
        out << a.submissionId << ", " << a.reviewerId << ", " << a.topic << "\n";

    // Sort by ReviewerId for second section
    std::sort(assignments.begin(), assignments.end(), [](const Assignment &a, const Assignment &b)
              { return a.reviewerId < b.reviewerId; });

    out << "#ReviewerId,SubmissionId,Match\n";
    for (const auto &a : assignments)
        out << a.reviewerId << ", " << a.submissionId << ", " << a.topic << "\n";

    out << "#Total: " << assignments.size() << "\n";

    // Collect unmet submissions
    struct UnmetSubmission
    {
        int submissionId;
        int topic;
        int missingReviews;
    };

    std::vector<UnmetSubmission> unmet;
    for (Vertex<int> *sub : submissions)
    {
        int assigned = 0;
        for (auto edge : sub->getAdj())
            if (edge->getFlow() == 1.0)
                assigned++;

        int missing = data.min_reviews_per_submission - assigned;
        if (missing > 0)
            unmet.push_back({sub->getId(), sub->getExpertise(), missing});
    }

    // Sort by SubmissionId
    std::sort(unmet.begin(), unmet.end(), [](const UnmetSubmission &a, const UnmetSubmission &b)
              { return a.submissionId < b.submissionId; });

    out << "#SubmissionId,Domain,MissingReviews\n";
    for (const auto &u : unmet)
        out << u.submissionId << ", " << u.topic << ", " << u.missingReviews << "\n";
    out.close();
}
