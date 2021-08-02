#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <iostream>

// This is the ITERATOR TESTING file.

TEST_CASE("BASIC TEST FOR ITERATION") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	// insert edges into the graph
	CHECK(g.insert_edge(1, 4, "a"));
	CHECK(g.insert_edge(1, 4, "b"));
	CHECK(g.insert_edge(1, 4, "c"));
	CHECK(g.insert_edge(4, 8, "d"));
	CHECK(g.insert_edge(1, 8, "e"));
	CHECK(g.insert_edge(5, 4, "f"));
	// Basic test to see if the find iterator function works.
    auto iter = g.find(1,4,"c");
    auto s = *iter;
    CHECK(s.from == 1);
    CHECK(s.to == 4);
    CHECK(s.weight == "c");

    // try to see if iter++ works:
    iter++;
    s = *iter;
    CHECK(s.from == 1);
    CHECK(s.to == 8);
    CHECK(s.weight == "e");

    //Check if iter-- works:
    iter--;
    iter--;
    s = *iter;
    CHECK(s.from == 1);
    CHECK(s.to == 4);
    CHECK(s.weight == "b");
}

TEST_CASE("HOW THE ITERATOR WORKS IN EDGE CONDITIONS?") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	// insert edges into the graph
	CHECK(g.insert_edge(1, 4, "a"));
	CHECK(g.insert_edge(1, 4, "b"));
	CHECK(g.insert_edge(1, 4, "c"));
	CHECK(g.insert_edge(4, 8, "d"));
	CHECK(g.insert_edge(1, 8, "e"));
	CHECK(g.insert_edge(5, 4, "f"));
}