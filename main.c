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
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset((void*)&(my_addr.sin_zero), 0, 8);

    printf("creating...\n");
    int sc_fd = -1;
    sc_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(sc_fd == -1) {
        printf("failed\n");
        return -1;
    }

    printf("binding....\n");
    int r = bind(sc_fd, (const struct sockaddr*)&my_addr, sizeof(struct sockaddr));

    if(r == -1) {
        printf("failed\n");
        return -1;
    }


    printf("listening....\n");
    r = listen(sc_fd, 10);
    if(r == -1) {
        printf("failed\n");
        return -1;
    }

    printf("accepting.....\n");
    while(1){
        size_t addr_size = sizeof(struct sockaddr_in);
        int client_sc_fd = accept(sc_fd, (struct sockaddr*)&client_addr, &addr_size);
        printf("Accepted\n");

        if(client_sc_fd == -1) {
            printf("failed\n");
            continue;
        }

        int child_p = fork();

        if(child_p == 0) {
            serve(client_sc_fd);
            close(client_sc_fd);
            close(sc_fd);
            return 0;
        } else {
            int status;
            int wpid = wait(&status);
            printf("wait pid %d, status %d\n", wpid, status);
            close(client_sc_fd);
        }
    }

    close(sc_fd);

    return 0;
}

