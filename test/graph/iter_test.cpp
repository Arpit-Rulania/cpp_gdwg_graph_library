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
	auto iter = g.find(1, 4, "c");
	CHECK((*iter).from == 1);
	CHECK((*iter).to == 4);
	CHECK((*iter).weight == "c");

	// try to see if iter++ works:
	iter++;
	CHECK((*iter).from == 1);
	CHECK((*iter).to == 8);
	CHECK((*iter).weight == "e");

	// Check if iter-- works:
	iter--;
	iter--;
	CHECK((*iter).from == 1);
	CHECK((*iter).to == 4);
	CHECK((*iter).weight == "b");
}

TEST_CASE("HOW THE ITERATOR WORKS IN EDGE CONDITIONS?") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	// insert edges into the graph
	CHECK(g.insert_edge(1, 4, "a"));
	CHECK(g.insert_edge(1, 4, "b"));
	CHECK(g.insert_edge(1, 4, "c"));
	CHECK(g.insert_edge(4, 8, "d"));
	// LETS TEST THE BEGIN ITERATOR
	auto iter = g.begin();
	CHECK((*iter).from == 1);
	CHECK((*iter).to == 4);
	CHECK((*iter).weight == "a");
	iter++;
	iter++;
	CHECK((*iter).weight == "c");
	iter++;
	iter++;
	CHECK(iter == g.end());
	// std::cout << (*iter).from << " **** " << (*iter).to << " **** " << '\n';
}

TEST_CASE("erase_edge(iterator) TEST") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	// insert edges into the graph
	CHECK(g.insert_edge(1, 4, "a"));
	CHECK(g.insert_edge(1, 4, "b"));
	CHECK(g.insert_edge(1, 4, "c"));
	CHECK(g.insert_edge(4, 8, "d"));
	CHECK(g.insert_edge(5, 4, "b"));
	CHECK(g.insert_edge(4, 4, "c"));
	CHECK(g.insert_edge(7, 8, "d"));
	auto a = g.find(1, 4, "b");
	auto b = g.find(5, 4, "b");
	auto next = g.erase_edge(a, b);
	// Erasing the set of edges but not b.
	CHECK((*next).from == 5);
	CHECK((*next).to == 4);
	CHECK((*next).weight == "b");
}