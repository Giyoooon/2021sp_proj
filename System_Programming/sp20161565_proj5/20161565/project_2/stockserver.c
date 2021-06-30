/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"
#define NTHREADS 100
#define SBUFSIZE 20

//int echo(int connfd);
void echo_cnt(int connfd);
//static void init_echo_cnt(void);
void *thread(void *vargp);

sbuf_t sbuf;
int maxfd, exit_flag;
sem_t fileopen;
int main(int argc, char **argv) 
{
    int i, listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];
    //char buf[MAXLINE];
    pthread_t tid;
    //Stock = fopen("stock.txt", "r");
    time_t start, end;
    //Fclose(Stock);
    if (argc != 2) {
	    fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(0);
    }
    init_DB();
    listenfd = Open_listenfd(argv[1]);
    
    //printf("listenfd == %d\n", listenfd);
    Sem_init(&fileopen, 0 , 1);
	sbuf_init(&sbuf, SBUFSIZE);
    
    for(i = 0; i < NTHREADS; i++)
        Pthread_create(&tid, NULL, thread, NULL);

    while (1) {
        clientlen = sizeof(struct sockaddr_storage); 
	    //connfdp = Malloc(sizeof(int)); 
        //*connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        //Pthread_create(&tid, NULL, thread, connfdp);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        //printf("slot : %d items: %d\n", (int) sbuf.slots, (int) sbuf.items);
        P(&fileopen);
        exit_flag = 0;
        maxfd++;
        V(&fileopen);
        sbuf_insert(&sbuf, connfd);
    }
    
    exit(0);
}

void *thread(void *vargp){
    //int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    //Free(vargp);
    //while (echo(connfd) != 0);
    //Close(connfd);
    while(1){
        int connfd = sbuf_remove(&sbuf);
        //printf("connfd %d\n", connfd);
       
        echo_cnt(connfd);
        Close(connfd);
        P(&fileopen);
        maxfd--;
        if(maxfd == 0){
            FILE* fp = fopen("stock.txt", "w");
            Write_DBtxt(stock_DB,fp);
            fclose(fp);
        }
        V(&fileopen);
    }
};
/* $end echoserverimain */