#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP
#include <algorithm>
#include <iostream>
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
			using is_transparent = void;
			// This overload will sort the destination node(edges) in increasing order.
			// If the destination node is the same then it is sorted by weight "E"
			bool operator()(edgePair const& a, edgePair const& b) const {
				return ((*((a.first).lock()) < *((b.first).lock()))
				        || ((*((a.first).lock()) == *((b.first).lock())) && (a.second < b.second)));
			}
			// This overload is used when set.find() is used.
			// Comparator made for both lhs and rhs cases.
			auto operator()(edgePair const& a, N const& b) const noexcept -> bool {
				return *((a.first).lock()) < b;
			}
			auto operator()(N const& a, edgePair const& b) const noexcept -> bool {
				return a < *((b.first).lock());
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
			using outer_iterator =
			   typename std::map<std::shared_ptr<N>, destination_node, mapComparator>::const_iterator;
			using inner_iterator = typename destination_node::const_iterator;

		public:
			using value_type = graph<N, E>::value_type;
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			iterator(const outer_iterator& curr, const outer_iterator& end, const inner_iterator& pos)
			: curr_{curr}
			, end_{end}
			, pos_{pos} {};

			auto operator*() noexcept -> reference {
				value_type v;
				v.from = *(curr_->first);
				v.to = *(pos_->first.lock());
				v.weight = (pos_->second);
				return v;
			}

			// pre increment
			auto operator++() noexcept -> iterator& {
				if (curr_ != end_) {
					pos_++;
					if (pos_ == ((curr_->second).end())) {
						curr_++;
						while (curr_ != end_) {
							if (!(curr_->second).empty()) {
								pos_ = (curr_->second).begin();
								return *this;
							}
							curr_++;
						}
					}
				}
				return *this;
			}

			// post increment... strapped to pre increment.
			auto operator++(int) noexcept -> iterator {
				auto temp = *this;
				++(*this);
				return temp;
			}

			// pre decrement
			auto operator--() noexcept -> iterator& {
				if (curr_ == end_) {
					--curr_;
					while ((curr_->second).empty()) {
						--curr_;
					}
					pos_ = (curr_->second).cend();
					pos_--;
					return *this;
				}
				while (pos_ == (curr_->second).cbegin()) {
					curr_--;
					pos_ = (curr_->second).cend();
				}
				--pos_;
				return *this;
			}

			// post decrement
			auto operator--(int) noexcept -> iterator {
				auto temp = *this;
				--(*this);
				return temp;
			}

			// Iterator comparison
			auto operator==(iterator const& other) -> bool {
				if (other.curr_ == other.end_ || curr_ == end_) {
					return (other.curr_ == curr_);
				}
				else {
					return ((other.pos_ == pos_) && (other.curr_ == curr_));
				}
			}

		private:
			friend class graph;
			outer_iterator curr_; // Current graph iterator
			outer_iterator end_; // End iterator
			inner_iterator pos_; // Position of inner iterator
		};

		[[nodiscard]] auto begin() const -> iterator {
			return iterator{graph_.begin(), graph_.end(), ((graph_.begin())->second).begin()};
		}

		[[nodiscard]] auto end() const -> iterator {
			return iterator{graph_.end(), graph_.end(), {}};
		}

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

		// Copy constructor to copy the whole old graph.
		graph(graph const& other) {
			// We create memory copies of the src nodes because they can be modified.
			// Note: We copy do modify memory for example in replace_node().
			for (auto i = other.graph_.begin(); i != other.graph_.end(); ++i) {
				insert_node(*(i->first));
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

		// This function inserts a new edge that is not in the graph.
		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either "
				                         "src or dst node does not exist");
			}
			// sNode is the set of edges going from src.
			auto sNode = graph_.find(src);
			// We need to make an edge to check if it already exists.
			// First we need to find the dst node share ptr and make a weak ptr.
			auto dNode = graph_.find(dst);
			std::weak_ptr<N> weak1 = dNode->first;
			// Make a pair of the weak ptr from above and weight from the args.
			std::pair<std::weak_ptr<N>, E> edge1(weak1, weight);
			// Check if the edge exists.
			auto foundEdge = sNode->second.find(edge1);
			// If the edge does not exist, add new edge.
			if (foundEdge == sNode->second.end()) {
				graph_[sNode->first].insert(edge1);
				return true;
			}
			return false;
		}

		auto replace_node(N const& old_data, N const& new_data) -> bool {
			if (!is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node "
				                         "that doesn't exist");
			}
			if (is_node(new_data)) {
				return false;
			}
			// Get the key corresponding to old data
			auto sNode = graph_.find(old_data)->first.get();
			// std::cout<< "***" << *sNode->first <<"\n";
			// std::cout<< sNode <<"\n";
			*sNode = new_data;
			std::cout << *sNode << "\n";
			return true;
		}

		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (!is_node(old_data) || !is_node(new_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old"
				                         " or new data if they don't exist in the graph");
			}
			// Get a pointer to old data node
			auto oNode = graph_.find(old_data);
			// Get a pointer to new data node
			auto nNode = graph_.find(new_data);
			// Merge old set into new set
			(nNode->second).merge(oNode->second);
			// Set the old incoming edges to point to new
			for (auto i = graph_.begin(); i != graph_.end(); ++i) {
				if (i->second.size() == 0 || i->first == nNode->first || i->first == oNode->first) {
					continue;
				}
				for (auto j = i->second.begin(); j != i->second.end(); ++j) {
					if (*(j->first).lock() == old_data) {
						// Create a new edge and add it to "i" as an outgoing node
						std::weak_ptr<N> weak1 = nNode->first;
						std::pair<std::weak_ptr<N>, E> edge1(weak1, j->second);
						graph_[i->first].insert(edge1);
					}
				}
			}
			// Finally delete the old node totally.
			erase_node(old_data);
		}

		auto erase_node(N const& value) -> bool {
			auto oNode = graph_.find(value);
			// Deleteing all of the outgoing edges
			oNode->second.clear();
			// Deleteing all of the incoming edges
			for (auto i = graph_.begin(); i != graph_.end(); ++i) {
				if (i->second.size() == 0) {
					continue;
				}
				for (auto j = i->second.begin(); j != i->second.end();) {
					if (*((j->first).lock()) == value) {
						i->second.erase(j++);
					}
					else {
						j++;
					}
				}
			}
			// Delete the node itself.
			graph_.erase(oNode);
			return (graph_.find(value) == graph_.end());
		}

		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst "
				                         "if they don't exist in the graph");
			}
			auto sNode = graph_.find(src);
			for (auto it = sNode->second.begin(); it != sNode->second.end(); ++it) {
				if ((*it->first.lock() == dst) && (it->second == weight)) {
					sNode->second.erase(it);
					return true;
				}
			}
			return false;
		}

		auto erase_edge(iterator i) -> iterator {
			auto nextptr = i++;
			auto b = erase_edge((*i).from, (*i).to, (*i).weight);

			if (b) {
				return nextptr;
			}
			return end();
		}

		auto erase_edge(iterator i, iterator s) -> iterator {
			for (auto it = i; it != s; it++) {
				erase_edge((*it).from, (*it).to, (*it).weight);
			}
			return s;
		}

		auto clear() noexcept -> void {
			graph_.clear();
		}

		/***************************************
		**                                    **
		**            Accessors               **
		**                                    **
		***************************************/

		// This function tells us if the node exists in the graph.
		[[nodiscard]] auto is_node(N const& value) -> bool {
			return (graph_.find(value) != graph_.end());
		}

		// This function tells us if the graph is empty.
		[[nodiscard]] auto empty() -> bool {
			return graph_.empty();
		}

		// Function that returns total number of nodes for testing purposes.
		[[nodiscard]] auto size() -> int {
			return static_cast<int>(graph_.size());
		}

		// This function tells us if a connection exists between src and dst.
		[[nodiscard]] auto is_connected(N const& src, N const& dst) -> bool {
			// error testing.
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst"
				                         " node don't exist in the graph");
			}
			// Get the key value pair related to src.
			auto sNode = graph_.find(src);
			// sNode->second is the set(value).
			return ((sNode->second).find(dst) != (sNode->second).end());
		}

		// This function returns a vector of all the nodes in the graph.
		[[nodiscard]] auto nodes() -> std::vector<N> {
			std::vector<N> v;
			for (auto i = graph_.begin(); i != graph_.end(); ++i) {
				v.emplace_back(*i->first);
			}
			return v;
		}

		// This function returns a vector of all the weight between 2 nodes.
		[[nodiscard]] auto weights(N const& src, N const& dst) -> std::vector<E> {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node"
				                         " don't exist in the graph");
			}
			auto sNode = graph_.find(src)->second;
			std::vector<E> v;
			for (auto it = sNode.begin(); it != sNode.end(); ++it) {
				if (*(it->first).lock() == dst) {
					v.emplace_back(it->second);
				}
			}
			return v;
		}

		// This function returns an iterator to an edge.
		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) -> iterator {
			auto srcNode = graph_.find(src);
			// auto sNode = srcNode->second;
			auto dNode = graph_.find(dst);
			std::weak_ptr<N> weak1 = dNode->first;
			std::pair<std::weak_ptr<N>, E> edge1(weak1, weight);
			auto foundEdge = srcNode->second.find(edge1);
			if (foundEdge == srcNode->second.end()) {
				return end();
			}
			else {
				return iterator{srcNode, graph_.end(), foundEdge};
			}
		}

		// This function returns a vector of all the nodes leaving src.
		[[nodiscard]] auto connections(N const& src) -> std::vector<N> {
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't"
				                         " exist in the graph");
			}
			auto sNode = graph_.find(src)->second;
			std::vector<N> v;
			for (auto it = sNode.begin(); it != sNode.end(); ++it) {
				// To avoid duplicates we do a binary search(very fast).
				if (!std::binary_search(v.begin(), v.end(), *(it->first.lock()))) {
					v.emplace_back(*(it->first.lock()));
				}
			}
			return v;
		}
		/*
		[[nodiscard]] auto operator==(graph const& other) -> bool {
		   auto a = other.graph_.begin();
		   while (a !=)
		}
		*/
		friend auto operator<<(std::ostream& os, graph const& g) -> std::ostream& {
			for (auto iter = g.graph_.begin(); iter != g.graph_.end(); ++iter) {
				os << *(iter->first) << "(" << '\n';
				for (auto it = iter->second.begin(); it != iter->second.end(); ++it) {
					os << '\t' << *(it->first.lock()) << " | " << (it->second) << '\n';
				}
				os << ")" << '\n';
			}
			return os;
		}

	private:
		std::map<std::shared_ptr<N>, destination_node, mapComparator> graph_;
	};
} // namespace gdwg

#endif // GDWG_GRAPH_HPP