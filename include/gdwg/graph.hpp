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
		graph(){};
		//graph() = default;

		graph(std::initializer_list<N> il) {
			for(auto i = il.begin(); i != il.end(); ++i) {
				auto exist = graph_.find(il);   //il is not a pointer??
				if (exist == graph_.end()) {
					graph_[std::make_shared<N>(i)];    //is this the correct way to not add value
				}                                      //and is <N>(i) correct or should it be *i?
			}
		}



		struct value_type {
			N from;
			N to;
			E weight;
		};

		// Your member functions go here
	private:
		std::map<std::shared_ptr<N>, std::set<std::shared_ptr<std::pair<N&, E>>>> graph_;
	};
} // namespace gdwg

#endif // GDWG_GRAPH_HPP
