#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include "core/graph.hpp"
#include "core/bfs.hpp"
#include "core/Astar.hpp"


void print_path(const std::vector<int>& path) {
    if (path.empty()) {
        std::cout << "No path\n";
        return;
    }
    for (int node : path) std::cout << node << " ";
    std::cout << std::endl;
}

int main() {
    Graph g;
    for (int i = 0; i < 6; i++) g.add_node();

    g.add_connection(0, 1);
    g.add_connection(0, 2);
    g.add_connection(1, 3);
    g.add_connection(2, 4);
    g.add_connection(3, 5);
    g.add_connection(4, 5);

    g.print();

    std::vector<std::pair<int, int>> pos = {{0, 0}, {1, 0}, {0, 1}, {2, 0}, {1, 1}, {2, 1}};

    auto manhattan = [&pos](int a, int b) {
        return std::abs(pos[a].first - pos[b].first) + std::abs(pos[a].second - pos[b].second);
    };

    int bfs_count = 0, astar_count = 0;
    std::vector<int> bfs_path = bfs(g, 0, 5, &bfs_count);
    std::vector<int> astar_path = a_star(g, 0, 5, manhattan, &astar_count);

    std::cout << "BFS path: ";
    print_path(bfs_path);
    std::cout << "A* path: ";
    print_path(astar_path);

    std::cout << "BFS expanded: " << bfs_count << " nodes\n";
    std::cout << "A* expanded: " << astar_count << " nodes\n";

    assert(bfs_path.size() == astar_path.size());

    std::cout << "No-path case (BFS): ";
    g.add_node();
    print_path(bfs(g, 0, 6));
    std::cout << "No-path case (A*): ";
    print_path(a_star(g, 0, 6));

    return 0;
}