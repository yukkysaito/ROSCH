#ifndef NODE_GRAPH_HPP
#define NODE_GRAPH_HPP
#include <string>
#include <map>
#include "yaml-cpp/yaml.h"


#define DISPLAY_MAX_BUF 32
#define MAX_LEAF_LIST 16
#define MAX_FREED_NODE_LIST 32

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
    struct node* parent;
    struct node* child;
    struct node* next;
    /* struct node* parent_next; */
} node_t;

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
};
}

#endif // NODE_GRAPH_HPP
