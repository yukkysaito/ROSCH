#ifndef NODE_GRAPH_HPP
#define NODE_GRAPH_HPP
#include <string>
#include <map>
#include "yaml-cpp/yaml.h"


#define DISPLAY_MAX_BUF 32
#define MAX_LEAF_LIST 16

typedef struct node {
    char* name;
    int index;
    int pid;
    int depth;
    int is_exist;
    int target;
    float wcet;
    float laxity;
    float global_wcet;
    std::vector<std::string> v_topic;
    struct node* parent;
    struct node* child;
    struct node* next;
    /* struct node* parent_next; */
} node_t;

typedef struct node_info {
    std::string name;
    int index;
    std::vector<std::string> v_subtopic;
} node_info_t;

namespace rosch {
class NodeGraph {
public:
    NodeGraph(const std::string& filename="/tmp/node_graph.yaml");
    ~NodeGraph();
    int get_node_index(const std::string);
    std::string get_node_name(const int node_index);
    std::vector<std::string> get_node_subtopic(const int node_index);
private:
    std::vector<node_info_t> v_node_info_;
    YAML::Node node_list;
};

class SingletonNodeGraphAnalyzer : public NodeGraph{
private:
    SingletonNodeGraphAnalyzer();
    SingletonNodeGraphAnalyzer(const SingletonNodeGraphAnalyzer&);
    SingletonNodeGraphAnalyzer& operator=(const SingletonNodeGraphAnalyzer&);
    ~SingletonNodeGraphAnalyzer();
    void free_graph_node_();
    int test;

public:
    node_t* root_node;
    static SingletonNodeGraphAnalyzer& getInstance();
    int getIntTest();
    node_t* get_target_node();
    void finish_target_node();
    void finish_node(node_t* node);
    void finish_node(int node_index);
    void finish_topic(int node_index, std::string topic);
    node_t* search_node(int node_index);
    bool is_empty_topic_list(int node_index);
};
}

#endif // NODE_GRAPH_HPP
