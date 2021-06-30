#include "20161565.h"

int pass1(FILE* asm_file){ // process for pass1
	char asm_tok[100][100]; //store tokenize file line 
	char one_line[100], tmp_line[100];//file line string
	char* token; //token string
	int asmToken; // number of tokenize string
	int LOCCTR = 0; //Location counter
	int label_flag = 0; // check if line have label
	
	while(fgets(one_line, 100, asm_file) != NULL){// read one line in asm ifle 
		
		if(strlen(one_line) != 0){//enter 입력제외
			one_line[strlen(one_line)-1] = '\0';
			strcpy(tmp_line,one_line);
		}
		strcpy(lstArr[lst_idx].line, one_line);
		//parsing
		asmToken = 0;
		token = strtok(tmp_line,", \t");
		while(token != NULL){
			strcpy(asm_tok[asmToken], token);
			asmToken++;
			token = strtok(NULL,", \t");
		}
		
		if(asmToken == 0){//all blank
			lstArr[lst_idx].Loc_counter = -1;
			lstArr[lst_idx].Objcode = -1;
			lstArr[lst_idx].label_flag = 2;
			lst_idx++;
			continue;	
		}
		
		switch(one_line[0]){
			case '.': //주석
				label_flag = -1;
				lstArr[lst_idx].label_flag = label_flag;
				break;
			case ' ': case '\t': // no label
				label_flag = 0;
				lstArr[lst_idx].label_flag = label_flag;
				break;
			default://has label 
				label_flag = 1;
				lstArr[lst_idx].label_flag = label_flag;
				break;
		}
		if(label_flag == -1){//주석
			lstArr[lst_idx].Loc_counter = -1;
			lstArr[lst_idx].Objcode = -1;
			lst_idx++;
			continue;
		}
		else if(label_flag == 1){//if have label, store in symtab
			int sym_flag = add_symTab(asm_tok,asmToken,LOCCTR);
			if(sym_flag == -1){
				printf("Line %3d : \"%s\" duplicate symbol.\n", 5 + lst_idx * 5, asm_tok[0]);
			}
		}
		//assembly directive 
		//"START"
		if(st_flag == 0){//first line
			if((strcmp(asm_tok[label_flag],"START") == 0)){//OPCODE"START" 
				Starting_address = atoi(asm_tok[label_flag + 1]);
				lstArr[lst_idx].Loc_counter = Starting_address;
				lstArr[lst_idx].Objcode = -1;
				LOCCTR = Starting_address;
				strcpy(programName,asm_tok[0]);
				lst_idx++;
				st_flag = 1;
				continue;
			}
			// OPCODE "START" 미존재시 Starting address에 0 저장 후 LOCCTR 에 Starting address 저장
			LOCCTR = Starting_address = 0;
			st_flag = -1;
		}
		
		//"BASE"
		if(strcmp(asm_tok[label_flag],"BASE") == 0){
			lstArr[lst_idx].Loc_counter = -1;
			lstArr[lst_idx].Objcode = -1;
			BASE_flag = 1;
			BASE_line_idx = lst_idx;
			strcpy(BASE_name,asm_tok[label_flag+1]);
		}
		//"BYTE"
		else if(strcmp(asm_tok[label_flag],"BYTE") == 0){
			lstArr[lst_idx].Loc_counter = LOCCTR;
			lstArr[lst_idx].Objcode = 0;
			
			if(asm_tok[label_flag+1][0] == 'X'){
				LOCCTR += 1;
			}
			else if(asm_tok[label_flag+1][0] == 'C'){
				LOCCTR += strlen(asm_tok[label_flag+1])-3; // C , ',' 제외
			}
		}
		//"WORD"
		else if(strcmp(asm_tok[label_flag],"WORD") == 0){
			lstArr[lst_idx].Loc_counter = LOCCTR;
			lstArr[lst_idx].Objcode = 0;
			
			LOCCTR +=3;
		}
		//"RESB"
		else if(strcmp(asm_tok[label_flag],"RESB") == 0){
			lstArr[lst_idx].Loc_counter = LOCCTR;
			lstArr[lst_idx].Objcode = -1;
			LOCCTR += atoi(asm_tok[label_flag +1]);
		}
		//RESW
		else if(strcmp(asm_tok[label_flag],"RESW") == 0){
			lstArr[lst_idx].Loc_counter = LOCCTR;
			lstArr[lst_idx].Objcode = -1;
			
			LOCCTR += 3 * atoi(asm_tok[label_flag + 1]);
		}	
		//END
		else if(strcmp(asm_tok[label_flag],"END") == 0){
			lstArr[lst_idx].Loc_counter = -1;
			lstArr[lst_idx].Objcode = -1;
			
			End_flag = 1;
			pLength = LOCCTR - Starting_address;
			lst_idx++;
			break;
		}
		//opcode mnemonic
		else{
			char opcd[100];
			int ext_flag =0;
			int format;
			if(asm_tok[label_flag][0] == '+'){//extended mode
				ext_flag = 2;
				strcpy(opcd, asm_tok[label_flag] + 1);
			}
			else strcpy(opcd,asm_tok[label_flag]);
			
			Opcode_table* opFind = find_OpcodeNode(opcd);

			if(opFind == NULL){ //cannot find opcode mnemonic in opcode hash table
				//error message
				printf("line %3d : Invalid opcode mnemonic \"%s\".\n",5 * lst_idx + 5, asm_tok[label_flag]);
				return 0;
			}

			lstArr[lst_idx].Loc_counter = LOCCTR;
			lstArr[lst_idx].Objcode = 0;		
			format = (int) opFind->opcode_format[ext_flag] - '0';
			// opcode node 에 저장된 정보 -> lstNode
			LOCCTR += format;
		}
	
		lst_idx++;
		
	}
	return 1;
}

int pass2(){ // process for pass2
	int asmToken = 0; //number of tokenize string
	char asm_tok[100][100]; //store tokenize string 
	char tmp_line[100], tmp_str[100]; //store temp string 
	char OPCODE[100]; // OPCODE string
	char OPERAND[100]; // OPERAND string
	char *token; //tokenize  string

	int idx; // index
	int n ,i, x, b, p, e, opHex1,opHex2,disp; // bit n,i,x,b.p,e and opHex1,2 for store opcode ,and disp address
	int address_loc, tmpValue; // address_loc for symbol label
	long long Cal_ObjCode = 0; // store calculated objccode
	long long nixbpe = 0; // store nixbpe bits
	
	for(idx = 0; idx < lst_idx; idx++){
		if(lstArr[idx].Objcode == -1){ //dosen't need to calculate objcode
			continue;
		}
		//init
		n = i = x = b = p = e = opHex1 = opHex2 = disp = 0;
		
		strcpy(tmp_line,lstArr[idx].line);
		
		// tokenize string
		asmToken = 0;
		token = strtok(tmp_line,", \t");
		while(token != NULL){
			strcpy(asm_tok[asmToken], token);
			asmToken++;
			token = strtok(NULL,", \t");
		}
		
		for(int k = 0; k < 100 ;k++){//init2
				asm_tok[asmToken][k] = 0;
		}

		int idx_label = lstArr[idx].label_flag;
		if(strcmp(asm_tok[idx_label],"BYTE") == 0){//byte
			
			if(asm_tok[idx_label + 1][0] == 'C'){// store char
				strcpy(tmp_str, asm_tok[idx_label+1] +2);
				tmp_str[strlen(tmp_str)-1] = '\0';
				for(int k = 0;k < strlen(tmp_str);k++){
					disp *= 256;
					disp += tmp_str[k];
				}
				lstArr[idx].Objcode = disp;
			}
			else if(asm_tok[idx_label + 1][0] == 'X'){//store hex
				strcpy(tmp_str, asm_tok[idx_label+1] +2);
				tmp_str[strlen(tmp_str)-1] = '\0';
				lstArr[idx].Objcode = strtoHex(tmp_str);
				strcpy(lstArr[idx].str,tmp_str);
				lstArr[idx].printObjnum = -1;
				if(lstArr[idx].Objcode == -1){
					printf("Line %3d : Wrong Hexnumber.\n", idx*5 + 5);
				}
			}
			else{// wrong operand
				printf("Line %3d : Wrong byte operand.\n", idx * 5 + 5);
				return 0;
			}
		}
		else if(strcmp(asm_tok[idx_label],"WORD") == 0){//word
			lstArr[idx].Objcode = (long long)atoi(asm_tok[idx_label+1]);
			lstArr[idx].printObjnum = 6;
		}
		else if(strcmp(asm_tok[idx_label],"RESB") == 0){//resb
			lstArr[idx].Objcode = -1;
		}
		else if(strcmp(asm_tok[idx_label],"RESW") == 0){// resw
			lstArr[idx].Objcode = -1;
		}
		else{
			int format_idx = 0;
			
			if(asm_tok[idx_label][0] == '+'){ // extend mode
				strcpy(OPCODE,asm_tok[idx_label]+1);
				e = 1;
			}
			else{
				strcpy(OPCODE,asm_tok[idx_label]);
			}
			
			if(e == 1) format_idx = 2;

			Opcode_table* tmpOpnode = find_OpcodeNode(OPCODE);
			
			opHex1 = tmpOpnode->value / 16;
			opHex2 = tmpOpnode->value % 16;

			if(tmpOpnode->opcode_format[format_idx] == '1'){
				//format 1
				lstArr[idx].Objcode = tmpOpnode->value;
				lstArr[idx].printObjnum = 2;
				//lstArr[]				
			}
			else if(tmpOpnode->opcode_format[format_idx] == '2'){
				//format 2
				tmpValue = find_register(asm_tok[idx_label+1]);
				if(tmpValue == -1){
					printf("Line %3d : Cannot find register \"%s\".\n", idx* 5 + 5, asm_tok[idx_label+1]);
					return 0;
				}
				Cal_ObjCode = (long long) (tmpOpnode->value << 8) + (tmpValue << 4);
				tmpValue = find_register(asm_tok[idx_label+2]);
				if(tmpValue == -1){
					printf("Line %3d : Cannot find register \"%s\".\n", idx* 5 + 5, asm_tok[idx_label+1]);
					return 0;
				}
				Cal_ObjCode += (long long) tmpValue;

				lstArr[idx].Objcode = Cal_ObjCode;
				lstArr[idx].printObjnum = 4;
			}
			else if(tmpOpnode->opcode_format[format_idx] == '3' ||tmpOpnode->opcode_format[format_idx] == '4' ){
				//format 3 or 4
				if(asmToken > idx_label + 1){
					if(asm_tok[idx_label+1][0] == '@'){// indirec addressing
						n = 1;
						i = 0;
						strcpy(OPERAND,asm_tok[idx_label+1]+1);
					}
					else if(asm_tok[idx_label+1][0] == '#'){//immediate addressing
						n = 0;
						i = 1;
						strcpy(OPERAND,asm_tok[idx_label+1]+1);
					}
					else{// simple addressing
						n = 1;
						i = 1;
						strcpy(OPERAND,asm_tok[idx_label+1]);
					}

					if(asmToken == idx_label + 3){//if number of operand is 2 
						x = 1;
					}
					
					address_loc = find_symbol(OPERAND);
					if(address_loc == -1){
						//error message
						disp = atoi(OPERAND);
						if(disp == 0 && OPERAND[0] != '0'){// cannot find symbol
							printf("Line %3d : Symbol \"%s\" dosen't exist.\n", idx*5+5,OPERAND);
							return 0;
						}
						//stored symbol LOCCTR out of range 
						if(e == 0 && disp >= 4096){//format 3
							printf("Line %3d : Displacement out of range.\n", idx*5+5);
							return 0;
						}
						if(e == 1 && disp >= 1048576){//format 4
							printf("Line %3d : Displacement out of range.\n", idx*5+5);
							return 0;
						}
					}
					else if(e == 0){
						int PC = address_loc - lstArr[idx].Loc_counter - 3;
						if(-2048 <= PC && PC <= 2047){//PC relative
							p = 1;
							if(PC < 0){
								PC += 4096;
								disp = PC;
							}
							else{ 	
								disp = PC;
							}
						}
						else {// base relative
							b = 1;
							if(BASE_flag == 1){
								int BASE_LOC = find_symbol(BASE_name);
								if(BASE_LOC == -1){
									printf("Line %3d : Wrong Base name.\n",idx*5 +5);
									return 0;
								}
								disp = address_loc - BASE_LOC;
							}
							else{// no "BASE" OPCODE
								disp = -1; 
							}
							if(0 > disp && disp > 4095){
								printf("Line %3d : Address out of range.\n", idx*5 + 5);
								return 0;
							}
						}
					}
				}
				else{
					n = 1;
					i = 1;
				}

				if(e){
					b = 0;
					p = 0;
				}
				//calculate objcode
				Cal_ObjCode = (long long) tmpOpnode->value >> 2;
				nixbpe = (long long) (n << 5);
				nixbpe += (long long) (i << 4);
				nixbpe += (long long) (x << 3);
				nixbpe += (long long) (b << 2);
				nixbpe += (long long) (p << 1);
				nixbpe += (long long) e;
				lstArr[idx].Objcode = (Cal_ObjCode << 6) + nixbpe;
				
				if(e == 0){// format 3
					lstArr[idx].Objcode *= (1 << 12);
					lstArr[idx].Objcode += disp;
					lstArr[idx].printObjnum = 6;
				}
				else{ //if extend
					if(address_loc != -1){
						disp = address_loc;
						modify[Modify_idx++] = lstArr[idx].Loc_counter + 1;
						lstArr[idx].printObjnum = 8;
					}
					lstArr[idx].Objcode *= (1<<20);
					lstArr[idx].Objcode += disp;
					lstArr[idx].printObjnum = 8;
				}
			}
		}	
	}
	return 1;
}

int cmd_assembleFilename(char input[100][100], int tok_num){// function for cmd assemble 
	if(tok_num != 2){//wrong option
		return -1;
	}
	FILE* fp = NULL;
	char filename[100], lstfilename[100], objfilename[100];
	int step1,step2; 
	
	strcpy(filename, input[1]);
	
	if(isdotAsm(filename) == 0){
		return 0;
	}
	fp  = fopen(filename, "r");
	
	if(fp == NULL){//file not exist
		printf("Cannot find filename \"%s\" in this directory.\n",filename);
		return 0;
	}
	//assemble init
	tmp_symtab = symtab;
	symtab = NULL;
	st_flag = 0;
	Starting_address = 0;
	pLength = 0;
	programName[0] = 0;
	BASE_line_idx = 0;
	BASE_flag = 0;
	BASE_name[0] = 0;
	End_flag = 0 ;
	lst_idx = 0;
	Modify_idx = 0;
	
	for(int i = 0 ; i< 100 ;i++){
		modify[i] = 0;
	}
	for(int i = 0 ; i< 2000;i++){
		lstArr[i].Loc_counter = 0;
		lstArr[i].label_flag = 0;
		lstArr[i].Objcode = 0;
		lstArr[i].printObjnum = 0;
		lstArr[i].line[0] = 0;
		lstArr[i].str[0] = 0;
	}
	
	//pass1 실행
	step1 = pass1(fp);
	if(step1 != 1){
		//free tmp_symtab
		symtab = tmp_symtab;
		fclose(fp);
		return 0;
	}

	//pass2 실행
	step2 = pass2();
	if(step2 != 1){
		//free symtab
		symtab = tmp_symtab;
		fclose(fp);
		return 0;
	}
	// pass1 과 pass2 가 정상적으로 작동하였을때
	fclose(fp);
	//free_symtab(symtab);
	tmp_symtab = NULL;

	char tmp_fname[100];
	char* tok_filename;

	strcpy(tmp_fname,filename);

	// create filename
	tok_filename = strtok(tmp_fname,".");
	strcpy(lstfilename, tok_filename);
	strcat(lstfilename,".lst");
	strcpy(objfilename, tok_filename);
	strcat(objfilename,".obj");
	//filename.lst 생성
	fp = fopen(lstfilename,"w");
	makeLstFile(fp);	
	fclose(fp);

	//filename.obj 생성
	fp = fopen(objfilename,"w");
	makeObjFile(fp);
	fclose(fp);


	printf("[%s], [%s]\n",lstfilename,objfilename);
	return 1;
}

void makeLstFile(FILE* lstfile){// write information at .lst file
	int i;
	for(i = 0; i < lst_idx; i++){
		fprintf(lstfile, "%-8d\t",i * 5 + 5); //print line num 
		if(lstArr[i].Loc_counter == -1){
			fprintf(lstfile, "    \t");
		}
		else{
			fprintf(lstfile, "%04X\t", lstArr[i].Loc_counter);
		}
		
		fprintf(lstfile, "%s", lstArr[i].line);
		
		if(lstArr[i].Objcode == -1){
			fprintf(lstfile, "\n");
		}
		else{
			for(int j = strlen(lstArr[i].line); j < 40;j++){
				fprintf(lstfile," ");
			}
			switch (lstArr[i].printObjnum)
			{
			case 2:
				fprintf(lstfile, "%02llX\n",lstArr[i].Objcode);
				break;
			case 4:
				fprintf(lstfile, "%04llX\n",lstArr[i].Objcode);
				break;
			case 6:
				fprintf(lstfile, "%06llX\n",lstArr[i].Objcode);
				break;
			case 8:
				fprintf(lstfile, "%08llX\n",lstArr[i].Objcode);
				break;
			default:
				fprintf(lstfile, "%02llX\n",lstArr[i].Objcode);
				break;
			}
			
		}
	}
}

void makeObjFile(FILE* Objfile){// write information at .obj file
	int i;
	char str[300] = {0,},tmp_oneline[100];
	fprintf(Objfile,"H%-6s%06X%06X\n",programName,lstArr[0].Loc_counter,pLength);
	for(i = 0; i< lst_idx;){
		str[0] = '\0';
		tmp_oneline[0] = '\0';
		if(lstArr[i].Loc_counter == -1 || lstArr[i].Objcode == -1){
			i++;
			continue;
		}

		fprintf(Objfile,"T%06X",lstArr[i].Loc_counter);
		
		for(;i<lst_idx;i++){
		
			if(lstArr[i].Loc_counter == -1){
				continue;
			}
			if(lstArr[i].Objcode == -1){
				break;
			}
			else if(lstArr[i].printObjnum == -1){
				strcpy(tmp_oneline,lstArr[i].str);
				//fprintf(Objfile,"%02llX",lstArr[i].Objcode);
			}
			else{
				switch(lstArr[i].printObjnum){
					case 2:
						sprintf(tmp_oneline,"%02llX",lstArr[i].Objcode);
						break;
					case 4:
						sprintf(tmp_oneline,"%04llX",lstArr[i].Objcode);
						break;
					case 6:
						sprintf(tmp_oneline,"%06llX",lstArr[i].Objcode);
						break;
					case 8:
						sprintf(tmp_oneline,"%08llX",lstArr[i].Objcode);
						break;
					default:
						sprintf(tmp_oneline,"%llX",lstArr[i].Objcode);
						break;
				}
			}
			if(strlen(str) + strlen(tmp_oneline) > 60){
				i--;
				
				break;
			}
			strcat(str,tmp_oneline);
		}
		fprintf(Objfile,"%02X%s\n",(int)strlen(str)/2,str);
		i++;
	}

	for(i = 0; i<Modify_idx;i++){//modify
		fprintf(Objfile, "M%06X05\n",modify[i]);
	}
	//if(st_flag == 1)
	fprintf(Objfile, "E%06X\n", lstArr[0].Loc_counter);
}

int isdotAsm(char* filename){// check if file extension end with ".asm"
	char* dotAsm = ".asm";
	
	char* ptr = strstr(filename, dotAsm);

	if(ptr == NULL || strcmp(dotAsm,ptr) != 0){
		printf("Valid filename extension of source file is \".asm\". Check your input.\n");
		return 0;
	}
	return 1;
}

int cmd_symbol(char input[100][100], int tok_num){// function for cmd symbol
	if(tok_num != 1){
		return -1;
	}	
	if(symtab == NULL){
		printf("SYMTAB not exist!\n");
		return 1;
	}
	
	symbolNode* pWalk;
	pWalk = symtab;
	while(pWalk != NULL){
		printf("\t%s\t%04X\n",pWalk->Label,pWalk->Loc_counter);
		pWalk = pWalk->link;
	}

	return 1;	
}
int add_symTab(char asm_tok[100][100], int tok_num,int LOCCTR){
	symbolNode* pNew, *pWalk, *pPrev;
	pNew = (symbolNode *)malloc(sizeof(symbolNode));
	
	pNew->Loc_counter = LOCCTR;
	strcpy(pNew->Label, asm_tok[0]);
	pNew->link = NULL;
	if(symtab == NULL){
		symtab = pNew;
		return 1;
	}
	
	pWalk = symtab;
	pPrev = pWalk;

	while(pWalk != NULL){
		if(strcmp(pWalk->Label, pNew->Label) == 0){
			return -1; // same label error
		}
		if(pWalk == symtab){
			if(strcmp(pWalk->Label,pNew->Label) > 0){
				pNew->link = pWalk;
				symtab = pNew;
				return 1;
			}
		}
		else if(strcmp(pWalk->Label,pNew->Label) > 0){
				pNew->link = pPrev->link;
				pPrev->link = pNew;				
				return 1;
		}
		pPrev = pWalk;
		pWalk = pWalk->link;
	}
	pPrev->link = pNew;
	return 1;
}

int find_symbol(char* operand){// function for find symbol
	symbolNode* pWalk;
	pWalk = symtab;
	while(pWalk != NULL){
		if(strcmp(pWalk->Label,operand) == 0){
			return pWalk->Loc_counter;
		}
		pWalk = pWalk->link;
	}
	return -1;
}
int find_register(char* reg){// function for find register 
	if(strcmp(reg, "A") == 0 || strcmp(reg, "") == 0){
		return 0;
	}
	else if(strcmp(reg, "X") == 0){
		return 1;
	}
	else if(strcmp(reg, "L") == 0){
		return 2;
	}
	else if(strcmp(reg, "B") == 0){
		return 3;
	}
	else if(strcmp(reg, "S") == 0){
		return 4;
	}
	else if(strcmp(reg, "T") == 0){
		return 5;
	}
	else if(strcmp(reg, "F") == 0){
		return 6;
	}
	else if(strcmp(reg, "PC") == 0){
		return 8;
	}
	else if(strcmp(reg, "SW") == 0){
		return 9;
	}
	// can not find register
	return -1;
}
Opcode_table* find_OpcodeNode(char* mnemonic){//opcode hash table에서 입력받은  mnemonic 과 일치하는 opcode return 
	// find Opcode_mnemonic
	// if find input mnemonic in Opcode_hash_table, return 0.
	// if not print caution message and return 1.
	int i;
	Opcode_table* pFind;

	for(i = 0; i< 20 ; i++){
		pFind  = opTable[i];
		
		while(pFind != NULL){
			if(strcmp(pFind->key, mnemonic) == 0){
				//printf("opcode is %X\n",pFind->value);
				return pFind;
			}
			pFind = pFind->link;
		}
	}
	// if input mnemonic can not found in opTable.
	return NULL;
}
void free_symtab(symbolNode* remove){//free symbolnode
	symbolNode* pRemove;

	while(remove != NULL){	
		pRemove = remove;
		//
		//printf("%d\n",pRemove->num);
		//
		remove = remove->link;
		free(pRemove);
	}
}
