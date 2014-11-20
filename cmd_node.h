#ifndef __CMD_NODE_H__
#define __CMD_NODE_H__

struct cmd_node {
    char* cmd;
    char** args;
    int pipe_to_file;
    char* filename;
    int pipe_count;
    struct cmd_node* next_node;
    int pipe_fd[2];
    int is_exec;

    // add pipe to user
    int pipe_to_user;
    int pipe_from_user;
    int user_id; // other client fd

};

typedef struct cmd_node cmd_node_t;

void free_cmd_list(cmd_node_t** cmd_list);
void insert_cmd_node(cmd_node_t** list, cmd_node_t* node);
cmd_node_t* pull_cmd_node(cmd_node_t** list);
cmd_node_t* clone_cmd_node(cmd_node_t* node);
void free_cmd_node(cmd_node_t* node);

#endif
