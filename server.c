#include <sys/wait.h>
#include "server.h"
#include "cmd_node.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "util.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "node_list.h"
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "tokenizer.h"
#include "pipe_node.h"
#include "client_node.h"

void serve(client_node_t* client) {

    // replace stdin, stdout, stderr to client fd
    close(0);
    close(1);
    close(2);
    dup(client->client_sc_fd);
    dup(client->client_sc_fd);
    dup(client->client_sc_fd);

    int c;
    for(c=0; c<client->num_env; c++) {
        setenv(client->env[c], client->env_val[c], 1);
    }

    cmd_node_t* cmd_node_list = NULL;
    parse_tokens(&cmd_node_list);
    int state;

    if(cmd_node_list != NULL) {
        cmd_node_t* last_node = cmd_node_list;
        while(last_node->next_node != NULL) {
            if(last_node->next_node->pipe_count == -1) {
                last_node->next_node = NULL;
                break;
            }
            last_node = last_node->next_node;
        }

        if(last_node->pipe_from_user == 1) {
            // add pipe to here
            // FIXIT
            int user_id = last_node->user_id;

            // XXX
            // get_global_fd(from_user_id, to_user_id)
            int in_fd = get_global_fd(user_id, client->id);
            pipe_node_t* f_usr_node = malloc(sizeof(pipe_node_t));
            f_usr_node->count = 1;
            f_usr_node->out_fd = -1;
            f_usr_node->in_fd = in_fd;
            f_usr_node->next_node = client->pipe_list;
            client->pipe_list = f_usr_node;

            // remove node from cmd list
            cmd_node_t* tmp_node = cmd_node_list;
            while(tmp_node->next_node != NULL) {
                if(tmp_node->next_node->pipe_from_user == 1) {
                    tmp_node->next_node = tmp_node->next_node->next_node;
                    break;
                }
                tmp_node = tmp_node->next_node;
            }
        }

        // process commands
        // despatch node to right palce
        while(cmd_node_list != NULL) {
            cmd_node_t* node_to_exec = pull_cmd_node(&cmd_node_list);
            if(node_to_exec->pipe_count == -1) {
                break;
            }
            int errnum = exec_cmd_node(node_to_exec, client);
            if(errnum == -1) {
                // command not found
                printf("Unknown command: [%s].\n", node_to_exec->cmd);
                fflush(stdout);
                free_cmd_list(&cmd_node_list);
            } else if(errnum == -2) {
                // pipe to me not exist
                printf("*** Error: the pipe #%d->#%d does not exist yet. ***\n", node_to_exec->user_id, client->id);
                fflush(stdout);
            } else if(errnum == -3) {
                // pipe to other exist
                printf("*** Error: the pipe #%d->#%d already exists. ***\n", client->id, node_to_exec->user_id);
                fflush(stdout);
            }
        }
    }
    printf("%% ");
    fflush(stdout);
}

int exec_cmd_node(cmd_node_t* cmd_node, client_node_t* client) {

    int pipe_count = cmd_node->pipe_count;
    int pid = -1;
    int input_pipe_fd = -1;
    int output_pipe_fd = -1;

    if(strcmp(cmd_node->cmd, "printenv") == 0) {
        char* env_name = cmd_node->args[1];
        if(env_name != NULL) {
            char* env_val = getenv(env_name);
            printf("%s=%s\n", env_name, env_val);
            fflush(stdout);
        }
        return 0;
    } else if(strcmp(cmd_node->cmd, "setenv") == 0) {
        char* env_name = cmd_node->args[1];
        char* env_val = cmd_node->args[2];
        setenv(env_name, env_val, 1);
        return 0;
    } else if(strcmp(cmd_node->cmd, "exit") == 0) {
        exit(0);
    } else if(strcmp(cmd_node->cmd, "who") == 0) {

    } else if(strcmp(cmd_node->cmd, "tell") == 0) {

    } else if(strcmp(cmd_node->cmd, "yell") == 0) {

    } else if(strcmp(cmd_node->cmd, "name") == 0) {

    }

    decrease_all_pipe_node(client->pipe_list);

    // get this process input source
    pipe_node_t* in_pipe_node = find_pipe_node_by_count(client->pipe_list, 0);
    if(in_pipe_node != NULL) {
        input_pipe_fd = in_pipe_node->in_fd;
        close(in_pipe_node->out_fd);
        in_pipe_node->count--;
    }

    // get this process output source
    pipe_node_t* out_pipe_node = find_pipe_node_by_count(client->pipe_list, pipe_count);
    if(out_pipe_node != NULL) {
        output_pipe_fd = out_pipe_node->out_fd;
    } else if(cmd_node->pipe_to_file == 1){
        output_pipe_fd = get_file_fd(cmd_node->filename);
    } else if(cmd_node->pipe_count != 0) {
        int new_pipe_fd[2];
        pipe(new_pipe_fd);
        out_pipe_node = malloc(sizeof(pipe_node_t));
        out_pipe_node->count = cmd_node->pipe_count;
        out_pipe_node->in_fd = new_pipe_fd[0];
        out_pipe_node->out_fd = new_pipe_fd[1];
        out_pipe_node->next_node = NULL;
        insert_pipe_node(&(client->pipe_list), out_pipe_node);

        output_pipe_fd = new_pipe_fd[1];
    }


    pid = fork();
    if(pid == 0) {
        if(input_pipe_fd != -1) {
            // not use stdin
            close(0);
            dup(input_pipe_fd);
            close(input_pipe_fd);
        }

        // out
        if(out_pipe_node != NULL) {
            close(1);
            dup(out_pipe_node->out_fd);
            close(out_pipe_node->out_fd);
        } else if(cmd_node->pipe_to_file == 1) {
            close(1);
            dup(output_pipe_fd);
            close(output_pipe_fd);
        }

        execvp(cmd_node->cmd, cmd_node->args);
        exit(-1);

    } else if(pipe_count != 0) {
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status) && WEXITSTATUS(status) != 0){
            inscrease_all_pipe_node(client->pipe_list);
            return -1;
        } else {
            if(input_pipe_fd != -1) {
                close(input_pipe_fd);
            }
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status) && WEXITSTATUS(status) != 0){
            inscrease_all_pipe_node(client->pipe_list);
            return -1;
        } else {
            if(in_pipe_node != NULL) {
                close(in_pipe_node->out_fd);
                in_pipe_node->count--;
            }
            close_unused_fd();
        }
    }
    return 0;
}

int get_file_fd(char* filename) {
    return open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IROTH);
}

void close_unused_fd(client_node_t* client) {
    pipe_node_t* new_list = NULL;
    while(client->pipe_list != NULL) {
        pipe_node_t* tmp_node = client->pipe_list;
        client->pipe_list = client->pipe_list->next_node;
        tmp_node->next_node = NULL;

        if(tmp_node->count <= 0) {
            if(fcntl(tmp_node->out_fd, F_GETFD) != -1) {
                close(tmp_node->out_fd);
            }
            if(fcntl(tmp_node->in_fd, F_GETFD) != -1) {
                close(tmp_node->in_fd);
            }
        } else {
            insert_pipe_node(&new_list, tmp_node);
        }
    }

    client->pipe_list = new_list;
}


