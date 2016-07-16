#include "ros_rosch/node_graph.hpp"
#include <iostream>
#include <map>
#include <string>
#include "yaml-cpp/yaml.h"

using namespace rosch;

NodeGraph::NodeGraph(const std::string& filename){
    try{
        node_list = YAML::LoadFile(filename);
        for (unsigned int i(0); i < node_list.size(); i++) {
            const YAML::Node subnode_name = node_list[i]["nodename"];
            const YAML::Node subnode_index = node_list[i]["nodeindex"];

            // std::cout << subnode_name.as<std::string>() << ":" << subnode_index.as<int>() << std::endl;
            node_index_map.insert(std::make_pair(subnode_name.as<std::string>(), subnode_index.as<int>()));
        }
    }
    catch(YAML::Exception& e) {
        std::cerr << e.what() << std::endl;
    }
};

NodeGraph::~NodeGraph(){};

int NodeGraph::get_node_index(const std::string node_name) {
    std::map<std::string, int>::const_iterator itr = node_index_map.find(node_name);
    if(itr != node_index_map.end()) {
        return itr->second;
    } else {
        return -1;
    }
};
