#include <iostream>
#include "graph.hpp"
#include <fstream>
#include <limits>
#include <iomanip>
#include <random>
#include <unordered_set>
#include <algorithm>
#include <iterator>
#include <memory>
#include <nlohmann/json.hpp>

#define MAX_CAP 0x3FFFFFFF

using json = nlohmann::json;

template<class T>
class Topology{
public:
  explicit Topology(int server_num, int source_num, int sink_num, std::vector<T> link_caps):
  server_num(server_num), source_num(source_num), sink_num(sink_num) {
    this->link_caps.assign(link_caps.begin(), link_caps.end());
  }

  auto GenRandomTopology() -> std::unique_ptr<Graph<T>>;
private:
  int server_num;
  int source_num;
  int sink_num;
  std::vector<T> link_caps;
};

template<class T>
auto Topology<T>::GenRandomTopology() -> std::unique_ptr<Graph<T>>{
  int source = server_num;
	int sink = source + 1;

  int max_loop = server_num - 1;

  std::random_device node_generator;
  std::uniform_int_distribution<int> node_distribution(0, 1);

  std::random_device cap_generator;
  std::uniform_int_distribution<int> cap_distribution(0, link_caps.size()-1);

  std::random_device source_generator;
  std::uniform_int_distribution<int> source_distribution(0, max_loop);

  std::random_device sink_generator;
  std::uniform_int_distribution<int> sink_distribution(0, max_loop);

  std::unordered_set<int> source_set;
  std::unordered_set<int> sink_set;

  auto g = std::make_unique<Graph<T>>(server_num+2);
  for (int i=0; i < max_loop; ++i) {
    for (int j=i+1; j < server_num; ++j) {
      if (node_distribution(node_generator)) {
        g->AddLink(i, j, link_caps[cap_distribution(cap_generator)]);
      }
    }
  }

  for (int i=0; i < source_num; ++i) {
    int source_node = source_distribution(source_generator);
    while (source_set.count(source_node)) {
      source_node = source_distribution(source_generator);
    }
    g->AddLink(source, source_node, static_cast<T>(MAX_CAP));
    source_set.insert(source_node);
  }

  for (int i=0; i < sink_num; ++i) {
    int sink_node = sink_distribution(sink_generator);
    while (source_set.count(sink_node) || sink_set.count(sink_node)) {
      sink_node = sink_distribution(sink_generator);
    }
    g->AddLink(sink_node, sink, static_cast<T>(MAX_CAP));
    sink_set.insert(sink_node);
  }
  return g;
}

int main(int argc, char *argv[]) {
  int node_num = 20;
  int source_num = 3;
  int sink_num = 5;

  int source = node_num;
	int sink = source + 1;

  int max_loop = node_num - 1;

  std::random_device node_generator;
  std::uniform_int_distribution<int> node_distribution(0, 1);

  std::random_device cap_generator;
  std::uniform_int_distribution<int> cap_distribution(0, 2);

  std::random_device source_generator;
  std::uniform_int_distribution<int> source_distribution(0, max_loop);

  std::random_device sink_generator;
  std::uniform_int_distribution<int> sink_distribution(0, max_loop);

  std::unordered_set<int> source_set;
  std::unordered_set<int> sink_set;


  double link_caps[3] = {10.5, 40.5, 100.5};

  Graph<double> g(node_num+2);
  for (int i=0; i < max_loop; ++i) {
    for (int j=i+1; j < node_num; ++j) {
      if (node_distribution(node_generator)) {
        g.AddLink(i, j, link_caps[cap_distribution(cap_generator)]);
      }
    }
  }

  for (int i=0; i < source_num; ++i) {
    int source_node = source_distribution(source_generator);
    while (source_set.count(source_node)) {
      source_node = source_distribution(source_generator);
    }
    g.AddLink(source, source_node, static_cast<double>(MAX_CAP));
    source_set.insert(source_node);
  }

  for (int i=0; i < sink_num; ++i) {
    int sink_node = sink_distribution(sink_generator);
    while (source_set.count(sink_node) || sink_set.count(sink_node)) {
      sink_node = sink_distribution(sink_generator);
    }
    g.AddLink(sink_node, sink, static_cast<double>(MAX_CAP));
    sink_set.insert(sink_node);
  }

  double max_flow = g.MaxFlow(source, sink);
  std::cout << "Max Flow: " << max_flow << std::endl;
	auto adj = g.GetLinks();
  
  std::vector<int> source_nodes(source_set.begin(), source_set.end());
  std::vector<int> sink_nodes(sink_set.begin(), sink_set.end());

	json res = {{"node_num", node_num}, {"source_nodes", source_nodes}, {"sink_nodes", sink_nodes}, {"max_flow", max_flow}};
	for (int i=0; i < node_num; i++) {
		std::string u = std::to_string(i);
		for (auto &link: adj[i]) {
			if (link.v < node_num && link.flow > 0) {
				res["nodes"][u][std::to_string(link.v)] = link.flow;
			}
		}
	}
	std::ofstream out("../example/testoutput.json");
	out << std::setw(4) << res << std::endl;
  return 0;
}