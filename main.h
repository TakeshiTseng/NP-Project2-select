#ifndef __MAIN_H__
#define __MAIN_H__


int get_global_fd(int from_id, int to_id);
void global_pipe(int from, int to, int pipe[2]);
int global_fd_exist(int from, int to);
void clean_client_global_fd(int id);

// for global fd
pipe_node_t* global_fd_list;
#endif
