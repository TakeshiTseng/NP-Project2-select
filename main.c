#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include "server.h"
#include "main.h"
#include "passivesock.h"
#include "client_node.h"

char* welcome_message = "****************************************\r\n** Welcome to the information server. **\r\n****************************************";

void handleSIGCHLD() {
    int stat;

    /*Kills all the zombie processes*/
    while(waitpid(-1, &stat, WNOHANG) > 0);
    // while(wait3(&stat, WNOHANG, (struct rusage*)0)>=0);
}

int main(int argc, const char * argv[])
{

    // static fd
    dup2(0, 1000);
    dup2(1, 1001);
    dup2(2, 1002);

    // handling zombie processes
    signal(SIGCHLD, handleSIGCHLD);

    srand(time(0));

    struct sockaddr_in my_addr;
    struct sockaddr_in client_addr;
    int port = 2000 + rand() % 100;
    printf("Port : %d\n", port);
    char port_str[5];
    sprintf(port_str, "%d", port);
    int sc_fd = passivesock(port_str, "tcp", 5);

    printf("accepting.....\n");

    fd_set rfds;
    fd_set afds;

    int max_clients = 30;
    int nfds = getdtablesize();
    FD_ZERO(&afds);
    int addrlen = sizeof(client_addr);

    FD_SET(sc_fd, &afds);
    while(1) {
        memcpy(&rfds, &afds, sizeof(rfds));

        if(select(nfds, &rfds, NULL, NULL, NULL) < 0) {
            perror("Select error\n");
            exit(1);
        }

        if(FD_ISSET(sc_fd, &rfds)) {
            int new_client_sock = accept(sc_fd, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen);
            if(new_client_sock < 0) {
                perror("Accept error!\n");
                exit(1);
            }

            client_node_t* new_client_node;
            new_client_node = create_client_node(new_client_sock, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            if(insert_to_client_list(new_client_node) != -1) {
                dup2(new_client_sock, 1);
                printf("%s\r\n", welcome_message);
                printf("%% ");
                fflush(stdout);
                char user_in_prompt[1024];
                sprintf(user_in_prompt, "*** User '(no name)' entered from %s/%d. ***\r\n", new_client_node->ip, new_client_node->port);
                broad_cast(new_client_node, user_in_prompt);
                FD_SET(new_client_sock, &afds);
            } else {
                perror("client list full");
            }

        } else {
            int client_fd;
            for(client_fd = 0; client_fd<nfds; client_fd++) {
                if(client_fd != sc_fd && FD_ISSET(client_fd, &rfds)) {
                    client_node_t* client = find_client_node(client_fd);
                    if(client == NULL) {
                        perror("error client fd exist, but client node not exit!\n");
                        continue;
                    }
                    if(serve(client) == -1) {
                        // logout
                        FD_CLR(client->client_sc_fd, &afds);
                        remove_client_node(client);
                        int c;
                        for(c=0; c<3; c++) {
                            dup2(1000 + c, c);
                        }
                        if(fcntl(client->client_sc_fd, F_GETFD) != -1) {
                            close(client->client_sc_fd);
                        }
                    }
                    break;
                }
            }
        }
    }
    return 0;
}

int get_global_fd(int from_id, int to_id) {
    int res = -1;
    pipe_node_t* tmp = global_fd_list;
    if(tmp == NULL) {
        return -1;
    }
    if(tmp->from_user_id == from_id && tmp->to_user_id == to_id) {
        res = tmp->in_fd;
        global_fd_list = global_fd_list->next_node;
    } else {
        while(tmp->next_node != NULL) {
            if(tmp->next_node->from_user_id == from_id && tmp->next_node->to_user_id == to_id) {
                res = tmp->next_node->in_fd;
                tmp->next_node = tmp->next_node->next_node;
                break;
            }
            tmp = tmp->next_node;
        }

    }

    return res;
}

void global_pipe(int from, int to, int pipe_fd[2]) {
    pipe_node_t* new_node = malloc(sizeof(pipe_node_t));

    new_node->count = 1;
    int r = pipe(pipe_fd);
    if(r == -1) {
        perror("Global pipe error");
        return;
    }
    new_node->out_fd = pipe_fd[1];
    new_node->in_fd = pipe_fd[0];
    new_node->next_node = NULL;
    new_node->from_user_id = from;
    new_node->to_user_id = to;

    insert_pipe_node(&global_fd_list, new_node);
}

int global_fd_exist(int from_id, int to_id) {
    pipe_node_t* tmp = global_fd_list;
    while(tmp != NULL) {
        if(tmp->from_user_id == from_id && tmp->to_user_id == to_id) {
            return 1;
        }
        tmp = tmp->next_node;
    }
    return 0;
}

void clean_client_global_fd(int id) {
    pipe_node_t* tmp = global_fd_list;
    pipe_node_t* new_list = NULL;
    while(tmp != NULL) {
        if(tmp->from_user_id != id && tmp->to_user_id != id) {
            pipe_node_t* tmp_node = tmp;
            tmp = tmp->next_node;
            tmp_node->next_node = NULL;

            insert_pipe_node(&new_list, tmp_node);
        } else {
            tmp = tmp->next_node;
        }
    }

    global_fd_list = new_list;
}
