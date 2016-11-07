#include "ros_rosch/node_graph.hpp"
#include "ros_rosch/type.h"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <map>
#include <string>

using namespace rosch;

NodesInfo::NodesInfo() : v_node_info_(0), config_() {
    std::string filename(config_.get_configpath());
    LoadConfig(filename);
}

NodesInfo::~NodesInfo() {}

void NodesInfo::LoadConfig(const std::string &filename) {
  try {
    YAML::Node node_list;
    node_list = YAML::LoadFile(filename);
    for (unsigned int i(0); i < node_list.size(); i++) {
      const YAML::Node name = node_list[i]["nodename"];
      const YAML::Node index = node_list[i]["nodeindex"];
      const YAML::Node core = node_list[i]["core"];
      const YAML::Node proc_core = node_list[i]["proc_core"];
      const YAML::Node start_time = node_list[i]["start_time"];
      const YAML::Node run_time = node_list[i]["run_time"];
      const YAML::Node subtopic = node_list[i]["sub_topic"];
      const YAML::Node pubtopic = node_list[i]["pub_topic"];

      NodeInfo node_info;
      node_info.name = name.as<std::string>();
//      node_info.index = index.as<int>();
      node_info.index = i;
      node_info.core = core.as<int>();

      node_info.v_subtopic.resize(0);
      for (int idx(0); idx < subtopic.size(); ++idx) {
        node_info.v_subtopic.push_back(subtopic[idx].as<std::string>());
      }

      node_info.v_pubtopic.resize(0);
      for (int idx(0); idx < pubtopic.size(); ++idx) {
        node_info.v_pubtopic.push_back(pubtopic[idx].as<std::string>());
      }

      node_info.v_sched_info.resize(0);
      for (int idx(0); idx < proc_core.size(); ++idx) {
          SchedInfo sched_info;
          sched_info.core = proc_core.as<int>();
          sched_info.run_time = run_time.as<int>();
          sched_info.start_time = start_time.as<int>();
          sched_info.end_time = sched_info.start_time + sched_info.run_time;
          node_info.v_sched_info.push_back(sched_info);
      }
      v_node_info_.push_back(node_info);
    }
  } catch (YAML::Exception &e) {
      std::cerr << e.what() << std::endl;
  }
}

NodeInfo NodesInfo::GetNodeInfo(const int index)
{
    try {
        for (int idx(0); idx < v_node_info_.size(); ++idx) {
            if (v_node_info_.at(idx).index == index)
                return v_node_info_.at(idx);
        }
        throw index;
    }
    catch (int e){
        std::cerr << "Node index : " << e << ". cannot find node infomartion." << std::endl;
    }
}

NodeInfo NodesInfo::GetNodeInfo(const std::string name)
{
    try {
        for (int idx(0); idx < v_node_info_.size(); ++idx) {
            if (v_node_info_.at(idx).name == name)
                return v_node_info_.at(idx);
        }
        throw name;
    }
    catch (std::string e){
        std::cerr << "Node name : " << e << ". cannot find node infomartion." << std::endl;
    }
}

size_t NodesInfo::GetNodeListSize(void) { return v_node_info_.size(); }
