#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <iostream>

// This is the ITERATOR TESTING file.

TEST_CASE("test to see insert_edge work") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	// insert edges into the graph
	CHECK(g.insert_edge(1, 4, "a"));
	CHECK(g.insert_edge(1, 4, "b"));
	CHECK(g.insert_edge(1, 4, "c"));
	CHECK(g.insert_edge(4, 8, "d"));
	CHECK(g.insert_edge(1, 8, "e"));
	CHECK(g.insert_edge(5, 4, "f"));
    auto iter = g.find(1,4,"c");
}