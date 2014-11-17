#ifndef __NODE_LIST_H__
#define __NODE_LIST_H__

#include "cmd_node.h"

struct node {
    cmd_node_t* head;
    cmd_node_t* tail;
    struct node* next_node;
};

typedef struct node node_t;

void insert_to_node_chain(node_t** chain, node_t* node);

#endif
