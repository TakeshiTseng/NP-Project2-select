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

void handleSIGCHLD() {
    int stat;

    /*Kills all the zombie processes*/
    while(waitpid(-1, &stat, WNOHANG) > 0);
    // while(wait3(&stat, WNOHANG, (struct rusage*)0)>=0);
}

int main(int argc, const char * argv[])
{

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

            printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_client_sock, inet_ntoa(client_addr.sin_addr) , ntohs(client_addr.sin_port));
            if(insert_to_client_list(new_client_node) != -1) {
                dup2(new_client_sock, 1);
                printf("%s\n", welcome_message);
                printf("%% ");
                fflush(stdout);
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
                    serve(client);
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
    while(tmp != NULL) {
        if(tmp->from_user_id == from_id && tmp->to_user_id == to_id) {
            res = tmp->in_fd;
        }
        tmp = tmp->next_node;
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

