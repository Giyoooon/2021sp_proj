#include "20161565.h"

int cmd_help(int tok_num){// h[elp] 명령어를 처리한다.
	if(tok_num != 1){//invalid arg
		return -1;
	}
	//valid command
	printf("\t\th[elp]\n");
	printf("\t\td[ir]\n");
	printf("\t\tq[uit]\n");
	printf("\t\thi[story]\n");
	printf("\t\tdu[mp] [start, end]\n");
	printf("\t\te[dit] address, value\n");
	printf("\t\tf[ill] start, end, value\n");
	printf("\t\treset\n");
	printf("\t\topcode mnemonic\n");
	printf("\t\topcodelist\n");
	return 1; 
};

int cmd_dir(int tok_num){// d[ir] 명령어를 처리한다.
	if(tok_num != 1){// invalid arg
		return -1;
	}
	int i = 1;
	DIR* cur_dir = NULL;
	struct dirent* dir_name;
	struct stat file_info;
	cur_dir = opendir(".");	// 현재 directory open
	
	if(NULL != cur_dir){
		while((dir_name = readdir(cur_dir))!= NULL){
			printf("%25s",dir_name->d_name);

			stat(dir_name->d_name, &file_info);

			switch(file_info.st_mode & S_IFMT){
				case S_IFREG://일반 file 인 경우
					printf("*");
					break;
				case S_IFDIR://directory 인 경우
					printf("/");
					break;
			}
			if(i % 3 == 0){
				printf("\n");
				i %= 3;
			}
			i++;
		}	
	}
	else{// 현재 directory에 어떤 file, directroy 도 존재하지 않는 경우
		printf("There is no files or directories in this directory.\n");
	}

	closedir(cur_dir);// directory close

	printf("\n");
	return 1;
};

int cmd_quit(int tok_num){
	//free linked list
	// history, opcode hash Table
	if(tok_num != 1){// invalid arg
		return -1;
	}
	
	HISTORY* pRemove;

	while(history_head != NULL){
		
		pRemove = history_head;
		//
		//printf("%d\n",pRemove->num);
		//
		history_head = history_head->link;
		free(pRemove);
	}
	//linked list : history free
	
	//opcode_hash_table
	int i;
	Opcode_table* opFree, *contents;
	
	for(i = 0; i<20; i++){
		contents = opTable[i];
		
		while(contents != NULL){
			opFree = contents;
			contents = contents->link;
			free(opFree);
		}
		free(contents);
	}
	//linked list : opcode hash table free
	free(memory);
	// 1MB memory free
	return 1;
};

int cmd_history(char* input_str, int tok_num){// hi[story] 명령어를 처리한다.

	if(tok_num != 1){//invalid arg
		return -1;
	}
	add_history(input_str); //입력된 hi[story] 또한 history linked list 에 저장
	
	HISTORY *pWalk;
	pWalk = history_head;
	while(pWalk != NULL){
		printf("\t%-4d %s\n",pWalk->num,pWalk->command);
		pWalk = pWalk->link;
	}

	return 1;
};

void add_history(char *input_command){// valid 한 input string 을 history_head 에 push
	HISTORY* pNew = (HISTORY *)malloc(sizeof(HISTORY));
	HISTORY* pMove;

	strcpy(pNew->command, input_command);
	pNew->link = NULL;
	pNew->num = 1;

	if(!history_head){
		history_head = pNew;
		return;
	}
	pMove = history_head;
	
	while(pMove->link != NULL){
		pMove = pMove->link;	
	}
	
	pMove->link = pNew;
	pNew->num += pMove->num;
};
