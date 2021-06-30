#include "20161565.h"

void parse_input(char* input,char parsed_tok[100][100],int* tok_num){// 들어온input을 tokenzie 한다.
	char* token;
	*tok_num =0;
	
	token = strtok(input,"\t ,");	// '\t', ' ', ','을 기준으로 input string을 tokenize
	while(token != NULL){
		strcpy(parsed_tok[(*tok_num)],token);
		(*tok_num)++;
		token = strtok(NULL,"\t ,");
	}
};

int isvalid_input(char* input){// 들어온 input string이 valid 한 형태인지 확인한다.
	int i;
	int flag = 0;
	//flag 0 : command 단독
	//flag 1 : command + ' 'or '\t'.. 
	//flag 2 : command + ' ' + arg..
	//flag 3 : command + ' ' + arg + ','..
	//flag 4 : command + ' ' + arg + ' '..
	char tmp_str[100];
	int char_func;
	strcpy(tmp_str, input);
	
	for(i = 0 ; tmp_str[i] != '\0';i++){
		char_func = isvalid_char(tmp_str[i]);
		
		if(char_func == -1){
			return -1;
		}

		if(flag == 0){//command...
			switch(char_func){
				case 0:	
					flag = 1;
					break;
				case 1:
					break;
				default:
					return -1;
			}
		}
		else if (flag == 1){//command"__"...
			switch(char_func){
				case 0:
					break;
				case 1:
					flag = 2;
					break;
				default:
					return -1;
			}
		}
		else if(flag == 2){//command__"arg"...
			switch(char_func){
				case 2:
					flag = 3;
					break;
				case 1:
					break;
				case 0:
					flag = 4;
					break;
			}
		}
		else if(flag == 3){//command__arg","...
			switch(char_func){
				case 2:
					return -1;
				case 1:
					flag = 2;
					break;
			}
		}
		else if(flag == 4){//command__arg"__"
			switch(char_func){
				case 1:
					return -1;
				case 2:
					flag = 3;
					break;
			}
		}
	}
	
	if(flag == 3) return -1; // input string이 , 로 끝나는 경우
	else return 1; // valid한 형태인 경우
};

int isvalid_char(char c ){// 해당 character 가 valid 한지 확인 
	int i;

	for(i = 0; i < 26; i++){
		if(('a' + i) == c || ('A' + i) == c) return 1;
	}// character is 'a' ~ 'z' or 'A' ~'Z''
	for(i = 0; i < 10 ;i++){
		if(('0' + i) == c) return 1;
	}// character is '0' ~ '9'
	
	if(c == ' ' || c == '\t') return 0; //character is space or tab
	if (c == ',') return 2; // character is comma
	return -1; //wrong character


}

char* trim_str(char* input){// input string 의 앞, 뒤 공백 제거
	int i=0;
	int len;
	int ptr = 0;
	char trim[100];
	if(input == NULL){
		return input; 
	}
	
	len = strlen(input);
	strcpy(trim,input);
	
	for(i = 0 ; i< len ;i++){// ltrim
		if(trim[i] == ' ' || trim[i] == '\t' || trim[i] == '\n'){
			ptr++;
		}
		else break;
	}
	
	for(i = len-2; i >= 0;i--){ //rtrim
		if(input[i] == ' ' || input[i] == '\t' || input[i] == '\n'){
			trim[i] = '\0';
		}
		else break;
	}
	
	return input + ptr; //trim 된 input string 반환

};
