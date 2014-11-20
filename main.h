#ifndef __MAIN_H__
#define __MAIN_H__

char* welcome_message = "****************************************\n** Welcome to the information server. **\n****************************************";

int get_global_fd(int from_id, int to_id);
void global_pipe(int from, int to, int pipe[2]);
// for global fd

pipe_node_t* global_fd_list;
#endif
