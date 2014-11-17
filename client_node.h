#ifndef __CLIENT_NODE_H__
#define __CLIENT_NODE_H__

struct client_node {
    int id;
    int client_sc_fd;
    char name[256];
};

struct client_node client_node_t;

#endif
