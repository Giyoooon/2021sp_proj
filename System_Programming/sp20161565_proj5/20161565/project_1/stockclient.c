/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */
#include "csapp.h"


int main(int argc, char **argv) 
{
    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
	    fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
	    exit(0);
    }
    host = argv[1];
    port = argv[2];

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
    	Rio_writen(clientfd, buf, strlen(buf));
        //Fputs("client send message.\n",stdout);
        memset(buf, 0, MAXLINE);
	    Rio_readlineb(&rio, buf, MAXLINE);
        if(strcmp(buf, "exit\n") == 0) 
            break;
        for(int i=0;i<strlen(buf);i++){
            if(buf[i] == '!')
                buf[i] = '\n';
        }
	    Fputs(buf, stdout);
        
       // Fputs("client receive message.\n",stdout);
    }
    Close(clientfd); //line:netp:echoclient:close
    exit(0);
}
/* $end echoclientmain */
