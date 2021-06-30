#include "20161565.h"
#define MIN_VALUE 32
#define MAX_VALUE 126

int cmd_dump(char input[100][100], int tok_num){// du[mp] [start, end] 명령어를 처리한다.
	// no arg
	// start only 
	// start, end
	int start, end;
	int ptr = MEMORY_IDX;

	if(tok_num > 3){
		return -1;
	}

	switch(tok_num){
		case 1: //dump
			if(MEMORY_IDX >0xFFFFF) MEMORY_IDX = 0;
			start = MEMORY_IDX;
			end = min(MEMORY_IDX + 0x9F, 0xFFFFF);

			MEMORY_IDX = end + 1;
		break;

		case 2: //dump start
			start = strtoHex(input[1]);
			end = min(start + 0x9F,0xFFFFF);

			MEMORY_IDX = end + 1;
		break;
		
		case 3://dump start, end
			start = strtoHex(input[1]);
			end = strtoHex(input[2]);

			MEMORY_IDX = end + 1;
		break;
	}

	if(start == -1 || end == -1){ // input start or end are out of range
		printf("check input. start or end address is not hexamecimal number.\n");	
		MEMORY_IDX = ptr;
		return 0;
	}
	if(valid_startendRange(start, end) == -1){//check both start and end address are valid format
		MEMORY_IDX = ptr;
		return 0;
	}
	
	int i,j;
	int dump_start = (start / 16) * 16; //print dump start line index 
	int dump_end = (end / 16) * 16; //print dump end line  index 
	int print_line; // number of print line
	int line_idx;
	print_line = (dump_end - dump_start) / 16 + 1;

	for(i = 0; i < print_line; i++){
		line_idx = dump_start + 16 * i;
		printf("%05X ", line_idx);

		for(j = 0; j <16 ; j++){
			
			if(line_idx + j  < start || line_idx + j> end){
				printf("   ");
			}
			else{
				printf("%02X ", memory[line_idx + j] );
			}
		}
		printf("; ");
		for(j = 0; j < 16; j++){
			if(line_idx + j < start || line_idx + j > end){
				printf(".");
			}
			else if ( memory[line_idx + j] < MIN_VALUE || memory[line_idx + j] > MAX_VALUE){
				printf(".");
			}
			else{
				printf("%c",memory[line_idx + j]);
			}
		}
		printf("\n");
	}
	
	return 1;
};
int strtoHex(char* parameter){ // 해당 parameter를 16진수로 변환하여 return
	int hexToint = 0;
	if(valid_hexa(parameter) == -1){//invalid character in parameter
		return -1;
	}
	
	hexToint = strtol(parameter, NULL, 16); // strTohex
	
	return hexToint;
}
int valid_hexa(char* c){// 입력받은 c string이 16진수의 형태를 띄고 있는지 확인한다.
	// check if st and end are right range
	int i;
	for(i = 0 ; i < strlen(c);i++){
		if(('a' <= c[i]&& c[i] <= 'f') || ('A' <= c[i] && c[i] <= 'F') || ('0' <= c[i] && c[i] <= '9')) continue;
		else {
			return -1;
		}
	}
	return 1;
};
int valid_startendRange(int st_addr, int en_addr){// 입력받은 start end address 가 valid 한지 확인한다.
	if( st_addr > 0xFFFFF || en_addr > 0xFFFFF){ // address is too big 
		printf("start or end address is less than 0xFFFFF.\n");
		return -1;
	}
	
	if(st_addr > en_addr){// start address is bigger than end address
		printf("end address must be greater than start address.\n");
		return -1;
	}

	return 1;

};
int cmd_edit(char input[100][100], int tok_num){// e[dit] address, value 명령어를 처리한다.
	int address;
	int value;
	if(tok_num != 3) {//invalid args
		return -1;
	}
	
	address = strtoHex(input[1]);
	value = strtoHex(input[2]);
	
	if(address == -1 || value == -1){// address or value is not hexamecimal number
		printf("Check input. Address or value is not hexadecimal number.\n");
		return 0;
	}
	
	if(value > 0xFF){// value is too big 
		printf("Check input. Value must be less than 0xFF.\n");
	}
	if (address > 0xFFFFF){// address is too big
		printf("Check input. Address must be less than 0xFFFFF.\n");
	}
	if((value > 0xFF) || (address > 0xFFFFF)){
		return 0;
	}

	unsigned char cpy_val = value;
	
	memory[address] = cpy_val;
	return 1;
};

int cmd_fill(char input[100][100], int tok_num){// f[ill] start, end, value 명령어를 처리한다.
	int start, end , value;
	int i;
	if(tok_num != 4){
		return -1;
	}
	
	start = strtoHex(input[1]);
	end = strtoHex(input[2]);
	value = strtoHex(input[3]);

	if(start == -1 || end == -1 || value == -1){
		printf("Check input. Start, end address or value are not hexadecimal number.\n ");
		return 0;
	}

	//start, end range check
	if(valid_startendRange(start,end) == -1){
		return 0;
	}
	//value range check;

	if(value > 0xFF){
		printf("Value must be less than 0xFF.\n");
		return 0;	
	}
	
	unsigned char cpy_val = value;
	//fill memory with value from start address to end address
	for(i = start; i < end + 0x01; i++){
		memcpy(memory + i , &cpy_val, 1);
	}
	
	return 1;
};
int cmd_reset(int tok_num){// reset 명령어를 처리한다.
	if(tok_num != 1){
		return -1;
	}
	int i;
	for(i = 0; i < MAX_MEMORY; i++){
		memory[i] = 0x00;
	}
	return 1;
};
