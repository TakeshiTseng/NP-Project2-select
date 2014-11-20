#ifndef __TOKEN_H__
#define __TOKEN_H__


#define CMD      0 // command or argument or file name
#define PIPE     1
#define PIPE_N   2
#define PASS     3
#define NEW_LINE 4
#define FEOF     5
#define PIPE_TO_USR 6
#define PIPE_FROM_USR 7


struct token_node {
    int type;
    char* token_str;
    struct token_node* next_node;
};


typedef struct token_node token_node_t;

void insert_node(token_node_t** head, token_node_t* node);
token_node_t* pull_node(token_node_t** list);
void free_list(token_node_t** head);

#endif
