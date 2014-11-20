#ifndef __PIPE_NODE_H__
#define __PIPE_NODE_H__
struct pipe_node {
    int count;
    int out_fd;
    int in_fd;
    struct pipe_node* next_node;

    // for global use
    int from_user_id;
    int to_user_id;
};

typedef struct pipe_node pipe_node_t;

void remove_pipe_node(pipe_node_t** list, pipe_node_t* node);
void insert_pipe_node(pipe_node_t** list, pipe_node_t* node);
void decrease_all_pipe_node(pipe_node_t* list);
void inscrease_all_pipe_node(pipe_node_t* list);
pipe_node_t* find_pipe_node_by_count(pipe_node_t* list, int count);

#endif
