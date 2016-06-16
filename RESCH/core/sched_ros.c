#include <asm/current.h>
#include <linux/slab.h>
#include "sched_ros.h"

void node_init(node_t* node)
{
    node->name = NULL;
    node->index = -1;
    node->pid = -1;
    node->depth = 0;
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
    child_node->depth = parent_node->depth + 1;
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
    node_t *node = kmalloc(sizeof(node_t), GFP_KERNEL);
    if (node != NULL) {
        node_init(node);
        node->index = node_index;
        printk(KERN_INFO
               "create:%d\n", node_index);
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

void display(node_t* node)
{
    int i;
    char buf[32];
    for (i = 0; i < node->depth && i < 32; ++i) {
        buf[i] = ' ';
    }
    buf[i] = '\0';

    printk(KERN_INFO
           "%sL %d\n", buf, node->index);
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

void free_tree(node_t* node)
{
  if (node == NULL)
      return;
  if (node->child)
      free_tree(node->child);
  if (node->next)
      free_tree(node->next);
  display(node);
  free_node(node);
}

int free_node(node_t* node)
{
    kfree(node);
}
