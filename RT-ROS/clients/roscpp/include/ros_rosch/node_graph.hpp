#ifndef NODE_GRAPH_HPP
#define NODE_GRAPH_HPP
#include <string>
#include <map>
#include "yaml-cpp/yaml.h"

namespace rosch {
class NodeGraph {
public:
    NodeGraph(const std::string& filename="/tmp/node_graph.yaml");
    ~NodeGraph();
    int get_node_index(const std::string);
private:
    std::map<std::string, int> node_index_map;
    YAML::Node node_list;
};
}

#endif // NODE_GRAPH_HPP
