#include "cmd_node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void insert_cmd_node(cmd_node_t** head, cmd_node_t* node) {

    if(*head == NULL) {
        *head = node;
        (*head)->next_node = NULL;
    } else {
        cmd_node_t* _p = *head;
        while(_p->next_node != NULL) {
            _p = _p->next_node;
        }
        _p->next_node = node;
        node->next_node = NULL;
    }
}

cmd_node_t* pull_cmd_node(cmd_node_t** list) {
    if(*list == NULL) {
        return NULL;
    } else {
        cmd_node_t* _to_return = *list;
        *list = (*list)->next_node;
        _to_return->next_node = NULL;
        return _to_return;
    }
}

void free_cmd_list(cmd_node_t** cmd_list) {
    while(*cmd_list != NULL) {
        cmd_node_t* tmp = *cmd_list;
        (*cmd_list) = (*cmd_list)->next_node;
        tmp->next_node = NULL;
        free(tmp);
    }
}

cmd_node_t* clone_cmd_node(cmd_node_t* node) {
    cmd_node_t* new_node = malloc(sizeof(cmd_node_t));

    // copy cmd
    if(node->cmd != NULL) {
        new_node->cmd = malloc(sizeof(node->cmd));
        strcpy(new_node->cmd, node->cmd);
    } else {
        new_node->cmd = NULL;
    }

    // copy args
    if(node->args != NULL) {
        int num_of_args = 0;
        char** tmp_args = node->args;
        while(tmp_args[num_of_args++] != NULL);
        new_node->args = malloc(sizeof(char*) * num_of_args);
        int c;
        for(c=0; c<num_of_args-1; c++) {
            new_node->args[c] = malloc(sizeof(node->args[c]));
            strcpy(new_node->args[c], node->args[c]);
        }
        new_node->args[num_of_args-1] = NULL;
    } else {
        new_node->args = NULL;
    }

    // copy pipe_to_count
    new_node->pipe_count = node->pipe_count;

    // copy pipe_to_file
    new_node->pipe_to_file = node->pipe_to_file;

    // copy filename
    if(node->filename != NULL) {
        new_node->filename = malloc(sizeof(node->filename));
        strcpy(new_node->filename, node->filename);
    } else {
        new_node->filename = NULL;
    }
}
void free_cmd_node(cmd_node_t* node) {
    node->next_node = NULL;
    free(node);
}
