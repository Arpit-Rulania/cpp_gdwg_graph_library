#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <iostream>

TEST_CASE("basic test") {
	auto g = gdwg::graph<int, std::string>{};
	auto n = 5;
	g.insert_node(n);
	CHECK(g.is_node(n));
	//std::cout << g.find(n);
}
