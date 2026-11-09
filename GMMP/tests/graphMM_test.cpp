#include "GraphMM.hpp" // Assuming your header is named this
#include <cassert>
#include <iostream>

void test_node_operations() {
    GraphMM graph;

    // Test 1: Basic Addition
    assert(graph.addNode("Work") == true);
    assert(graph.nodeExists("Work") == true);

    // Test 2: Case-Insensitive Duplicate Prevention
    assert(graph.addNode("WORK") == false); 
    assert(graph.addNode("work") == false);

    // Test 3: Removal
    assert(graph.removeNode("Work") == true);
    assert(graph.nodeExists("work") == false);
    assert(graph.removeNode("NonExistent") == false);

    std::cout << "[PASS] Node Operations\n";
}

void test_connection_logic() {
    GraphMM graph;
    graph.addNode("Alpha");
    graph.addNode("Beta");
    graph.addNode("Gamma");

    // Test 4: Basic Connection
    assert(graph.addConnection("Alpha", "Beta") == true);

    // Test 5: Redundant Connection (Symmetry)
    // Connecting Beta->Alpha should fail because Alpha->Beta exists
    assert(graph.addConnection("Beta", "Alpha") == false);

    // Test 6: Case-Insensitive Connection Redundancy
    assert(graph.addConnection("ALPHA", "beta") == false);

    // Test 7: Self-Connection Prevention
    assert(graph.addConnection("Gamma", "Gamma") == false);
    assert(graph.addConnection("Gamma", "gamma") == false);

    // Test 8: Non-existent Node Connection
    assert(graph.addConnection("Alpha", "Delta") == false);

    std::cout << "[PASS] Connection Logic\n";
}

void test_cascade_deletion() {
    GraphMM graph;
    graph.addNode("A");
    graph.addNode("B");
    graph.addNode("C");

    graph.addConnection("A", "B");
    graph.addConnection("A", "C");
    
    // Verify internal state (2 connections)
    assert(graph.connections.size() == 2);

    // Test 9: Remove Node A, should wipe both connections
    graph.removeNode("A");
    assert(graph.connections.size() == 0);

    // Test 10: Ensure Node B and C still exist but are isolated
    assert(graph.nodeExists("B") == true);
    assert(graph.nodeExists("C") == true);

    std::cout << "[PASS] Cascade Deletion\n";
}

int main() {
    try {
        test_node_operations();
        test_connection_logic();
        test_cascade_deletion();
        
        std::cout << "\nALL TESTS PASSED RIGOROUSLY\n";
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}