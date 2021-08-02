#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <iostream>

// This is the MODIFIER TESTING file.

// In this test case we will be checking if insert edge works.
// We will also be testing all the different types of accessors to check if they work.
TEST_CASE("test to see insert_edge work") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	// Check if insert edge works and with duplicate.
	CHECK(g.insert_edge(1, 4, "apple"));
	CHECK(!g.insert_edge(1, 4, "apple"));
	CHECK(g.insert_edge(1, 4, "b-apple"));
	CHECK(g.insert_edge(1, 4, "c-apple"));
	CHECK(g.insert_edge(4, 8, "d-apple"));
	CHECK(g.insert_edge(1, 8, "e-apple"));
	CHECK(g.insert_edge(5, 4, "f-apple"));
	// Check if is_connected() function is operating correctly:
	CHECK(g.is_connected(1, 4));
	CHECK(!g.is_connected(1, 7));
	// Check if nodes() that returns all nodes works:
	auto v = g.nodes();
	CHECK(v[0] == 1);
	CHECK(v[3] == 7);
	CHECK(v.size() == 5);
	// Check if weights(src, dst) works:
	auto w = g.weights(1, 4);
	CHECK(w[0] == "apple");
	CHECK(w[1] == "b-apple");
	CHECK(w[2] == "c-apple");
	CHECK(w.size() == 3);
	// Check if connections(src) works:
	auto u = g.connections(1);
	CHECK(u[0] == 4);
	CHECK(u[1] == 8);
	CHECK(u.size() == 2);
}

TEST_CASE("Replace Node Test") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	// Check if insert edge works.
	CHECK(g.insert_edge(1, 4, "apple"));
	CHECK(g.insert_edge(1, 4, "b-apple"));
	CHECK(g.insert_edge(4, 8, "d-apple"));
	// 7 already exists so this should not work.
	CHECK(!g.replace_node(1, 7));
	// But 2 does not exist.
	CHECK(g.replace_node(1, 20));
	// now check if it has been replaces using accessor.
	/*auto w = g.weights(20, 4);
	CHECK(w[0] == "apple");
	CHECK(w[1] == "b-apple");*/
	auto v = g.nodes();
	CHECK(v[0] == 20);
	CHECK(v[3] == 7);
	CHECK(v.size() == 5);
}

TEST_CASE("Merge Replace Test") {
	auto g = gdwg::graph<std::string, int>{"A", "B", "C", "D", "E", "F"};
	CHECK(g.insert_edge("A", "B", 1));
	CHECK(g.insert_edge("A", "C", 2));
	CHECK(g.insert_edge("A", "D", 3));
	CHECK(g.insert_edge("B", "B", 1));
	CHECK(g.insert_edge("A", "A", 1));
	CHECK(g.insert_edge("D", "A", 6));
	// Perform merge replace:
	g.merge_replace_node("A", "B");
	// Check if the edges have been moved around correctly.
	CHECK(g.is_connected("B", "D"));
	CHECK(g.is_connected("B", "C"));
	CHECK(g.is_connected("D", "B"));
	CHECK(g.is_connected("B", "B"));
	// Check to see if A has been deleted.
	auto v = g.nodes();
	CHECK(v[0] == "B");
	CHECK(v[3] == "E");
	CHECK(v.size() == 5);
	// Check if the weights are correct
	auto w = g.weights("B", "D");
	CHECK(w[0] == 3);
	auto x = g.weights("D", "B");
	CHECK(x[0] == 6);
	auto y = g.weights("B", "B");
	CHECK(y[0] == 1);
}

TEST_CASE("erase node and erase edge test") {
	auto g = gdwg::graph<std::string, int>{"A", "B", "C", "D", "E", "F"};
	CHECK(g.insert_edge("A", "B", 1));
	CHECK(g.insert_edge("B", "F", 4));
	CHECK(g.insert_edge("A", "A", 1));
	CHECK(g.insert_edge("D", "A", 6));
	// right now a and b are connected:
	CHECK(g.is_connected("A", "B"));
	// delete the edge A->B:
	CHECK(g.erase_edge("A", "B", 1));  // TESTING ERASE EDGE.
	// now they should be seperate:
	CHECK(!g.is_connected("A", "B"));

	// now delete node A all toghter:  // TESTING ERASE NODE.
	CHECK(g.erase_node("A"));
	// Check total number of things left(=5)
	CHECK(g.size() == 5);
	auto v = g.connections("D");
	CHECK(v.size() == 0);

	// Now we test the function that deletes the whole map/graph.
	g.clear();
	CHECK(g.size() == 0);
}