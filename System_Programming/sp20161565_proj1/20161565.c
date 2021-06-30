#include "20161565.h"

int main(){
	
	char input_str[100];// SIC/XE 환경에서 들어온 input string을 저장
	char real_input[100]; // input_str 에서 받은 input에서 앞, 뒤 공백 제거한 string
	char tmp_history[100]; // input history
	char input_tok[100][100]; // parsing 된 real_input 의 command, register를 저장
	char command[100]; // parsing 된input_tok[0]을 저장 
	int par_num; //parsing 된 문자열의 개수
	int input_switch;	// 명령어 관련 함수에서 반환되는 값을 저장하여 맞게 작동되었는지 확인한다.
							//input_switch = 1 : 함수가 올바르게 작동
							//input_switch = 0 : 함수가 올바르게 작동하지 못하여 함수 내에서 error message 를 출력
							//input_switch = -1: 함수가 올바르게 작동하지 못하여 error message 출력 필요
	init();// opcode hash table을 생성하고 전역변수의 동적할당을 실행한다.
	
	while(1){
		printf("sicsim> ");
		fgets(input_str,100,stdin);
		
		if((int) strlen(input_str) > 100){
			printf("INPUT COMMAND LENGTH MUST BE LESS THAN 100.\n");
			continue;
		}
		
		input_str[strlen(input_str) -1] = '\0'; // fgets는 '\n' 까지 입력받음으로 마지막 인덱스에 해당하는 값을 '\0'로 변경
		strcpy(real_input,trim_str(input_str));	// 들어온 input string을 ltrim, rtrim 하여 real_input으로 복사 
		strcpy(tmp_history, real_input);	
		//
		fflush(stdin);	// 남아있는 buffer를 비워준다.

		//
		if(isvalid_input(real_input)== -1){// input stirng 이 invalid 한 형태일경우
			printf("\"%s\" is a invalid input. Check 'h[elp]'.\n",real_input);
			continue;
		}
		
		parse_input(input_str,input_tok,&par_num); // real_input을 tokenize한다.
		
		if(real_input[0] == '\0'){// 입력받은 input string이 없거나 공백일경우
			continue;
		}
		strcpy(command,input_tok[0]); //첫번째 tokenize string 을 command 로 복사

		if(strcmp(command,"h") == 0 || strcmp(command,"help") == 0){// h[elp]
			input_switch = cmd_help(par_num);
			if(input_switch == 1){ 
				add_history(tmp_history);
			}
			else if(input_switch == -1){
				printf("\"%s\" is invalid option. Check h[elp].\n",real_input);
			}
		}
		else if(strcmp(command,"d") == 0 || strcmp(command,"dir") == 0){// d[ir]
			input_switch = cmd_dir(par_num);
			if(input_switch == 1){
				add_history(tmp_history);
			}
			else if(input_switch == -1){
				printf("\"%s\" is invalid option. Check h[elp].\n",real_input);
			}
		}
		else if(strcmp(command,"q") == 0 || strcmp(command, "quit") == 0){// q[uit]
			input_switch = cmd_quit(par_num);
			if (input_switch == 1) break;
			else if(input_switch == -1){
				printf("\"%s\" is invalid option. Check h[elp].\n",real_input);
			}
		}
		else if(strcmp(command, "hi") == 0 || strcmp(command, "history") == 0){//hi[story]
			input_switch = cmd_history(real_input, par_num);
			if(input_switch == -1){
				printf("\"%s\" is invalid option. Check h[elp].\n",real_input);
			}
		}
		else if(strcmp(command, "du") == 0 || strcmp(command, "dump") == 0 ){//du[mp] [start, end]
			input_switch = cmd_dump(input_tok,par_num);
			if(input_switch == 1){
				add_history(tmp_history);
			}
			else if(input_switch == -1){
				printf("\"%s\" is invalid option. Check h[elp].\n",real_input);
			}
		}
		else if(strcmp(command,"e") == 0 || strcmp(command, "edit")==0){// e[dit] start, end
			input_switch = cmd_edit(input_tok,par_num);
			if(input_switch == 1){
				add_history(tmp_history);
			}	
			else if(input_switch == -1){
				printf("e[dit]\"%s\" is invalid option. Check h[elp].\n",real_input);
			}
		}
		else if(strcmp(command, "f") == 0 || strcmp(command, "fill") == 0){// f[ill] start, end, value
			input_switch = cmd_fill(input_tok, par_num);
			if(input_switch == 1){
				add_history(tmp_history);
			}
			else if(input_switch == -1){
				printf("\"%s\" is invalid option. Check h[elp].\n",real_input);
			}
		}
		else if(strcmp(command, "reset") == 0){// reset
			input_switch = cmd_reset(par_num);
			if(input_switch == 1){
				add_history(tmp_history);
			}
			else if(input_switch == -1){
				printf("\"%s\" is invalid option. Check h[elp].\n",real_input);
			}
		}
		else if(strcmp(command, "opcode") == 0){// opcode mnemonic
			input_switch = cmd_Opcode_mnemonic(input_tok,par_num);
			if(input_switch == 1){
				add_history(tmp_history);
			}
			else if(input_switch == -1){
				printf("\"%s\" is invalid option. Check h[elp].\n",real_input);
			}
		}
		else if(strcmp(command, "opcodelist") == 0){// opcodelist
			input_switch = print_Opcode_list(par_num);
			if(input_switch == 1){
				add_history(tmp_history);
			}
			else if(input_switch == -1){
				printf("\"%s\" is invalid option. Check h[elp].\n",real_input);
			}
		}
		else{ // invalid command 
			printf("\"%s\" is invalid input. Check h[elp].\n",tmp_history);
		}
	}
	return 0;
}

void init(){
	int i;
	history_head = NULL;	//history_head 초기화
	
	opTable = (Opcode_table **)malloc(20 * sizeof(Opcode_table *));	// opTable[20] 동적할당
	
	for(i = 0;i < 20 ; i++) opTable[i] = NULL;	//opTable 초기화
	
	Open_Opcode();	// opcode hash table 생성 
	memory = (unsigned char *)calloc(MAX_MEMORY,sizeof(unsigned char));// 가상 메모리 동적할당
}
