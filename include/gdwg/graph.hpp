#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <utility>

// TODO: Make this graph generic
//       ... this won't just compile
//       straight away
namespace gdwg {
	template<typename N, typename E>
	class graph {
	public:
		/***************************************
		**                                    **
		**       Transparent Comparators      **
		**                                    **
		***************************************/
		using edgePair = std::pair<std::weak_ptr<N>, E>;

		struct mapComparator {
			using is_transparent = void;
			// this overload will sort the source nodes of the map in increasing order.
			bool operator()(std::shared_ptr<N> const& a, std::shared_ptr<N> const& b) const {
				return *a < *b;
			}
			// this overload is used when the map.find(something) is called.
			// comparator made for both lhs and rhs cases.
			auto operator()(std::shared_ptr<N> const& a, N const& b) const noexcept -> bool {
				return *a < b;
			}
			auto operator()(N const& a, std::shared_ptr<N> const& b) const noexcept -> bool {
				return a < *b;
			}
		};

		struct setComparator {
			// using is_transparent = void;
			// this overload will sort the destination node(edges) in increasing order.
			// if the destination node is the same then it is sorted by weight "E"
			bool operator()(edgePair const& a, edgePair const& b) const {
				return ((*((a.first).lock()) < *((b.first).lock()))
				        || ((*((a.first).lock()) == *((b.first).lock())) && (a.second < b.second)));
			}
		};

		// type defining the destination node to avoid redundency.
		using destination_node = std::set<std::pair<std::weak_ptr<N>, E>, setComparator>;

		/***************************************
		**                                    **
		**          Custom Iterator           **
		**                                    **
		***************************************/
		struct value_type {
			N from;
			N to;
			E weight;
		};

		class iterator {
		public:
			/*using value_type = graph<N, E>::value_type;
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;*/

		private:
			typename std::map<std::shared_ptr<N>, destination_node, mapComparator>::const_iterator outer_iterator;
			typename destination_node::const_iterator inner_iterator;
			friend class graph;
		};

		/***************************************
		**                                    **
		**           constructors             **
		**                                    **
		***************************************/

		// Default constructor to create an empty map.
		graph() noexcept = default;

		// Create a graph using nodes from an initialiser list.
		graph(std::initializer_list<N> il) noexcept {
			for (auto i = il.begin(); i != il.end(); ++i) {
				insert_node(*i);
			}
		}

		// Creating a graph given all the start and end iter of an abject.
		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			for (auto i = first; i != last; ++i) {
				insert_node(*i);
			}
		}

		// Move constructor to create the graph.
		graph(graph&& other) noexcept = default;

		// Move operator to move-assign all the nodes of an old graph.
		auto operator=(graph&& other) noexcept -> graph& = default;
		// Graph<N, E>& operator=(Graph<N, E>&& old) noexcept = default;

		// Copy constructor to copy the whole old graph.
		graph(graph const& other) {
			// First we copy the shared pointer to the source(outer) node.
			// Note: We copy pointer to already existing memory because we dont modify the memory
			// anywhere.
			for (auto i = other.graph_.begin(); i != other.graph_.end(); ++i) {
				auto sptr = i->first;
				auto exist = graph_.find(*sptr);
				if (exist == graph_.end()) {
					graph_[sptr] = destination_node();
				}
			}
			// Next we copy all the destination(edges) nodes.
			// Outer loop to iterate over the map.
			for (auto i = other.graph_.begin(); i != other.graph_.end(); ++i) {
				// Inner loop to iterate over the set.
				if (i->second.size() == 0) {
					continue;
				}
				for (auto j = i->second.begin(); j != i->second.end(); ++j) {
					// auto node = graph_.find(*(j.first));
					auto p1 = std::get<0>(*j).lock();
					auto node = graph_.find(p1);
					// Create a weak copy of the shared ptr.
					std::weak_ptr<N> weak1 = node->first;
					// Create a pair of weak ptr,E to insert into the set.
					std::pair<std::weak_ptr<N>, E> edge1(weak1, std::get<1>(*j));
					graph_[node->first].insert(edge1);
				}
			}
		}

		/***************************************
		**                                    **
		**            modifiers               **
		**                                    **
		***************************************/

		// This function iserts a source node.
		auto insert_node(N const& value) -> bool {
			auto exist = graph_.find(value);
			if (exist == graph_.end()) {
				graph_[std::make_shared<N>(value)] = destination_node();
				return true;
			}
			return false;
		}

		/***************************************
		**                                    **
		**            Accessors               **
		**                                    **
		***************************************/

		[[nodiscard]] auto is_node(N const& value) -> bool {
			return (graph_.find(value) != graph_.end());
		}

		[[nodiscard]] auto empty() -> bool {
			return graph_.empty();
		}

		// Function that returns total number of nodes for testing purposes.
		[[nodiscard]] auto size() -> int {
			return static_cast<int>(graph_.size());
		}

	private:
		std::map<std::shared_ptr<N>, destination_node, mapComparator> graph_;
	};
} // namespace gdwg

#endif // GDWG_GRAPH_HPP