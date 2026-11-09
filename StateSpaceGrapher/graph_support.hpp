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


