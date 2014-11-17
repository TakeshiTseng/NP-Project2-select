#include "token.h"
#include <stdio.h>
#include <stdlib.h>

void free_list(token_node_t** head) {
    if(*head == NULL) {
        return;
    }
    if((*head)->next_node == NULL) {
        free(*head);
    } else {
        free_list(&((*head)->next_node));
        (*head)->next_node = NULL;
        free(*head);
        *head = NULL;
    }
}

void insert_node(token_node_t** head, token_node_t* node) {

    if(*head == NULL) {
        *head = node;
        (*head)->next_node = NULL;
    } else {
        token_node_t* _p = *head;
        // walk to last node
        while(_p->next_node != NULL) {
            _p = _p->next_node;
        }
        _p->next_node = node;
        node->next_node = NULL;
    }

}

token_node_t* pull_node(token_node_t** list) {
    if(*list == NULL) {
        return NULL;
    } else {
        token_node_t* _to_return = *list;
        *list = (*list)->next_node;
        return _to_return;
    }
}
