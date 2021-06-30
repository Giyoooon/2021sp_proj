#include "20161565.h"

int cmd_progaddr(char input[100][100], int tok_num){// progaddr [address] : address를 program 시작주소로 지정해준다.
    if(tok_num >2){
        return -1;
    }
    int prog_addr = strtol(input[1],NULL,16); // [address]는 string으로 입력되므로 16진수로 변환
    if(prog_addr < 0 || prog_addr > 0xFFFFF){// prog_addr range check
        program_address = 0;
        printf("Program address out of range.\n");
        return 0;
    }
    program_address = EXEADDR;
    return 1;
}
void init_estab(){//estab을 초기화 한다.
    pLength = 0;
    for(int i=0;i<4;i++){
        for(int j= 0 ; j < 100; j++){
            estab[i][j].name[0] = '\0';
            estab[i][j].address = 0;
        }
        estab_Max[i] = 0;
    }
}
void add_estab(int file_num,char* symbol,int addr){// estab[file_num]의 맨끝에 external symbol과 주소를 추가한다.
    for(int i =0 ; i<6;i++){ // 6자리에 ' '이나 '\n'이 들어갈 경우 '\0'으로 바꾸어줌
        if(symbol[i] == ' ' || symbol[i] == '\n'){
            symbol[i] = '\0';
        }
    }

    strcpy(estab[file_num][estab_Max[file_num]].name,symbol);
    estab[file_num][estab_Max[file_num]].address = addr;

    estab_Max[file_num]++;
}
int return_estabAddr(char* name){// 해당 name과 일치하는 estab의 address를 반환한다.
    for(int i = 0; i < 4; i++){
        for(int j = 0 ; j < estab_Max[i]; j++){
            if(strcmp(estab[i][j].name, name) == 0){
                return estab[i][j].address;
            }
        }
    }
    return -1;
}
int cmd_loader(char input[100][100],int tok_num){// 명령어 loader [filename1.obj] .. 에 관한 명령을 실행한다.
    if(tok_num == 1 || tok_num > 4){//input check
        return -1;
    }
    FILE* fp[4];
    int file_number;
    int pass1,pass2;
    
    init_estab();

    for(int i=0; i< tok_num-1;i++){// filename.obj 파일을 읽어 fp[i]에 저장한다.
        fp[i] = fopen(input[i+1],"r");
        if(fp[i] == NULL){
            printf("\"%s\"is not exist in current directory.\n", input[i+1]);
            return 0;
        }
    }

    file_number = tok_num - 1;
    pass1 = loader_pass1(fp,file_number);
    if(pass1 == 0){
        init_estab();
        loader_flag = -1;
        return 0;
    }
    
    for(int i=0;i<file_number;i++){
        fclose(fp[i]);
        fp[i] = fopen(input[i+1],"r");
        if(fp[i] == NULL){
            printf("\"%s\"is not exist in current directory.\n", input[i+1]);
            return 0;
        }
    }
    pass2 = loader_pass2(fp,file_number);
    if(pass2 == 0){
        init_estab();
        loader_flag = -1;
        return 0;
    }
    print_loadMap(file_number);
    for(int i=0;i<file_number;i++){ 
        fclose(fp[i]);
    }
    loader_flag =1;
    return 1;
}
int loader_pass1(FILE* fp[], int file_number){// loader 2pass중 pass1 에 해당하는 동작을 수행한다.
    int CSADDR = 0;
    int CSLTH;
    char str[100];
    char prog_name[10];
    CSADDR = program_address;
    for(int i = 0; i < file_number; i++){
        fgets(str,100,fp[i]);
        strncpy(prog_name,str+1,6);
        prog_name[6] = '\0';
        CSLTH = strtol(str+13,NULL,16);
        if(return_estabAddr(prog_name) != -1){//해당 symbol이 중복일경우
            printf("\"%s\"is duplicated symbol.\n",prog_name);
            return 0;
        }
        add_estab(i,prog_name,CSADDR);
        while(fgets(str,100,fp[i]) !=NULL){
            char recordType = str[0];
            str[strlen(str)-1] = '\0';
            if(recordType == 'D'){
                int rel_addr;
                char* ptr;
                char ext_symbol[10], relAddr_str[10];
                for(ptr = str+1; *(ptr) != '\0'; ptr += 12){
                    strncpy(ext_symbol,ptr,6);
                    ext_symbol[6]= '\0';
                    strncpy(relAddr_str,ptr+6,6);
                    relAddr_str[6]= '\0';
                    rel_addr = strtol(relAddr_str,NULL,16);
                    if(return_estabAddr(ext_symbol) != -1){
                        printf("\"%s\"is duplicated symbol.\n",ext_symbol);
                        return 0;
                    }
                    add_estab(i, ext_symbol, rel_addr + CSADDR);
                }
            }
            else
                continue;
        }
        CSADDR += CSLTH;
        program_Length[i] = CSLTH;
        pLength += program_Length[i];
    }
    return 1;
}
int loader_pass2(FILE* fp[], int file_number){// loader 2pass중 pass2 에 해당하는 동작을 수행한다.
    int CSADDR = 0;
    int CSLTH;
    char str[100];
    char refName[100][10]={'\0',};
    CSADDR = program_address;
    for(int i =0;i < file_number;i++){
        fgets(str,100,fp[i]);
        CSLTH = strtol(str+13,NULL,16);

        while(fgets(str,100,fp[i]) !=NULL){ // 'T' record type
            char recordType = str[0];
            //printf("%c\n",recordType);
            if(recordType == '.' || recordType == 'D')
                continue;
            if(recordType == 'T'){
                char str_addr[10];
                char str_len[10];
                int text_Addr, text_len;

                strncpy(str_addr,str+1,6);
                str_addr[6] = '\0';
                strncpy(str_len, str+7,2);
                str_len[2] ='\0';
                text_Addr = strtol(str_addr,NULL,16) + CSADDR;
                text_len = strtol(str_len,NULL,16);

                for(int j = 0; j < text_len; j++){
                    char tmp[10];
                    strncpy(tmp,str + 9 + 2 * j, 2);
                    tmp[2] = '\0';
                    memory[text_Addr + j] = (unsigned char) strtol(tmp,NULL,16);
                }
            }
            else if(recordType == 'M'){//'M' record type
                char str_modify_address[10];
                char str_len[10];
                char tmp[10];
                int modify_address;
                int obj_value;
                strncpy(str_modify_address,str+1,6);
                str_modify_address[6] = '\0';
                strncpy(str_len,str+7,2);
                str_len[2] = '\0';

                modify_address = strtol(str_modify_address,NULL,16)+CSADDR;
                for(int j = 0; j < 3; j++){
                    sprintf(tmp + j*2, "%02X", memory[modify_address + j]);
                }
                tmp[6] = '\0';
                obj_value = strtol(tmp,NULL,16);

                if(tmp[0] >= '8' || tmp[0] < '0'){// signed bit == 1, 2의 보수
                    obj_value = -(0xFFFFFF - obj_value +1);
                }
                
                strncpy(tmp, str+10,2);
                tmp[2] = '\0';

                int ref_idx;
                int ref_Addr;
                char operator = str[9];
                ref_idx = atoi(tmp);
                ref_Addr = return_estabAddr(refName[ref_idx]);
                if(ref_Addr == -1){
                    printf("Symbol \"%s\"is not defind.\n", refName[ref_idx]);
                    return 0;
                }
                if(operator == '+'){
                    obj_value += ref_Addr;
                }
                else if(operator == '-'){
                    obj_value -= ref_Addr;
                }

                if(obj_value < 0){
                   obj_value -= 0xFF000000;
                }
                char objValue[10];
                sprintf(objValue,"%06X", obj_value);
                for(int j = 0;j < 3; j++){
                    char tmp_mem[10];
                    strncpy(tmp_mem, objValue + j * 2, 2);
                    tmp_mem[2] = '\0';
                    memory[modify_address+j] = (unsigned char) strtol(tmp_mem,NULL,16); 
                }
            }
            else if(recordType == 'R'){ // 'R' record type
                int ref_idx;
                char tmp[10];
                strcpy(refName[1], estab[i][0].name);

                for(int j = 1; j < strlen(str)-1; j += 8){
                    strncpy(tmp, str+j,2);
                    tmp[2] = '\0';
                    ref_idx = atoi(tmp);
                    strncpy(tmp, str + j + 2, 6);
                    tmp[6] = '\0';
                    for(int k = 0; k < 6 ; k++){
                        if(tmp[k] == ' ' || tmp[k] == '\n'){
                            tmp[k] = '\0';
                        }
                    }
                    strcpy(refName[ref_idx], tmp);
                }
            }
            else if( recordType == 'E'){
                if(str[1] != '\n'){
                    char tmp[10];
                    strncpy(tmp, str+1,6);
                    tmp[6] = '\0';
                    EXEADDR = CSADDR + strtol(tmp,NULL,16);
                }
                CSADDR += CSLTH;
            }
            
        }
    }
    return 1;
}
void print_loadMap(int file_num){// loader가 성공적으로 끝났다면 loadmap을 출력한다.
    printf("control symbol address length\n");
    printf("section name\n");
    printf("--------------------------------\n");
    for(int i = 0; i < file_num;i++){
        for(int j = 0; j < estab_Max[i]; j++){
            if(j == 0){
                printf("%-7s          %04X  %04X\n",estab[i][j].name,estab[i][j].address,program_Length[i]);
            }
            else{           
                printf("        %6s   %04X\n",estab[i][j].name,estab[i][j].address);
            }
        }
    }
    int total_length = 0;
    for(int i = 0; i< file_num;i++){
        total_length += program_Length[i];
    }
    printf("--------------------------------\n");
    printf("          total length %04X\n",total_length);
}