#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <iostream>

// This is the CONSTRUCTOR TESTING file.

TEST_CASE("basic test") {
	auto g = gdwg::graph<int, std::string>{};
	auto n = 5;
	g.insert_node(n);
	CHECK(!g.empty());
	CHECK(g.is_node(n));
}

TEST_CASE("test to see if empty constructor works") {
	auto g = gdwg::graph<int, std::string>();
	CHECK(g.empty());
}

TEST_CASE("test to see if initialiser list constructor works") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	CHECK(!g.empty());
	// here size is not 6 because one 1 is duplicate.
	CHECK(g.size() == 5);
	CHECK(g.is_node(1));
	CHECK(g.is_node(5));
	CHECK(!g.is_node(12));
}

TEST_CASE("test to see if move constructor works") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	auto h = gdwg::graph<int, std::string>(std::move(g));
	// because of move g should be empty and h should not.
	CHECK(g.empty());
	CHECK(!h.empty());
	// here size is not 6 because one 1 is duplicate.
	CHECK(h.size() == 5);
	CHECK(h.is_node(5));
	CHECK(!h.is_node(12));
}

TEST_CASE("test to see if move operator works") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	auto h = std::move(g);
	// because of move g should be empty and h should not.
	CHECK(g.empty());
	CHECK(!h.empty());
	// here size is not 6 because one 1 is duplicate.
	CHECK(h.size() == 5);
	CHECK(h.is_node(5));
	CHECK(!h.is_node(12));
}

TEST_CASE("test to see if copy constructor works") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	auto h = gdwg::graph<int, std::string>(g);
	// here size is not 6 because one 1 is duplicate.
	CHECK(g.size() == 5);
	CHECK(h.size() == 5);
	CHECK(h.is_node(5));
	CHECK(!h.is_node(12));
	CHECK(g.is_node(7));
	CHECK(!g.is_node(55));
}

TEST_CASE("test to see if copy assignment works") {
	auto g = gdwg::graph<int, std::string>{1, 5, 7, 1, 4, 8};
	auto h = g;
	// here size is not 6 because one 1 is duplicate.
	CHECK(g.size() == 5);
	CHECK(h.size() == 5);
	CHECK(h.is_node(5));
	CHECK(!h.is_node(12));
	CHECK(g.is_node(7));
	CHECK(!g.is_node(55));
}