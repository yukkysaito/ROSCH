#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

struct node_t {
    std::string name;
    int index;
    int core;
    std::vector<std::string> v_sub_topic;
    std::vector<std::string> v_pub_topic;
    struct node_t* parent;
    struct node_t* child;
    struct node_t* next;
};

#endif // NODE_H
