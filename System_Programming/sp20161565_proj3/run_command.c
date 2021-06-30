#include "20161565.h"
void init_reg(){// register를 초기화한다. PC는 프로그램 시작주소, L 은 프로그램 길이로 초기화한다.
    store_reg.A = 0x0;
    store_reg.X = 0x0;
    store_reg.L = 0x0;
    store_reg.B = 0x0;
    store_reg.T = 0x0;
    store_reg.F = 0x0;
    store_reg.F2 = 0x0;
    store_reg.PC = 0x0;
    store_reg.SW = 0x0;
    for(int i=0;i<20;i++)
        store_reg.reg[i] =0;

    store_reg.PC = program_address;
    store_reg.reg[8] = program_address; 
    store_reg.L = pLength;
    store_reg.reg[2] = pLength;    
}
int cmd_run(char input[100][100],int tok_num){// 명령어 run에 대한 처리를 담당한다.
    if(tok_num != 1){
        return -1;
    }
    if(loader_flag == 1){
        init_reg();
        loader_flag = 0;
    }
    int prog_endAddr = program_address + pLength;
    while(store_reg.reg[8] < prog_endAddr){
        run_instruction(memory[store_reg.reg[8]]);
        for(int i = 0; i < bp_idx;i++){
            if(store_reg.reg[8] == bp[i]){
                print_Reg();
                printf("\t\tStop at checkpoint [%X]\n",bp[i]);
                return 1;
            } 
        }
    }
    print_Reg();
    printf("\t\tEnd Program\n");
    return 1;
}
void run_instruction(int opcode){// 한 instruction에 대한 연산을 수행한다.
    int format = 0;
    int check_format = opcode/0x10;
    switch(check_format){
        case 0xC:
        case 0xF:
            format = 1;
            break;
        case 0xA:
        case 0xB:
            format = 2;
            break;
        default:
            format = 3;
            if(memory[store_reg.reg[8] + 1] & 0b10000)//if e bit is set
                format = 4;
    }
    if(format == 1 || opcode == 0){//format 1 없음. 해당 memory가 비어있다면 
        store_reg.reg[8]++;
        return;
    }
    else if(format == 2){
        int r1,r2;
        r1 = memory[store_reg.reg[8] + 1] / 0x10;
        r2 = memory[store_reg.reg[8] + 1] % 0x10;
        switch(opcode){
            case 0xB4: //CLEAR
                store_reg.reg[r1] = 0;
            break;
            case 0xA0: //COMPR
                if(store_reg.reg[r1] == store_reg.reg[r2]){
                    store_reg.reg[9] = '='; //reg[9] :SW
                }
                else if(store_reg.reg[r1] > store_reg.reg[r2]){
                    store_reg.reg[9] = '>'; //reg[9] :SW
                }
                else{ //store_reg.reg[r1] < store_reg.reg[r2]
                    store_reg.reg[9] = '<'; //reg[9] :SW
                }
            break;
            case 0xB8://TIXR
                store_reg.reg[1]++; // X++;
                if(store_reg.reg[1] == store_reg.reg[r1]){
                    store_reg.reg[9] = '='; //reg[9] :SW
                }
                else if(store_reg.reg[1] > store_reg.reg[r1]){
                    store_reg.reg[9] = '>'; //reg[9] :SW
                }
                else{ //store_reg.reg[1] < store_reg.reg[r2]
                    store_reg.reg[9] = '<'; //reg[9] :SW
                }
            break;
        }
        store_reg.reg[8] +=2; //PC += 2;
    }
    else{ //format 3 or 4
        int nixbpe[6]= {0,0,0,0,0,0};
        int addr = find_AddrorConst(store_reg.reg[8], format); 
        find_nixbpe(nixbpe, store_reg.reg[8]);
        store_reg.reg[8] += format;
        
        if(format == 3 && (addr > (1<<11)) && (nixbpe[4] == 1)){//PC relative & signed bit 1
            addr -= (1 << 12); //2's complement
        }
        if(nixbpe[2] == 1){ // x bit set
            addr += store_reg.reg[1];
        }
        if(nixbpe[3] == 1){ // b bit set
            addr += store_reg.reg[3];
        }
        if(nixbpe[4] == 1){ // p bit set
            addr += store_reg.reg[8];
        }
        if(nixbpe[0] == 1 && nixbpe[1] == 0){ //indirect aadressing
            addr = indirect_addr(addr, 3);
        }
        
        switch((opcode / 4) * 4){
            case 0x14: //STL   
                store_memory(addr, store_reg.reg[2], 3);// memory <- L
            break;    
            case 0x68://LDB
                if(nixbpe[0] == 0 && nixbpe[1] == 1){
                    store_reg.reg[3] = addr; // B <- addr
                }
                else{
                    store_reg.reg[3] = indirect_addr(addr,3);
                }
            break;
            case 0x48://+JSUB
                store_reg.reg[2] = store_reg.reg[8];
                store_reg.reg[8] = addr;
            break;
            case 0x00://LDA
                if(nixbpe[0] == 0 && nixbpe[1] == 1){
                    store_reg.reg[0] = addr; // B <- addr
                }
                else{
                    store_reg.reg[0] = indirect_addr(addr,3);
                }
            break;
            case 0x28://COMP
                if(!(nixbpe[0] == 0 && nixbpe[1] == 1)){
                    addr = indirect_addr(addr,3);
                }
                if(store_reg.reg[0] == addr){
                    store_reg.reg[9] = '=';
                }
                else if(store_reg.reg[0] > addr){
                    store_reg.reg[9] = '>';
                }
                else{ //store_reg.reg[0] < addr
                    store_reg.reg[9] = '<';
                }
            break;
            case 0x30://JEQ
                if(store_reg.reg[9] == '='){
                    store_reg.reg[8] = addr;
                }
            break;
            case 0x3C://J
                store_reg.reg[8] = addr;
            break;
            case 0x0C://STA
                store_memory(addr,store_reg.reg[0],3);
            break;
            case 0x74://LDT
                if(!(nixbpe[0] == 0 && nixbpe[1] == 1)){
                    addr = indirect_addr(addr, 3);  
                }
                store_reg.reg[5] = addr;
            break;
            case 0xE0://TD
                //다음 instruction으로 넘어가되 CC 는 '<'로 변경되었다고 가정한다.
                store_reg.reg[9] = '<';
            break;
            case 0xD8://RD
                //input device로부터 아무것도 받지 못했다고 가정.
                // 다음 명령어인 COMPR A,S의 결과 CC 가 '=' 이라고 가정한다.
                store_reg.reg[0] = 0; // A = 0
                store_reg.reg[9] = '=';
            break;
            case 0x54://STCH
                store_memory(addr, store_reg.reg[0] % 0X10, 1);
            break;
            case 0x38://JLT
                if(store_reg.reg[9] == '<'){
                    store_reg.reg[8] = addr;
                }
            break;
            case 0x10://STX
                store_memory(addr, store_reg.reg[1], 3);
            break;
            case 0x4C://RSUB
                store_reg.reg[8] = store_reg.reg[2];//PC = l
            break;
            case 0x50://LDCH
                if(!(nixbpe[0] == 0 && nixbpe[1] == 1)){
                    addr = indirect_addr(addr,3);
                }
                store_reg.reg[0] =(store_reg.reg[0] & 0xFFFFFF00) + (addr/0x10000);
            break;
            case 0xDC://WD
                //다음 instruction으로 넘어간다.
            break;
        }
    }
    sync_reg();//store_reg의 register array와 각 register간의 동기화
}
void store_memory(int addr, int value, int byte){ // value를 memory의 해당 address에 byte만큼 저장한다.
    char tmp[10], word[3];

    if(value < 0){
        value -= 0xFF000000;
    }
    sprintf(tmp,"%06X",value);
    for(int i = 0; i < byte; i++){
        strncpy(word,tmp + i * 2 ,2);
        word[2] = '\0';
        memory[addr+i] = strtol(word, NULL, 16);
    }
};
int indirect_addr(int addr, int byte){//n=0,i=1 일때 indircet address를 반환한다.
    int indir_addr = memory[addr];
    for(int i = 1; i < byte; i++){
        indir_addr *= 0x100;
        indir_addr += memory[addr+i];
    }
    return indir_addr;
};
void sync_reg(){//store_reg의 register array와 각 register간의 동기화
    store_reg.A = store_reg.reg[0];
    store_reg.X = store_reg.reg[1];
    store_reg.L = store_reg.reg[2];
    store_reg.B = store_reg.reg[3];
    store_reg.S = store_reg.reg[4];
    store_reg.T = store_reg.reg[5];
    store_reg.F = store_reg.reg[6];
    store_reg.F2 = store_reg.reg[7];
    store_reg.PC = store_reg.reg[8];
    store_reg.SW = store_reg.reg[9];
}
int find_AddrorConst(int PC,int format){ // format에 따른 address or constant를 반환한다.
    int addr = memory[PC + 1] % 0x10;
    addr *= 0x100;
    addr += memory[PC + 2];
    if(format == 4){//format == 4
        addr *= 0x100;
        addr += memory[PC+3];
    }
    return addr;
}
void find_nixbpe(int nixbpe[], int PC){// array nixbpe[6]에 순서대로 n,i,x,b,p,e에 해당하는 bit condition을 저장한다. 
    int ni = memory[PC] % 0b100;
    int xbpe = memory[PC + 1] / 0b10000;
    if(ni & 0b10){
        nixbpe[0] = 1;
    }
    if(ni & 0b01){
        nixbpe[1] = 1;
    }
    if(xbpe & (0b1000)){//x bit set
        nixbpe[2] = 1;
    }
    if(xbpe & (0b0100)){//b bit set
        nixbpe[3] = 1;
    }
    if(xbpe & (0b0010)){//p bit set
        nixbpe[4] = 1;
    }
    if(xbpe & (0b0001)){//e bit set
        nixbpe[5] = 1;
    }
}
void print_Reg(){//register들을 출력한다.
    printf("\tA : %06X   X : %06X\n",store_reg.A, store_reg.X);
    printf("\tL : %06X  PC : %06X\n",store_reg.L, store_reg.PC);
    printf("\tB : %06X   S : %06X\n",store_reg.B, store_reg.S);
    printf("\tT : %06X\n",store_reg.T);
}
int cmd_bp(char input[100][100],int tok_num){// 명령어 bp에 대한 연산을 수행한다.
    if(tok_num > 2){
        return -1;
    }
    else if(tok_num == 1){
        print_bp();
    }
    else{
        int i;
        if(strcmp(input[1],"clear") == 0){// bp clear
            //clear bp
            for(i = 0; bp[i] != -1; i++){
                bp[i] = -1;
            }
            bp_idx = 0;
            printf("\t\t[ok] clear all breakpoints\n");
        }
        else{ // bp [address]
            int bp_address = strtoHex(input[1]);
            if(bp_address < 0 || bp_address > 0xFFFFF){
                printf("Invalid break point address.\n");
                return 0;
            }
            insert_bp(bp_address);
            printf("\t\t[ok] create breakpoint %X\n",bp_address);
        }
    }
    return 1;
}
void insert_bp(int addr){ //명령어 bp [address] : address를 bp_address 배열에 저장 후 오름차순으로 sort하는 함수
    if(bp_idx >= 100){
        printf("Number of breakpoint no more than 100.\n");
        return;
    }
    bp[bp_idx++] = addr;
    if (bp_idx == 1) return ;

    for(int i = bp_idx-1 ; i > 0;i--){
         if(bp[i] < bp[i-1]){
            int tmp = bp[i];
            bp[i] = bp[i-1];
            bp[i-1] = tmp;
        }
        
    }
};

void print_bp(){ //명령어 bp : 모든 breakpoint를 출력하는 함수
    printf("\t\tbreakpoint\n");
    printf("\t\t----------\n");
    for(int i = 0; i < bp_idx; i++){
        printf("\t\t%X\n",bp[i]);
    }
};
