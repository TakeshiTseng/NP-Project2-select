#include "node_list.h"
#include <stdio.h>


void insert_to_node_chain(node_t** chain, node_t* node) {
    if(*chain == NULL) {
        *chain = node;
        (*chain)->next_node = NULL;
    } else {
        node_t* tmp_chain = *chain;
        while(tmp_chain->next_node != NULL) {
            tmp_chain = tmp_chain->next_node;
        }
        tmp_chain->next_node = node;
        node->next_node = NULL;
    }
}
