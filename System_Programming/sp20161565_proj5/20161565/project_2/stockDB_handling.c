#include "csapp.h"

void init_DB(){
    int ID, amount, price;
    char line[1000];
    //stock_DB = NULL;
    stock_DB = NULL;
    FILE* fp = fopen("stock.txt", "r");
    while(fgets(line, 1000, fp) != NULL){
        sscanf(line,"%d %d %d",&ID, &amount, &price);
        //printf("%d %d %d\n", ID, amount, price);
        stock_DB = stock_stock(stock_DB, ID, amount, price);
    }
    fclose(fp);
   
    //ShowDB(stock_DB);
}

StockDB *stock_stock(StockDB *Node, int ID, int amount, int price){// stock.txt 읽어서 binary tree stock DB만들기 
    if(Node == NULL){
        StockDB *tmp;
        tmp = (StockDB*)malloc(sizeof(StockDB));
        tmp->id = ID;
        tmp->stock_num = amount;
        tmp->price = price;
        tmp->readcnt = 0;
        Sem_init(&tmp->mutex,0,1);
        tmp->left_stock = NULL;
        tmp->right_stock = NULL;
        Node = tmp;
    }
    else if(Node->id == ID){
        printf("error : Same ID\n ");
    }
    else if(Node->id < ID){
        Node->left_stock = stock_stock(Node->left_stock, ID, amount, price);
    }
    else {
        Node->right_stock = stock_stock(Node->right_stock, ID, amount, price);
    }
    return Node;
}

void Write_DBtxt(StockDB* Node,FILE* fd){
    if(Node != NULL){
        fprintf(fd, "%d %d %d\n",Node->id, Node->stock_num, Node->price);
        Write_DBtxt(Node->left_stock, fd);
        Write_DBtxt(Node->right_stock, fd);
    }
}

void do_update_stockDB(char* usrbuf){
    char tmpbuf[MAXLINE];
    char cmd[6], str_id[10], str_stocknum[100];
    char* str_ptr;
    int flag = 0;
    strcpy(tmpbuf, usrbuf);
    
    str_ptr = strtok(tmpbuf, " \n");
    strcpy(cmd, str_ptr);
    str_ptr = strtok(NULL, " ");
    strcpy(str_id, str_ptr);
    str_ptr = strtok(NULL, " ");
    strcpy(str_stocknum, str_ptr);
        
    //printf("%s",cmd);
    if(strcmp(cmd, "buy") == 0){
        
        Requset_buy(stock_DB, atoi(str_id),atoi(str_stocknum),&flag);
        if(flag == 1){
            strcpy(usrbuf, "[buy] success\n");
        }
        else if(flag == 0){
            strcpy(usrbuf, "Cannot find stock id\n");
        }
        else{
            strcpy(usrbuf, "Not enough left stock\n");
        }
    }
    else if(strcmp(cmd, "sell") == 0){
        Requset_sell(stock_DB, atoi(str_id), atoi(str_stocknum), &flag);
        if(flag == 1){
            strcpy(usrbuf, "[sell] success\n");
        }
        else {
            strcpy(usrbuf, "Cannot find stock id\n");
        }
    }
}

void ShowDB(StockDB* Node, char* tmp){
    if(Node != NULL){
        char temp[30];
		sprintf(temp,"%d %3d %-6d!",Node->id, Node->stock_num, Node->price);
        //printf("%d %d %d\n", Node->id, Node->price, Node->stock_num);
        //Rio_writen(connfd, temp, MAXLINE);
        strcat(tmp,temp);
        ShowDB(Node->left_stock,tmp);
        //printf("%d %d %d\n", Node->id, Node->price, Node->stock_num);
        ShowDB(Node->right_stock, tmp);     
    }
}

void Requset_buy(StockDB* Node, int id, int stock_num, int* flag){   
    if(Node != NULL){
        if(Node->id == id){// node가 같은 
            *flag = 1;
            P(&Node->mutex);
            if(Node->stock_num < stock_num){  
                *flag = -1;
            }
            else Node->stock_num -= stock_num;
            V(&Node->mutex);
            return;
        }
        Requset_buy(Node->left_stock, id, stock_num, flag);
        Requset_buy(Node->right_stock, id, stock_num, flag);
    }
}

void Requset_sell(StockDB* Node, int id, int stock_num, int* flag){   
    if(Node != NULL){
        if(Node->id == id){// node가 같은 
            *flag = 1;
            P(&Node->mutex);
            Node->stock_num += stock_num;
            V(&Node->mutex);
            return;
        }
        Requset_sell(Node->left_stock, id, stock_num, flag);
        Requset_sell(Node->right_stock, id, stock_num, flag);
    }
}