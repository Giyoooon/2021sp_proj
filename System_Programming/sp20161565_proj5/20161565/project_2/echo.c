/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include "csapp.h"

static int byte_cnt;
static sem_t mutex;

static void init_echo_cnt(void){
    Sem_init(&mutex,0,1);
    byte_cnt=0;
}
void echo_cnt(int connfd)
{
    int n;
    char buf[MAXLINE];
    rio_t rio;
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    Pthread_once(&once, init_echo_cnt);
    Rio_readinitb(&rio, connfd);
    //printf("echo!\n");
    //printf("thread %d\n", (int) pthread_self());   
    while((n = Rio_readlineb(&rio,buf,MAXLINE)) != 0) { 
        P(&mutex);    
        byte_cnt += n;
        printf("thread %d received %d (%d total) bytes on fd %d\n",(int) pthread_self(),n, byte_cnt, connfd);
        V(&mutex);
        
        if(strcmp(buf, "show\n") == 0){
            char tmp_buf[MAXLINE] = {0, };
            //memset(tmp_buf, '\0', MAXLINE);
            P(&stock_DB->mutex);
            ShowDB(stock_DB, tmp_buf);
            V(&stock_DB->mutex);
            tmp_buf[strlen(tmp_buf)-1] = '\n';
            strcpy(buf, tmp_buf);
        }
        else if(strcmp(buf, "exit\n") == 0){
            Rio_writen(connfd, buf, n);
            return;
        }
        else if(strcmp(buf, "\n") == 0){}
        else{
            //printf("BUF : %s", buf);
            do_update_stockDB(buf);
        }    
        n = strlen(buf);
        //printf("%s",buf);
        
        Rio_writen(connfd, buf, n);
    }
    //printf("EchO!\n");
}
/*
int echo(int connfd) 
{
    int n, exit_flag = 0; 
    char buf[MAXLINE] = {'\0',};
    char tmp_buf[MAXLINE];
    rio_t rio;
    static pthread_t
    Rio_readinitb(&rio, connfd);
    if((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %d bytes\n", n);
        //strcpy(tmp_buf, do_update_stockDB(buf));
        //printf("connfd == %d\n",connfd);
        if(strcmp(buf, "show\n") == 0){
            memset(tmp_buf, '\0', MAXLINE);
            ShowDB(stock_DB, tmp_buf);
            tmp_buf[strlen(tmp_buf)-1] = '\n';
            strcpy(buf, tmp_buf);
        }
        else if(strcmp(buf, "exit\n") == 0){
            FILE* fp;
            fp = fopen("stock.txt", "w");
            Write_DBtxt(stock_DB, fp);
            fclose(fp);
            exit_flag =1;
            //Close(connfd);
            //Rio_writen(connfd, buf, MAXLINE);
        }
        else if(strcmp(buf, "\n") == 0){}
        else{
            do_update_stockDB(buf);
        }    
        n = strlen(buf);
    	Rio_writen(connfd, buf, n);
    }
    if(exit_flag == 1) return 0;
    return n;
}*/

/* $end echo */


