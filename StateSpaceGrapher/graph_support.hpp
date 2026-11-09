#pragma once

#include "../state-spaces/ss.hpp"
#include "../core/graph.hpp"

#include <vector>
#include <queue>


Graph ss_to_graph(StateSpace ss) {
    Graph graph;
    for (int i = 0; i < ss.NUM_NODES; i++) graph.add_node();
    for (auto connection : ss.edges) graph.add_connection(connection.first, connection.second ,1);
    return graph;
}



std::vector<int> dijkstra_shortest_path(const Graph& graph, int start, int goal) {
    std::vector<bool> visited(graph.size());
    std::vector<int> costs(graph.size(), -1);
    std::vector<int> bearing(graph.size(), -1);
    using PII = std::pair<int, int>;
    std::priority_queue<PII, std::vector<PII>, std::greater<PII>> nodeheap;

    costs[start] = 0;
    nodeheap.push({0, start});

    bool found = false;
    while (!nodeheap.empty()) {
        auto [cost, ego_node] = nodeheap.top();
        nodeheap.pop();
        if (cost > costs[ego_node]) continue;

        if (ego_node == goal) {
            found = true;
            break;
        }

        for (auto [other_node, dist] : graph.adjacent_nodes[ego_node]) {
            if (visited[other_node]) continue;
            int new_cost = cost + dist;
            if (costs[other_node] == -1 || new_cost < costs[other_node]) {
                costs[other_node] = new_cost;
                bearing[other_node] = ego_node;
                nodeheap.push({new_cost, other_node});
            }
            visited[ego_node] = true;
        }
    }

    if (!found) return {};
    std::vector<int> traversal = {goal};
    while (traversal.back() != start) traversal.push_back(bearing[traversal.back()]);
    std::vector<int> path;
    for (int i = traversal.size()-1; i >=0; i--) path.push_back(traversal[i]);
    return path;
}
