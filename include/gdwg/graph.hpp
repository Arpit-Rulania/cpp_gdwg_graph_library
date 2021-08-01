#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP
#include <memory>
#include <map>
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
            // here something is templated as the user might do anything.
            // comparator made for both lhs and rhs cases.
            template<typename t>
            auto operator()(std::shared_ptr<N> const& a, t const& b) const noexcept -> bool {
                return *a < b;
            }
            template<typename t>
            auto operator()(t const& a, std::shared_ptr<N> const& b) const noexcept -> bool {
                return a < *b;
            }
        };

        struct setComparator {
            //using is_transparent = void;
            // this overload will sort the destination node(edges) in increasing order.
            // if the destination node is the same then it is sorted by weight "E"
            bool operator()(edgePair const& a, edgePair const& b) const {
                return ((*(a.first) < *(b.first)) || ((*(a.first) == *(b.first)) && (a.second < b.second)));
            }
        };

		// type defining the destination node to avoid redundency.
		using destination_node = std::set<std::pair<std::weak_ptr<N>, E>, setComparator>;

		/***************************************
		**                                    **
		**          Custom Iterator           **
		**                                    **
		***************************************/

		class iterator {
			public:

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
		graph() = default;

		// Create a graph using nodes from an initialiser list.
		graph(std::initializer_list<N> il) {
			for(auto i = il.begin(); i != il.end(); ++i) {
				insert_node(*i);
			}
		}

		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			for(auto i = first; i != last; ++i) {
				insert_node(*i);
			}
		}

		graph(graph&& other) noexcept = default;    //prefered method.
		/*graph(graph&& other) noexcept {
			graph_ = std::exchange(other.graph_, 0u);   //not sure if this would work

			for(auto i = other.graph_.begin(); i != other.graph_.end(); ++i) {
				insert_node(*i->first); //should it be * or **
			}
			for(auto i = other.graph_.begin(); i != other.graph_.end(); ++i) {
				for(auto j = i->second.begin(); j != i->second.end(); ++j) {
					auto node = graph_.find(*(j.first));
					std::weak_ptr<N> weak1 = node;  //is this even correct?
					std::pair <std::weak_ptr<N>, E> edge1 (weak1,j.second);
					graph_[node].insert(edge1);
				}
			}
			other.graph_.clear();
		}*/


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

		struct value_type {
			N from;
			N to;
			E weight;
		};

		/***************************************
		**                                    **
		**            Accessors               **
		**                                    **
		***************************************/

		[[nodiscard]] auto is_node(N const& value) -> bool {
			return (graph_.find(value) != graph_.end());
		}


	private:
		std::map<std::shared_ptr<N>, destination_node, mapComparator> graph_;
		//std::map<std::shared_ptr<N>, std::set<std::pair<std::shared_ptr<N>, E>>, mapComparator> graph_;
		//std::map<std::shared_ptr<N>, std::set<std::shared_ptr<std::pair<std::shared_ptr<N>, E>>>, mapComparator> graph_;
	};
} // namespace gdwg

#endif // GDWG_GRAPH_HPP

// do we need to use custom iterator for modifiers etc
// should we use custom iterators?
