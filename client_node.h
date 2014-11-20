#ifndef __CLIENT_NODE_H__
#define __CLIENT_NODE_H__
#include "cmd_node.h"
#include "pipe_node.h"

struct client_node {
    int id;
    int client_sc_fd;
    char name[20];
    char ip[16];
    int port;

    cmd_node_t* cmd_exec_list;
    pipe_node_t* pipe_list;

    char env[256][1024];
    char env_val[256][1024];
    char num_env;
};

typedef struct client_node client_node_t;

client_node_t* create_client_node(int client_socket_fd, char ip[16], int port);
int insert_to_client_list(client_node_t* new_client_node);
client_node_t* find_client_node(int fd);

// global variable
client_node_t* client_list[31];

#endif
