#pragma once
#include <algorithm>
#include <queue>
#include <vector>
#include "graph.hpp"


inline std::vector<int> bfs(const Graph& g, int start, int end, int* nodes_expanded = nullptr) {
    int n = g.adjacent_nodes.size();
    if (start < 0 || start >= n || end < 0 || end >= n) return {};

    std::vector<int> parent(n, -1);
    std::vector<bool> visited(n, false);
    std::queue<int> q;

    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        if (nodes_expanded) (*nodes_expanded)++;
        if (current == end) break;

        for (const auto& edge : g.adjacent_nodes[current]) {
            int neighbor = edge.first;
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                parent[neighbor] = current;
                q.push(neighbor);
            }
        }
    }

    if (!visited[end]) return {};

    std::vector<int> path;
    for (int node = end; node != -1; node = parent[node]) {
        path.push_back(node);
    }
    std::reverse(path.begin(), path.end());
    return path;
}