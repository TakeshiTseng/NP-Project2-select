#include "client_node.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cmd_node.h"

client_node_t* create_client_node(int client_sc_fd, char ip[16], int port) {

    client_node_t* new_client_node = malloc(sizeof(client_node_t));

    new_client_node->id = -1;
    new_client_node->client_sc_fd = client_sc_fd;
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
    for(c=1; c<=30; c++) {
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
    for(c=1; c<=30; c++) {
        if(client_list[c] != NULL && client_list[c]->client_sc_fd == fd) {
            return client_list[c];
        }
    }
    return NULL;
}

void broad_cast(client_node_t* current_client, char* message) {
    int c;
    for(c=1; c<=30; c++) {
        if(client_list[c] != NULL) {
            dup2(client_list[c]->client_sc_fd, 1);
            printf("%s", message);
            fflush(stdout);
        }
    }
    dup2(current_client->client_sc_fd, 1);
}


void set_env_to_client_node(client_node_t* client, char* name, char* val) {
    int c;
    for(c=0; c<client->num_env; c++) {
        if(strcmp(client->env[c], name) == 0) {
            strcpy(client->env_val[c], val);
            return;
        }
    }
    strcpy(client->env[client->num_env], name);
    strcpy(client->env_val[client->num_env], name);
    client->num_env++;
}

void remove_client_node(client_node_t* client) {
    int c;
    for(c=1; c<=30; c++) {
        if(client_list[c] == client) {
            client_list[c] = NULL;
        }
    }
}


int tell(client_node_t* current_client, char* client_id_str, char* message) {
    int c;
    int client_id = atoi(client_id_str);
    if(client_list[client_id] == NULL || client_id < 0 || client_id >= 30) {
        return -1;
    } else {
        dup2(client_list[client_id]->client_sc_fd, 1);
        printf("*** %s told you ***: %s\r\n", current_client->name, message);
        fflush(stdout);
        dup2(current_client->client_sc_fd, 1);
        return 0;
    }
}

int who(client_node_t* current_client) {
    int c;
    printf("<ID>\t<nickname>\t<IP/port>\t<indicate me>\r\n");
    for(c=1; c<=30; c++) {
        client_node_t* t_client = client_list[c];
        if(t_client != NULL) {
            printf("%d\t%s\t%s/%d", c, t_client->name, t_client->ip, t_client->port);
            if(client_list[c] == current_client) {
                printf("\t<-me");
            }
            printf("\r\n");
            fflush(stdout);
        }
    }
}

int check_name_exist(char* name) {
    int c;
    for(c=1; c<=30; c++) {
        if(client_list[c] != NULL && strcmp(client_list[c]->name, name) == 0) {
            return 1;
        }
    }
    return 0;
}
