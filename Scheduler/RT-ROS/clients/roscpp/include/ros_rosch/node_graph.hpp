#ifndef NODE_GRAPH_HPP
#define NODE_GRAPH_HPP
#include "ros_rosch/config.h"
#include "ros_rosch/spec.h"
#include "ros_rosch/type.h"
#include "yaml-cpp/yaml.h"
#include <map>
#include <string>
#include <vector>

namespace rosch {
class NodesInfo {
public:
    NodesInfo();
    NodesInfo(const std::string &filename);
    ~NodesInfo();
    NodeInfo GetNodeInfo(const int index);
    NodeInfo GetNodeInfo(const std::string name);
    size_t GetNodeListSize(void);
private:
  Config config_;
  void LoadConfig(const std::string &filename);
  std::vector<NodeInfo> v_node_info_;
};
}

#endif // NODE_GRAPH_HPP
