/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include "csapp.h"

int echo(int connfd) 
{
    int n, exit_flag = 0; 
    char buf[MAXLINE] = {'\0',};
    char tmp_buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    if((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        //printf("server received %d bytes\n", n);
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
}
/* $end echo */

