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

		struct mapCompare {
			using is_transparent = void;
            bool operator()(std::shared_ptr<N> const& a, std::shared_ptr<N> const& b) const {
                return *a < *b;
            }
            template<typename t>
            auto operator()(std::shared_ptr<N> const& a, t const& b) const noexcept -> bool {
                return *a < b;
            }
            template<typename t>
            auto operator()(t const& a, std::shared_ptr<N> const& b) const noexcept -> bool {
                return a < *b;
            }
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
				insert_node(*i);    // is i(first) in this case value or pointer? do we need to dereference it?
			}
		}

		//  CONFUSED!
		graph(graph&& other) noexcept = default;    //prefered method.
		graph(graph&& other) noexcept {
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
		        graph_[std::make_shared<N>(value)] = std::set<std::pair<std::weak_ptr<N>, E>>();
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
		std::map<std::shared_ptr<N>, std::set<std::pair<std::weak_ptr<N>, E>>, mapCompare> graph_;
		//std::map<std::shared_ptr<N>, std::set<std::pair<std::shared_ptr<N>, E>>, mapCompare> graph_;
		//std::map<std::shared_ptr<N>, std::set<std::shared_ptr<std::pair<std::shared_ptr<N>, E>>>, mapCompare> graph_;
	};
} // namespace gdwg

#endif // GDWG_GRAPH_HPP

// do we need to use custom iterator for modifiers etc
// should we use custom iterators?
