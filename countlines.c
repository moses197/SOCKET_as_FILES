#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define EXIT_FAILURE 1
#define SERVER_PORT 80

#define MAXLINE 4096
#define SA struct sockaddr

void err_n_die(const char *fmt);


// return a socket for an open connection to the server
int http_get(char *address, short port) {
    int sockfd;
    int sendbytes;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_n_die("Error while creating the socket!");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if ((inet_pton(AF_INET, address, &servaddr.sin_addr)) <= 0)
        err_n_die("inet_pton error for  ");

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
        err_n_die("connect failed!");

    // We're connected, prepare the message
    sprintf(sendline, "GET / HTTP/1.1\r\nConnection: close\r\n\r\n");
    sendbytes = strlen(sendline);

    // Send the message -- making sure you send it all
    //This code is a bit fragile, since it bails if only some of the bytes are
    // sent, normally, you would want to retry, unless the return value was -1
    if (send(sockfd, sendline, sendbytes, 0) != sendbytes)
        err_n_die("wriet error");

    return sockfd;
}

void print_usage(char *progname) {
    printf("usage: \n");
    printf("\t%s http <http ip address>\n", progname);
    printf("\t%s file <filename>\n", progname);
    printf("\t%s stdin\n", progname);
}

int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    char *src = argv[1];
    FILE *input_source = NULL;

    if (strcmp(src, "stdin") == 0) {
        input_source = stdin;
    }else if (strcmp(src, "file") == 0) {
        input_source = fopen(argv[2], "r"); // FILE *pointer
    }else if (strcmp(src, "http") == 0) {
        int socket = http_get(argv[2], SERVER_PORT);
        input_source = fdopen(socket, "r"); // FILE *pointer
    } else {
        printf("error: invalid source: %s\n", src);
        return EXIT_FAILURE;
    }

    if (input_source == NULL) {
        printf("error: could not open source\n");
        return EXIT_FAILURE;
    }

    // read all the lines from source
    char buffer[MAXLINE];
    int num_lines = 0;
    while (!feof(input_source)) {
        if (fgets(buffer, MAXLINE, input_source)) {
            num_lines++;
        }
    }
    fclose(input_source);
    printf("we read %d lines\n", num_lines);
}

void err_n_die(const char *fmt) {
    printf("%s\n", fmt);
}