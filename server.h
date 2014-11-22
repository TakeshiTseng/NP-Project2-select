#ifndef __SERVER_H__
#define __SERVER_H__

#include "cmd_node.h"
#include "node_list.h"
#include "pipe_node.h"
#include "client_node.h"

int exec_cmd_node(cmd_node_t* cmd_node, client_node_t* client);
int is_cmd_exist(char* cmd, char* env_path);
int serve(client_node_t* client);
void remove_from_chain(node_t* node_to_remove);
int get_file_fd(char* filename);
void remove_unused_nodes();
void close_unused_fd();
int is_this_pipe_needed_by_someone(int fd);
void handle_from_user_node(cmd_node_t** cmd_list);
#endif
