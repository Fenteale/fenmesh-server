//######################################################
// Fenmesh Server
//######################################################

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "log.h"

#define PORTNUM     6017

// https://nullraum.net/how-to-create-a-daemon-in-c/

static void setup() {
    int socket_d = socket(AF_INET, SOCK_STREAM, 0); //socket() retuns a file descriptor of a socket to use
    // a lot of these old posix commands have a ton of esoteric parameters.
    // as far as I can tell, we never want to change the parameters above

    if (socket_d < 0) {
        logWrite("Error opening socket: %s", strerror(errno));
        exit(1); //exit with error
    }

    if (setsockopt(socket_d, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        logWrite("Error configuring socket: %s", strerror(errno));
    }
    /*if (setsockopt(socket_d, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0) {
        fprintf(stderr, "Error configuring socket: %s\n", strerror(errno));
    }*/
    
    struct sockaddr_in server_a, client_a;
    server_a.sin_family = AF_INET; // no idea tbh, dont touch this
    server_a.sin_addr.s_addr = INADDR_ANY; // bind to any available IP
    server_a.sin_port = htons(PORTNUM); // establish which port to open on

    //try to bind to the socket officially
    if (bind(socket_d, (struct sockaddr *) &server_a, sizeof(server_a)) < 0) {
        logWrite("Error binding socket: %s", strerror(errno));
        exit(1);
    }

    // let clients connect
    listen(socket_d, 5);

    int client_d, read_size;
    socklen_t client_len = sizeof(client_a);
    //printf("Ready and waiting for clients.\n");
    logWrite("Ready and waiting for clients.");

    while(1) {
        //try to accept a client's connection
        client_d = accept(socket_d, (struct sockaddr *) &client_a, &client_len);
        if(client_d >= 0) {
            logWrite("Client detected at %s", inet_ntoa(client_a.sin_addr)); //use the info from the client address struct to print out the connection info

            close( client_d ); //close connection after that
 
            logWrite("Client disconnected.");
        }
    }
}

static void daemonize() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        printf("Daemonization failed!\n");
        exit(EXIT_FAILURE);
    }
    
    if (pid > 0)
        exit(EXIT_SUCCESS);

    if (setsid() < 0)
        exit(EXIT_FAILURE);
    
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);
    
    if (pid > 0)
        exit(EXIT_SUCCESS);

    umask(0);

    chdir("/");

    int x;
    for(x = sysconf(_SC_OPEN_MAX); x>=0; x--)
        close(x);
    
    //openlog("fenmesh-server", LOG_PID, LOG_DAEMON);
    //openLog();

}

int main(int argc, char *argv[]) {
    int option;
    int toDaemonize = 0;
    while((option = getopt(argc, argv, "d")) != -1){
        switch(option) {
            case 'd':
                printf("Launching as Daemon.\n");
                toDaemonize = 1;
                break;
            default:
                printf("Unknown option %c\n", option);
                break;
        }
    }
    if(toDaemonize) {
        openLog(1);
        daemonize();
    }
    else {
        openLog(0);
    }

    setup();

    
    logWrite("Fenmesh-server shutting down.");
    closeLog();
    

    return EXIT_SUCCESS;
}