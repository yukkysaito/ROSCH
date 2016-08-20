#include "node_graph.h"
#include "node.h"
#include "node_graph_core.h"
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace sched_analyzer;

NodeGraph::NodeGraph() : config_() {
  std::string filename(config_.get_configpath());
  load_config_(filename);
}

NodeGraph::~NodeGraph() {}

void NodeGraph::load_config_(const std::string &filename) {
  std::cout << filename << std::endl;

  try {
    YAML::Node node_list;
    node_list = YAML::LoadFile(filename);
    for (unsigned int i(0); i < node_list.size(); i++) {
      const YAML::Node subnode_name = node_list[i]["nodename"];
      const YAML::Node subnode_index = node_list[i]["nodeindex"];
      const YAML::Node subnode_core = node_list[i]["core"];
      const YAML::Node subnode_subtopic = node_list[i]["sub_topic"];
      const YAML::Node subnode_pubtopic = node_list[i]["pub_topic"];
#ifdef SCHED_ANALYZER
      const YAML::Node subnode_runtime = node_list[i]["run_time"];
#endif
      node_info_t node_info;
      node_info.name = subnode_name.as<std::string>();
      // node_info.index = subnode_index.as<int>();
      node_info.index = i;
#ifdef SCHED_ANALYZER
      node_info.run_time = subnode_runtime.as<int>();
#endif
      node_info.core = subnode_core.as<int>();
      node_info.v_subtopic.resize(0);
      for (int j(0); (size_t)j < subnode_subtopic.size(); ++j) {
        node_info.v_subtopic.push_back(subnode_subtopic[j].as<std::string>());
      }
      node_info.v_pubtopic.resize(0);
      for (int j(0); (size_t)j < subnode_pubtopic.size(); ++j) {
        node_info.v_pubtopic.push_back(subnode_pubtopic[j].as<std::string>());
      }

      v_node_info_.push_back(node_info);
    }
  } catch (YAML::Exception &e) {
    std::cerr << e.what() << std::endl;
  }
}

int NodeGraph::get_node_index(const std::string &node_name) {
  for (int i(0); (size_t)i < v_node_info_.size(); ++i) {
    if (v_node_info_.at(i).name == node_name)
      return v_node_info_.at(i).index;
  }
  exit(-1);
}

size_t NodeGraph::get_node_list_size() { return v_node_info_.size(); }

std::string NodeGraph::get_node_name(const int node_index) {
  for (int i(0); (size_t)i < v_node_info_.size(); ++i) {
    if (v_node_info_.at(i).index == node_index)
      return v_node_info_.at(i).name;
  }
  exit(-1);
}

int NodeGraph::get_node_core(const int node_index) {
  for (int i(0); (size_t)i < v_node_info_.size(); ++i) {
    if (v_node_info_.at(i).index == node_index)
      return v_node_info_.at(i).core;
  }
  exit(-1);
}

int NodeGraph::get_node_core(const std::string &node_name) {
  for (int i(0); (size_t)i < v_node_info_.size(); ++i) {
    if (v_node_info_.at(i).name == node_name)
      return v_node_info_.at(i).core;
  }
  exit(-1);
}

std::vector<std::string> NodeGraph::get_node_subtopic(const int node_index) {
  for (int i(0); (size_t)i < v_node_info_.size(); ++i) {
    if (v_node_info_.at(i).index == node_index) {
      return v_node_info_.at(i).v_subtopic;
    }
  }
  exit(-1);
}

std::vector<std::string>
NodeGraph::get_node_subtopic(const std::string &node_name) {
  for (int i(0); (size_t)i < v_node_info_.size(); ++i) {
    if (v_node_info_.at(i).name == node_name)
      return v_node_info_.at(i).v_subtopic;
  }
  exit(-1);
}

int NodeGraph::get_node_run_time(const int node_index) {
  for (int i(0); (size_t)i < v_node_info_.size(); ++i) {
    if (v_node_info_.at(i).index == node_index)
      return v_node_info_.at(i).run_time;
  }
  exit(-1);
}

std::vector<std::string> NodeGraph::get_node_pubtopic(const int node_index) {
  for (int i(0); (size_t)i < v_node_info_.size(); ++i) {
    if (v_node_info_.at(i).index == node_index) {
      return v_node_info_.at(i).v_pubtopic;
    }
  }
  exit(-1);
}
std::vector<std::string>
NodeGraph::get_node_pubtopic(const std::string &node_name) {
  for (int i(0); (size_t)i < v_node_info_.size(); ++i) {
    if (v_node_info_.at(i).name == node_name) {
      return v_node_info_.at(i).v_pubtopic;
    }
  }
  exit(-1);
}

SingletonNodeGraphAnalyzer::SingletonNodeGraphAnalyzer() {
  root_node = native_c::make_root_node();
  /* Create Nodes */
  for (int index(0); (size_t)index < get_node_list_size(); ++index) {
    v_node_.push_back(
        native_c::make_node(get_node_name(index), index, get_node_core(index),
                            get_node_subtopic(index), get_node_pubtopic(index),
                            get_node_run_time(index)));
  }
  /* Create DAG from Node info */
  for (int index(0); (size_t)index < get_node_list_size(); ++index) {
    for (int index2(0); (size_t)index2 < get_node_list_size(); ++index2) {
      for (int i(0); (size_t)i < v_node_.at(index)->v_sub_topic.size(); ++i) {
        for (int j(0); (size_t)j < v_node_.at(index2)->v_pub_topic.size();
             ++j) {
          if (v_node_.at(index)->v_sub_topic.at(i) ==
              v_node_.at(index2)->v_pub_topic.at(j)) {
            native_c::insert_child_node(v_node_.at(index), v_node_.at(index2));
            std::cout << "-" << v_node_.at(index2)->name << std::endl;
          }
        }
      }
    }
    std::cout << "--->" << v_node_.at(index)->name << std::endl;
  }
  /* Set to the root node */
  for (int index(0); (size_t)index < get_node_list_size(); ++index) {
    if (v_node_.at(index)->parent[0] == NULL) {
      native_c::insert_child_node(root_node, v_node_.at(index));
    }
  }

  /*
  native_c::free_node(root_node, native_c::search_node(root_node, 0));
  native_c::show_tree_dfs(root_node);
  native_c::show_leaf_list(root_node);
  */
}

SingletonNodeGraphAnalyzer::~SingletonNodeGraphAnalyzer() {
  free_graph_node_();
}

node_t *SingletonNodeGraphAnalyzer::search_node(int node_index) {
  return native_c::search_node(root_node, node_index);
}

void SingletonNodeGraphAnalyzer::compute_laxity() {
  native_c::compute_laxity(root_node);
}

void SingletonNodeGraphAnalyzer::free_graph_node_() {
  native_c::free_tree(root_node);
}

node_t *SingletonNodeGraphAnalyzer::get_target_node() {
  return native_c::get_target(root_node);
}

/*
 * API : Result is list of schedulable nodes.
 */
bool SingletonNodeGraphAnalyzer::sched_leaf_list(
    std::vector<node_t> &node_list) {

  node_t **leaf_list = native_c::search_leaf_node(root_node);
  for (int i(0); i < native_c::get_max_leaf_list(); ++i) {
    if (leaf_list[i] == NULL) {
      return true;
    }
    node_list.push_back(*(leaf_list[i]));
  }
  return false;
}

bool SingletonNodeGraphAnalyzer::sched_child_list(
    int index, std::vector<node_t> &node_list) {
  node_t *child_list[CHILD_SIZE];
  if (native_c::search_child_nodes(root_node, index, child_list, CHILD_SIZE)) {
    for (int i(0); i < CHILD_SIZE; ++i) {
      if (child_list[i] == NULL) {
        return true;
      }
      node_list.push_back(*(child_list[i]));
    }
  }
  return false;
}

bool SingletonNodeGraphAnalyzer::sched_parent_list(
    int index, std::vector<node_t> &node_list) {
  node_t *parent_list[PARENT_SIZE];
  if (native_c::search_parent_nodes(root_node, index, parent_list,
                                    PARENT_SIZE)) {

    for (int i(0); i < PARENT_SIZE; ++i) {
      if (parent_list[i] == NULL) {
        return true;
      }
      node_list.push_back(*parent_list[i]);
    }
  }
  return false;
}

/*
 * API : Result is list of schedulable nodes.
 */
bool SingletonNodeGraphAnalyzer::sched_all_node_queue(
    std::vector<node_t> &node_list) {
  std::vector<sort_inv_laxity_t> arr;
  for (int i(0); (size_t)i < v_node_.size(); ++i) {
    sort_inv_laxity_t element = {v_node_.at(i)->index,
                                 v_node_.at(i)->inv_laxity,
                                 v_node_.at(i)->run_time};
    arr.push_back(element);
  }
  std::sort(arr.begin(), arr.end());

  int pre_inv_laxity = 0;
  int same_count = 0;
  for (std::vector<sort_inv_laxity_t>::iterator it = arr.begin();
       it != arr.end(); ++it) {
    if (pre_inv_laxity == it->inv_laxity) {
      std::cerr << get_node_name(it->index) << " has same inv_laxity."
                << std::endl
                << "You should confirm this." << std::endl;
      same_count++;
    }
    node_list.push_back(*(search_node(it->index)));

    pre_inv_laxity = it->inv_laxity;
  }

  return true;
}

void SingletonNodeGraphAnalyzer::finish_target_node() {
  native_c::finish_target(root_node, get_target_node());
}

void SingletonNodeGraphAnalyzer::show_tree_dfs() {
  native_c::show_tree_dfs(root_node);
}

void SingletonNodeGraphAnalyzer::show_leaf_list() {
  native_c::show_leaf_list(root_node);
}

void SingletonNodeGraphAnalyzer::finish_node(node_t *node) {
  native_c::finish_target(root_node, node);
}

void SingletonNodeGraphAnalyzer::finish_node(int node_index) {
  native_c::finish_target(root_node, search_node(node_index));
}

bool SingletonNodeGraphAnalyzer::is_empty_topic_list(int node_index) {
  node_t *searched = search_node(node_index);
  if (searched == NULL)
    return false;
  else
    return searched->v_sub_topic.empty();
}

bool SingletonNodeGraphAnalyzer::is_in_node_graph(int node_index) {
  node_t *searched = search_node(node_index);
  if (searched == NULL)
    return false;
  return true;
}

void SingletonNodeGraphAnalyzer::finish_topic(int node_index,
                                              std::string topic) {
  std::vector<std::string>::iterator itr =
      search_node(node_index)->v_sub_topic.begin();
  while (itr != search_node(node_index)->v_sub_topic.end()) {
    if (*itr == topic) {
      itr = search_node(node_index)->v_sub_topic.erase(itr);
    } else {
      itr++;
    }
  }
}

void SingletonNodeGraphAnalyzer::refresh_topic_list(int node_index) {
  std::vector<std::string> v_subtopic(get_node_subtopic(node_index));
  search_node(node_index)->v_sub_topic.resize(v_subtopic.size());
  std::copy(v_subtopic.begin(), v_subtopic.end(),
            search_node(node_index)->v_sub_topic.begin());
}

SingletonNodeGraphAnalyzer &SingletonNodeGraphAnalyzer::getInstance() {
  static SingletonNodeGraphAnalyzer inst;
  return inst;
}
