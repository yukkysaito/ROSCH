#ifndef NODE_GRAPH_H
#define NODE_GRAPH_H

#include "config.h"
#include "node.h"
#include "spec.h"
#include "yaml-cpp/yaml.h"
#include <string>
#include <vector>

namespace sched_analyzer {
class NodeGraph {
public:
  NodeGraph();
  ~NodeGraph();
  int get_node_index(const std::string &);
  std::string get_node_name(const int node_index);
  int get_node_core(const int node_index);
  int get_node_core(const std::string &node_name);
  int get_node_run_time(const int node_index);
  std::vector<std::string> get_node_subtopic(const int node_index);
  std::vector<std::string> get_node_subtopic(const std::string &node_name);
  std::vector<std::string> get_node_pubtopic(const int node_index);
  std::vector<std::string> get_node_pubtopic(const std::string &node_name);
  size_t get_node_list_size();

private:
  void load_config_(const std::string &filename);
  Config config_;
  std::vector<node_info_t> v_node_info_;
};

class SingletonNodeGraphAnalyzer : public NodeGraph {
public:
  static SingletonNodeGraphAnalyzer &getInstance();
  node_t *get_target_node();
  bool sched_leaf_list(std::vector<node_t> &node_list);
  bool sched_all_node_queue(std::vector<node_t> &node_list);
  bool sched_child_list(int index, std::vector<node_t> &node_list);
  bool sched_parent_list(int index, std::vector<node_t> &node_list);
  void finish_target_node();
  void finish_node(node_t *node);
  void finish_node(int node_index);
  void finish_topic(int node_index, std::string topic);
  node_t *search_node(int node_index);
  bool is_empty_topic_list(int node_index);
  bool is_in_node_graph(int node_index);
  void refresh_topic_list(int node_index);
  void show_tree_dfs();
  void show_leaf_list();
  void compute_laxity();

private:
  typedef struct sort_inv_laxity_t {
    int index;
    int inv_laxity;
    int run_time;
    bool operator<(const sort_inv_laxity_t &right) const {
      return inv_laxity == right.inv_laxity ? run_time > right.run_time
                                            : inv_laxity > right.inv_laxity;
    }
  } sort_inv_laxity_t;
  enum { PARENT_SIZE = 16, CHILD_SIZE = 16 }; // enum hack

  SingletonNodeGraphAnalyzer();
  SingletonNodeGraphAnalyzer(const SingletonNodeGraphAnalyzer &);
  SingletonNodeGraphAnalyzer &operator=(const SingletonNodeGraphAnalyzer &);
  ~SingletonNodeGraphAnalyzer();
  void free_graph_node_();
  std::vector<node_t *> v_node_; // Vector of nodes
  node_t *root_node;
};
}
#endif // NODE_GRAPH_H
