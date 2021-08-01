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
