/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"

int echo(int connfd);


int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];
    //Stock = fopen("stock.txt", "r");
    clock_t start, end;
    float result;
    //Fclose(Stock);
    if (argc != 2) {
	    fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(0);
    }
    
    listenfd = Open_listenfd(argv[1]);
    start = clock();
    init_DB();
    int maxfd=listenfd;
    fd_set fdmask, fdmask_backup;
    FD_ZERO(&fdmask);
    FD_ZERO(&fdmask_backup);
    FD_SET(listenfd,&fdmask);
    //printf("listenfd == %d\n", listenfd);
	while (1) {
        fdmask_backup=fdmask;
        Select(maxfd+1,&fdmask_backup,NULL,NULL,NULL);

        for(int i=0;i<=maxfd;i++){
            //printf("i = %d\n", i);
            if(FD_ISSET(i,&fdmask_backup)){
                if(i==listenfd){
                   // printf("client come here first\n");
                    clientlen = sizeof(struct sockaddr_storage); 
		            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		            Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
                    printf("Connected to (%s, %s)\n", client_hostname, client_port);
                    if(maxfd < connfd)
                        maxfd = connfd;
                    FD_SET(connfd, &fdmask);
                    fd_setNum++;
                }
                else{
                    if(echo(i) == 0){
                        FD_CLR(i, &fdmask);
                        Close(i);
                        fd_setNum--;
                    }
                }
            }
        }
        if(fd_setNum == 0){
            break;
        }
    }
    FILE* fp = fopen("stock.txt", "w");
    Write_DBtxt(stock_DB, fp);
    fclose(fp);
    end = clock();
    result = (float)(end - start)/CLOCKS_PER_SEC;
    printf("Total socket communication time : %.6f\n", result);
    exit(0);
}
/* $end echoserverimain */
