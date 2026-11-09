#pragma once
#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>
#include "graph.hpp"


inline std::vector<int> a_star(const Graph& g, int start, int end, std::function<int(int, int)> heuristic = [](int, int) {return 0;}, int* nodes_expanded = nullptr) {
    int n = g.adjacent_nodes.size();
    if (start < 0 || start >= n || end < 0 || end >= n) return {};

    const int INF = std::numeric_limits<int>::max();
    std::vector<int> g_score(n, INF);
    std::vector<int> parent(n, -1);

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> open;

    g_score[start] = 0;
    open.push({heuristic(start, end), start});
    
    while (!open.empty()) {
        auto [f, current] = open.top();
        open.pop();

        if (f > g_score[current] + heuristic(current, end)) continue;

        if (nodes_expanded) (*nodes_expanded)++;
        if (current == end) break;

        for (const auto& edge : g.adjacent_nodes[current]) {
            int neighbor = edge.first;
            int tentative_g = g_score[current] + edge.second;

            if (tentative_g < g_score[neighbor]) {
                g_score[neighbor] = tentative_g;
                parent[neighbor] = current;
                open.push({tentative_g + heuristic(neighbor, end), neighbor});
            }
        }
    }

    if (g_score[end] == INF) return {};

    std::vector<int> path;
    for (int node = end; node != -1; node = parent[node]) {
        path.push_back(node);
    }
    std::reverse(path.begin(), path.end());
    return path;
}