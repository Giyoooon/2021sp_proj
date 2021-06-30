#include "20161565.h"

void Open_Opcode(){// "opcode.txt"를 연다.
	FILE* fp  = NULL;
	fp = fopen("opcode.txt","r"); // open file "opcode.txt"
	
	if(!fp){ // can't read '"opcode.txt" 
		printf("Cannot read \"opcode.txt\" file.\n");
		printf("File not exist or currpted.\n");
		return;
	}
	int opcode,idx;
	char mnemonic[10];
	char format[10];

	while(fscanf(fp,"%X %s %s\n",&opcode, mnemonic, format)!=EOF){// "opcode.txt"에서 한줄씩 scan
		Opcode_table* pNew = (Opcode_table *)malloc(sizeof(Opcode_table));
		// create new node
		pNew->link = NULL;
		pNew->value = opcode;
		strcpy(pNew->key, mnemonic);
		strcpy(pNew->opcode_format, format);
		
		idx = create_hashfunction(mnemonic);// 입력받은 mnemonic에 해당하는 hash function 생성
			
		gen_Opcode_table(pNew,idx); // opcode hash table 구성
	}
	fclose(fp); // close "opcode.txt"
};

int create_hashfunction(char* mnemonic){// 입력받은 mnemonic에 해당하는 hasg function 생성 
	// add all character in mnemonic string
	// return mod this by 20 
	int hash = 0,i;
	
	for(i = 0; i< strlen(mnemonic);i++){
		hash += (int) mnemonic[i];
	}

	return hash % 20;

}
void gen_Opcode_table(Opcode_table *pNew,int idx){// opcode hash table 을 구성한다.
	//hash function에 해당하는 index에 opcode, mneminic 저장
	
	if(opTable[idx] == NULL){
		opTable[idx] = pNew;
	}

	else{
		Opcode_table* pWalk;
		pWalk = opTable[idx];

		while(pWalk->link != NULL){
			pWalk = pWalk->link;
		}
		pWalk->link = pNew;
	}
};
 
int cmd_Opcode_mnemonic(char input[100][100],int tok_num){// opcode mnemonic 명령어를 처리한다.
	// if input start with "opcode" and next input is mnemonic
	// find mnemonic in opTable and if mnemonic exist, print matchde opcode.
	char mnemonic[100];
	int flag;
	if(tok_num != 2){//invalid args
		return -1;
	}
	
	strcpy(mnemonic, input[1]);
	flag = find_Opcode_mnemonic(mnemonic);//입력받은  mnemonic과 일치하는 opcode return

	if( flag == -1){
		printf("\"%s\"is not exist in Opcode table.\n",input[1]);
		return 0;
	}
	else{
		return 1;	
	}

};

int find_Opcode_mnemonic(char* mnemonic){//opcode hash table에서 입력받은  mnemonic 과 일치하는 opcode return 
	// find Opcode_mnemonic
	// if find input mnemonic in Opcode_hash_table, return 0.
	// if not print caution message and return 1.
	int i;
	Opcode_table* pFind;

	for(i = 0; i< 20 ; i++){
		pFind  = opTable[i];
		
		while(pFind != NULL){
			if(strcmp(pFind->key, mnemonic) == 0){
				printf("opcode is %X\n",pFind->value);
				return 1;
			}
			pFind = pFind->link;
		}
	}
	// if input mnemonic can not found in opTable.
	return -1;
}
int print_Opcode_list(int tok_num){// opcodelist 명령어를 처리한다
	//print opTable
	if(opTable == NULL){//opcode hash table이 없는 경우
		printf("Opcode list not exist. Check your file.\n");
		return 0;
	}
	if(tok_num != 1){//invalid args
		return -1;
	}
	int i;
	Opcode_table *pMove;
	for(i = 0; i<20;i++){
		pMove = opTable[i];
		printf("%2d : ",i);
		if(pMove == NULL){ 
			printf("\n");
			continue;
		}
		while(pMove->link != NULL){
			printf("[%s, %X] -> ",pMove->key, pMove->value);
			pMove = pMove->link;
		}
		printf("[%s %X]\n",pMove->key, pMove->value);
	}
	return 1;
};
