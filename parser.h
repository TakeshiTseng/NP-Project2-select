#ifndef __PARSER_H__
#define __PARSER_H__
#include "cmd_node.h"

// states
#define STATE_INIT 0
#define STATE_CMD 1
#define STATE_FILE 2
#define STATE_ARGS 3
#define STATE_NW 4

struct arg_node {
    char* arg_str;
    struct arg_node* next_node;
};

typedef struct arg_node arg_node_t;
void parse_tokens(cmd_node_t** cmd_node_list);

#endif
