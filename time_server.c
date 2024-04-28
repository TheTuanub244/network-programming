
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>


void signalHandler(int signo) {
    pid_t pid = wait(NULL);
    printf("CHild process terminated, pid = %d\n", pid);
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket failed: ");
        return 1;
    }
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);


    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind failed: ");
        return 1;
    }


    if (listen(listener, 5)) {
        perror("listen failed: ");
        return 1;
    }


    char buf[256];

    signal(SIGCHLD, signalHandler);

    while (1) {
        printf("Cho client moi ket noi den...\n");
        int client = accept(listener, NULL, NULL);
        printf("Client moi ket noi den: %d\n", client);

        if (fork() == 0) {
            close(listener);

            while (1) {
                int ret = recv(client, buf, sizeof(buf), 0);
                if(ret<=0){
                    break;
                }
                buf[ret] = 0;
                printf("Received from %d: %s\n", client, buf);

                char comm[10], format[12], tmp[32];
                int a = sscanf(buf, "%s %s %s", comm, format, tmp);
                char repply_msg[32]; 
                if (a == 2) {
                    time_t rawtime;
                    struct tm *timeinfo;
                    time(&rawtime);
                    timeinfo = localtime(&rawtime); 
                    //kiem tra lenh
                    if (strcmp(format, "dd/mm/yyyy") == 0) {
                        strftime(repply_msg, sizeof(repply_msg), "%d/%m/%Y\n", timeinfo);
                    } else if (strcmp(format, "dd/mm/yy") == 0) {
                        strftime(repply_msg, sizeof(repply_msg), "%d/%m/%y\n", timeinfo);
                    } else if (strcmp(format, "mm/dd/yyyy") == 0) {
                        strftime(repply_msg, sizeof(repply_msg), "%m/%d/%Y\n", timeinfo);
                    } else if (strcmp(format, "mm/dd/yy") == 0) {
                        strftime(repply_msg, sizeof(repply_msg), "%m/%d/%y\n", timeinfo);
                    } else {
                        char* msg = "Sai lenh, hay nhap lai!\n";
                        send(client, msg, strlen(msg), 0);
                        continue;
                    }
                } else {
                    char* msg = "Sai cu phap, hay nhap lai!\n";
                    send(client, msg, strlen(msg), 0);
                    continue;
                }
                send(client, repply_msg, strlen(repply_msg), 0);
            }

            close(client);
            exit(0);
        }
        close(client);
    }

    return 0;
}
