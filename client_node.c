#include "client_node.h"
#include <string.h>
#include <stdlib.h>
#include "cmd_node.h"

client_node_t* create_client_node(int client_sc_fd, char ip[16], int port) {

    client_node_t* new_client_node = malloc(sizeof(client_node_t));

    new_client_node->id = -1;
    new_client_node->client_sc_fd;
    bzero(new_client_node->name, 20);
    strncpy(new_client_node->name, "(no name)", 10);
    strncpy(new_client_node->ip, ip, 16);
    new_client_node->port = port;
    new_client_node->cmd_exec_list = NULL;
    new_client_node->pipe_list = NULL;
    bzero(new_client_node->env[0], 1024);
    strcpy(new_client_node->env[0], "PATH");
    bzero(new_client_node->env_val[0], 1024);
    strcpy(new_client_node->env_val[0], "bin:.");
    new_client_node->num_env = 1;

    return new_client_node;
}

int insert_to_client_list(client_node_t* new_client_node) {
    int c;
    for(c=0; c<30; c++) {
        if(client_list[c] == NULL) {
            new_client_node->id = c;
            client_list[c] = new_client_node;
            return c;
        }
    }
    return -1;
}

client_node_t* find_client_node(int fd) {
    int c;
    for(c=0; c<30; c++) {
        if(client_list[c] != NULL && client_list[c]->client_sc_fd == fd) {
            return client_list[c];
        }
    }
    return NULL;
}
