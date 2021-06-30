//20161565 header file
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
//라이브러리
#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })	// 타입 상관없이 a, b 중 큰값을 반환하는 매크로
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })  // 타입 상관없이 a, b 중 작은 값을 반환하는 매크로
#define MAX_MEMORY 1048576 
// 매크로 함수
typedef struct history{// valid input에 대한  history 를 저장하는 struct
	int num;
	char command[100];
	struct history* link;	
} HISTORY;

typedef struct Opcode_hash_table{// "opcode.txt" 의opcode, mnemonic, format을 저장
	char key[10];
	char opcode_format[10];
	int value;
	struct Opcode_hash_table* link;
} Opcode_table;
//전역 번수
int MEMORY_IDX; // dump command를 수행할때 사용하는 unsigned char* memory의 address 
// 전역 linked list
Opcode_table** opTable;	//opcode 를 저장하는 linked list
HISTORY* history_head;	// history를 저장하는 linked list

unsigned char* memory; // 1MByte(16*65536)인 가상의 메모리 공간
//main 
void init(); // Shell 환경을 구축하기 위해 전역변수의 동적할당, opcode_table을 생성 한다.

//input handling
void parse_input(char* input ,char parsed_tok[100][100], int* tok_num); // 들어온 input을 tokenize한다.
int isvalid_char(char c);	// 해당하는 character가 valid 한지 확인한다.
int isvalid_input(char* input);	//들어온 input string이 valid 한 형태를 띄고 있는지 확인한다.
char* trim_str(char* input);	//input string의 앞, 뒤 공백을 삭제한다.
//shell command
int cmd_help(int tok_num);	// h[elp] 명령어를 처리한다. 
int cmd_dir(int tok_num);	// d[ir] 명령어를 처리한다.
int cmd_quit(int tok_num);	// q[uit] 명령어를 처리한다.
int cmd_history(char* input_str, int tok_num);		// hi[story] 를 처리한다.
void add_history(char* input_command);	// valid 한 명령어를 history_head 에 push한다.

// memory command
int cmd_dump(char input[100][100], int tok_num);	// du[mp] [start, end] 명령어를 처리한다.
int cmd_edit(char input[100][100], int tok_num);	// e[dit] address, value 명령어를 처리한다.
int cmd_fill(char input[100][100], int tok_num);	// f[ill] start, end, value 명령어를 처리한다.
int cmd_reset(int tok_num);	// reset 명령어를 처리한다.

int strtoHex(char* parameter);	// input 으로 들어온 start, end 에 해당하는 string을 hex 로 변환하여 반환한다.
int valid_hexa(char* c);	//	해당 문자열이 16진수의 형태를 띄고 있는지 확인한다.
int valid_startendRange(int st_addr,int en_addr);	//	들어온 start, end 의 주소가 valid 한지 확인한다.


 //opcode table command
void Open_Opcode();	//	"opcode.txt" 를 연다.
void gen_Opcode_table(Opcode_table *pNew,int idx);	// Open_Opcode 함수에서 opcode, mnemonic, format 을 scan하여 opcode table을 구성한다.

int cmd_Opcode_mnemonic(char input[100][100], int tok_num);	// opcode mnemonic 명령어를 처리한다.
int find_Opcode_mnemonic(char* mnemonic);	// cmd_Opcode_mneonic 함수에서 입력받은 mnemonic이 opcode table 에 있는지 확인한다.
int print_Opcode_list(int tok_num);	//	opcodelist 명령어를 처리한다.
int create_hashfunction(char* mnemonic);	// opcode table 을 구성할때 쓰이는 hashfunction을 반환한다.

