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
}

NodeGraph::~NodeGraph()
{
}

int NodeGraph::get_node_index(const std::string node_name) {
    std::map<std::string, int>::const_iterator itr = node_index_map.find(node_name);
    if(itr != node_index_map.end()) {
        return itr->second;
    } else {
        return -1;
    }
}



void node_init(node_t* node);
void insert_child_node(node_t* parent_node, node_t* child_node);
node_t* make_node(int node_index);
node_t* search_node(node_t* node, int node_index);
node_t** search_leaf_node(node_t* node);
void show_tree_dfs(node_t* node);
void free_tree(node_t* node);
int free_node(node_t* root_node, node_t* node);
void display(node_t* node);
void _search_leaf_node(node_t* node, node_t** leaf_list, int* i);
int _is_exist_leaf_node(node_t* node, node_t** leaf_list, int list_size);
void _free_tree(node_t* root_node, node_t* node);
node_t* search_parent_node(node_t* node, int node_index);
void remove_node(node_t* root_node, node_t* node);
node_t* get_target(node_t* node);
void finish_target(node_t* root_node, node_t* node);
bool is_leaf_node(node_t* root_node, node_t* node);

SingletonNodeGraphAnalyzer::SingletonNodeGraphAnalyzer()
    :test(0)
{
    root_node = make_node(16);
//    node_t* node15 = make_node(15);
//    node_t* node14 = make_node(14);
//    node_t* node13 = make_node(13);
//    node_t* node12 = make_node(12);
//    node_t* node11 = make_node(11);
//    node_t* node21 = make_node(21);
//    node_t* node22 = make_node(22);
//    node_t* node23 = make_node(23);
    node_t* node31 = make_node(31);
    node_t* node32 = make_node(32);
//    node_t* node41 = make_node(41);
//    insert_child_node(root_node, node15);
//    insert_child_node(root_node, node41);
//    insert_child_node(node15, node14);
//    insert_child_node(node15, node32);
//    insert_child_node(node14, node13);
//    insert_child_node(node13, node12);
//    insert_child_node(node13, node23);
//    insert_child_node(node12, node11);
//    insert_child_node(node23, node22);
//    insert_child_node(node22, node21);
//    insert_child_node(node32, node21);
//    insert_child_node(node31, node31);
//    insert_child_node(node41, node21);
    insert_child_node(root_node, node31);
    insert_child_node(node31, node32);
    show_tree_dfs(root_node);
}

SingletonNodeGraphAnalyzer::~SingletonNodeGraphAnalyzer()
{
    free_graph_node_();
}

int SingletonNodeGraphAnalyzer::getIntTest() {
    test++;

    return test;
}

void SingletonNodeGraphAnalyzer::free_graph_node_() {
    free_tree(root_node);
}

node_t* SingletonNodeGraphAnalyzer::get_target_node() {
    get_target(root_node);
}
void SingletonNodeGraphAnalyzer::finish_target_node() {
    finish_target(root_node, get_target_node());
}

void SingletonNodeGraphAnalyzer::finish_node(node_t* node) {
    finish_target(root_node, node);
}

void SingletonNodeGraphAnalyzer::finish_node(int node_index) {
    finish_target(root_node, search_node(root_node, node_index));
}


SingletonNodeGraphAnalyzer& SingletonNodeGraphAnalyzer::getInstance() {
    static SingletonNodeGraphAnalyzer inst;
    return inst;
}



/*
 * Integrated from related to ROS function in RESCH
*/
void node_init(node_t* node)
{
    node->name = NULL;
    node->index = -1;
    node->pid = -1;
    node->depth = 0;
    node->is_exist = 0;
    node->is_exist = 0;
    node->wcet = -1;
    node->laxity = -1;
    node->global_wcet = -1;
    node->parent = NULL;
    node->child = NULL;
    node->next = NULL;
}

void insert_child_node(node_t* parent_node, node_t* child_node)
{
    if (parent_node == NULL || child_node == NULL)
        return;

    /* Update deepest depth */
    if(child_node->depth < parent_node->depth + 1 || child_node->depth == 0) {
        child_node->depth = parent_node->depth + 1;
    }

    if(parent_node->child == NULL) {
        parent_node->child = child_node;
    }
    else {
        node_t* next_child = parent_node->child;
        while (next_child->next != NULL) {
            next_child = next_child->next;
        }
        next_child->next = child_node;
    }
}

node_t* make_node(int node_index)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));
    if (node != NULL) {
        node_init(node);
        node->index = node_index;
        std::cout << "create:" << node_index << std::endl;
    }
    return node;
}

node_t* search_node(node_t* node, int node_index)
{
  if (node == NULL)
      return NULL;
  if(node->index == node_index)
      return node;

  if (node->child) {
      node_t* result = search_node(node->child, node_index);
      if (result != NULL)
          return result;
  }

  if (node->next) {
      node_t* result = search_node(node->next, node_index);
      if (result != NULL)
          return result;
  }

  return NULL;
}

void _search_leaf_node(node_t* node, node_t** leaf_list, int* i)
{
    /* if (node == NULL) */
    /*     return; */

    if (node->child) {
        _search_leaf_node(node->child, leaf_list, i);
    } else {
        if (_is_exist_leaf_node(node, leaf_list, *i)) {
            return;
        } else {
            leaf_list[(*i)] = node;
            ++(*i);
        }
    }

    if (node->next)
        _search_leaf_node(node->next, leaf_list, i);
}

int _is_exist_leaf_node(node_t* node, node_t** leaf_list, int list_size)
{
    int i;
    for (i = 0; i < list_size; ++i) {
        if (leaf_list[i] == node)
            return 1; // exist(true)
    }
    return 0;
}

/**
 * API: return leaf nodes as node_t**.
 * End of leaf is NULL.
 */
node_t** search_leaf_node(node_t* node)
{
    if (node == NULL)
        return NULL;

    int i = 0;
    static node_t* leaf_list[MAX_LEAF_LIST];

    _search_leaf_node(node, leaf_list, &i);

    leaf_list[i] = NULL;

    int j;
    for (j = 0; j < i; ++j) {
        std::cout << "leaf[" << leaf_list[j]->index << "] is found.\n" << std::endl;
    }

    return leaf_list;
}

bool is_leaf_node(node_t* root_node, node_t* node)
{
    if (node == NULL)
        return NULL;

    int i = 0;
    static node_t* leaf_list[MAX_LEAF_LIST];

    _search_leaf_node(root_node, leaf_list, &i);

    leaf_list[i] = NULL;

    int j;
    for (j = 0; j < i; ++j) {
        if(node->index == leaf_list[j]->index)
            return true;
    }

    return false;
}

void display(node_t* node)
{
    int i;
    char buf[DISPLAY_MAX_BUF];
    for (i = 0; i < node->depth && i < sizeof(buf); ++i) {
        buf[i] = ' ';
    }
    buf[i] = '\0';

    std::cout << buf << "L" << node->index << std::endl;
}

void show_tree_dfs(node_t* node)
{
  if (node == NULL)
      return;
  display(node);
  if (node->child)
      show_tree_dfs(node->child);
  if (node->next)
      show_tree_dfs(node->next);
}


void free_tree(node_t* root_node)
{
    _free_tree(root_node, root_node);
}


void _free_tree(node_t* root_node, node_t* node)
{
    if (node == NULL)
        return;
    if (node->child)
        _free_tree(root_node, node->child);
    if (node->next)
        _free_tree(root_node, node->next);
    display(node);

    free_node(root_node, node);
}

int free_node(node_t* root_node, node_t* node)
{
    if(is_leaf_node(root_node, node)) {
        remove_node(root_node, node);
        free(node);
    }
}

void remove_node(node_t* root_node, node_t* node) {
    node_t* parent_node = search_parent_node(root_node, node->index);

    while(parent_node != NULL) {
        if(parent_node->child){
            if(parent_node->child->index == node->index)
                parent_node->child = NULL;
        }
        if (parent_node->next) {
            if(parent_node->next->index == node->index)
                parent_node->next = NULL;
        }

        parent_node = search_parent_node(root_node, node->index);
    }
}

node_t* search_parent_node(node_t* node, int node_index) {
    if (node == NULL)
        return NULL;

    if (node->child) {
        if(node->child->index == node_index) {
            return node;
        }

        node_t* result = search_parent_node(node->child, node_index);
        if (result != NULL)
            return result;
    }

    if (node->next) {
        if(node->next->index == node_index) {
            return node;
        }

        node_t* result = search_parent_node(node->next, node_index);
        if (result != NULL)
            return result;
    }

    return NULL;
}

node_t* get_target(node_t* node)
{
    if (node == NULL)
        return NULL;
    if (node->child) {
        node_t* result = get_target(node->child);
        if (result != NULL)
            return result;
    }
    if (node->next) {
        node_t* result = get_target(node->next);
        if (result != NULL)
            return result;
        }

    return node;
}

void finish_target(node_t* root_node, node_t* node)
{
    free_node(root_node, node);
}
