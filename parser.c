#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "tokenizer.h"
#include "cmd_node.h"

int _next_state(int state, int next_token_type);
void _insert_arg(arg_node_t** arg_list, arg_node_t* node);


void parse_tokens(cmd_node_t** cmd_node_list) {

    int state = STATE_INIT;
    int pre_state = STATE_INIT;
    cmd_node_t* current_cmd_node = NULL;

    // token
    int token_type;
    char* token_str;

    token_type = next_token(&token_str);

    while(token_type != NEW_LINE) {
        if(token_type == FEOF) {
            return;
        }
        if(state == STATE_INIT){
            if(current_cmd_node == NULL) {
                state = _next_state(state, token_type);
            } else {
                // pipe
                if(token_type == PIPE) {
                    current_cmd_node->pipe_count = 1;
                } else {
                    // pipt n
                    // cut first letter
                    char* pipe_n_str;
                    pipe_n_str = malloc(sizeof(char)*(strlen(token_str)+1));
                    strcpy(pipe_n_str, (token_str+1));
                    // set number to pipe count
                    current_cmd_node->pipe_count = atoi(pipe_n_str);
                    free(pipe_n_str);
                }

                insert_cmd_node(cmd_node_list, current_cmd_node);
                current_cmd_node = NULL;

                // get next token
                token_type = next_token(&token_str);
                // change to next state
                pre_state = state;
                state = _next_state(state, token_type);
            }
        } else if(state == STATE_CMD) {
            // from init state
            current_cmd_node = malloc(sizeof(cmd_node_t));

            // init data
            current_cmd_node->pipe_to_file = 0;// not to pipe to file
            current_cmd_node->pipe_count = 0;
            current_cmd_node->next_node = NULL;
            current_cmd_node->filename = NULL;
            current_cmd_node->pipe_fd[0] = -1;
            current_cmd_node->pipe_fd[1] = -1;
            current_cmd_node->is_exec = 0;

            // copy text to cmd node
            current_cmd_node->cmd = malloc(sizeof(char)*(strlen(token_str) + 1));
            strcpy(current_cmd_node->cmd, token_str);

            // set first argument to cmd
            // and last argument to NULL
            current_cmd_node->args = malloc(sizeof(char*) * 2);
            current_cmd_node->args[0] = malloc(sizeof(char)*(strlen(token_str) + 1));
            strcpy(current_cmd_node->args[0], token_str);
            current_cmd_node->args[1] = NULL;

            // change to next node
            token_type = next_token(&token_str);

            // change state
            pre_state = state;
            state = _next_state(state, token_type);

        } else if(state == STATE_ARGS) {
            // from cmd
            // calculate how many arguments
            arg_node_t* arg_list = NULL;
            int count = 2;
            // set count to 2 because command itself is the first argument
            // and we need to add a NULL to last argument
            while(token_type == CMD) {
                arg_node_t* arg = malloc(sizeof(arg_node_t));
                arg->arg_str = malloc(sizeof(char)*(strlen(token_str)+1));
                strcpy(arg->arg_str, token_str);
                _insert_arg(&arg_list, arg);
                token_type = next_token(&token_str);
                count++;
            }

            current_cmd_node->args = malloc(sizeof(char*) * count);

            // first to cmd
            current_cmd_node->args[0] = malloc(sizeof(char) * (strlen(current_cmd_node->cmd) + 1));
            strcpy(current_cmd_node->args[0], current_cmd_node->cmd);
            current_cmd_node->args[count-1] = NULL; // last to NULL

            count = 1;

            while(arg_list != NULL) {
                current_cmd_node->args[count] = malloc(sizeof(char)*(strlen(arg_list->arg_str)+1));
                strcpy(current_cmd_node->args[count], arg_list->arg_str);
                count++;

                // release unused node
                arg_node_t* node_to_release = arg_list;
                arg_list = arg_list->next_node;
                node_to_release->next_node = NULL;
                free(node_to_release);
            }

            pre_state = state;
            state = _next_state(state, token_type);
        } else if(state == STATE_FILE) {
            // release ">" token node
            token_type = next_token(&token_str);
            current_cmd_node->pipe_to_file = 1;
            current_cmd_node->filename = malloc(sizeof(char)*(strlen(token_str)+1));
            strcpy(current_cmd_node->filename, token_str);


            token_type = next_token(&token_str);
            pre_state = state;
            state = _next_state(state, token_type);
        }
    }
    // new line
    if(pre_state == STATE_INIT) {
        // do nothing?
        current_cmd_node = malloc(sizeof(cmd_node_t));
        current_cmd_node->pipe_count = -1; //assume we set -1 to new line command(?)
        current_cmd_node->next_node = NULL;
        insert_cmd_node(cmd_node_list, current_cmd_node);
    } else if(pre_state == STATE_FILE || pre_state == STATE_ARGS || pre_state == STATE_CMD) {
        insert_cmd_node(cmd_node_list, current_cmd_node);
    }
}



int _next_state(int state, int next_token_type) {

    if(state == STATE_INIT) {
        if(next_token_type == CMD) {
            return STATE_CMD;
        } else if(next_token_type == NEW_LINE) {
            return STATE_NW;
        }
    } else if(state == STATE_CMD) {
        if(next_token_type == CMD) {
            return STATE_ARGS;
        } else if(next_token_type == PASS) {
            return STATE_FILE;
        } else if(next_token_type == NEW_LINE) {
            return STATE_NW;
        } else if(next_token_type == PIPE || next_token_type == PIPE_N) {
            return STATE_INIT;
        }
    } else if(state == STATE_ARGS) {
        if(next_token_type == CMD) {
            return STATE_ARGS;
        } else if(next_token_type == PIPE || next_token_type == PIPE_N) {
            return STATE_INIT;
        } else if(next_token_type == PASS) {
            return STATE_FILE;
        } else if(next_token_type == NEW_LINE) {
            return STATE_NW;
        }
    } else if(state == STATE_FILE) {
        if(next_token_type == PIPE || next_token_type == PIPE_N) {
            return STATE_INIT;
        } else if(next_token_type == NEW_LINE) {
            return STATE_NW;
        }
    }
    return -1;
}


void _insert_arg(arg_node_t** arg_list, arg_node_t* new_node) {
    if(*arg_list == NULL) {
        *arg_list = new_node;
        new_node->next_node = NULL;
    } else {
        arg_node_t* tmp_node = *arg_list;
        while(tmp_node->next_node != NULL) {
            tmp_node = tmp_node->next_node;
        }
        tmp_node->next_node = new_node;
        new_node->next_node = NULL;
    }
}
