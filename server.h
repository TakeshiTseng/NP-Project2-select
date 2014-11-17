#ifndef __SERVER_H__
#define __SERVER_H__

#include "cmd_node.h"
#include "node_list.h"
#include "pipe_node.h"

int exec_cmd_node(cmd_node_t* cmd_node);
int is_cmd_exist(char* cmd, char* env_path);
void serve(int client_fd);
void remove_from_chain(node_t* node_to_remove);
int get_file_fd(char* filename);
void remove_unused_nodes();
void close_unused_fd();
cmd_node_t* cmd_exec_list;
pipe_node_t* pipe_list;
#endif
