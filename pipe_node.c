#include "pipe_node.h"
#include <stdio.h>

void remove_pipe_node(pipe_node_t** list, pipe_node_t* node) {
    if(*list == node) {
        *list = (*list)->next_node;
    } else {
        pipe_node_t* tmp_list = *list;
        while(tmp_list != NULL) {
            if(tmp_list->next_node == node) {
                tmp_list->next_node = node->next_node;
                break;
            }
            tmp_list = tmp_list->next_node;
        }
    }
}

void insert_pipe_node(pipe_node_t** list, pipe_node_t* node) {

    // dirty fix
    node->next_node = NULL;

    if(*list == NULL) {
        *list = node;
    } else {
        node->next_node = *list;
        *list = node;
    }
}

void decrease_all_pipe_node(pipe_node_t* list) {
    while(list != NULL) {
        list->count--;
        list = list->next_node;
    }
}
void inscrease_all_pipe_node(pipe_node_t* list) {
    while(list != NULL) {
        list->count++;
        list = list->next_node;
    }

}
pipe_node_t* find_pipe_node_by_count(pipe_node_t* list, int count) {
    pipe_node_t* result = NULL;
    while(list != NULL) {
        if(list->count == count) {
            result = list;
            break;
        }
        list = list->next_node;
    }
    return result;
}
