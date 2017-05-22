/*

The project is developed as part of Computer Architecture class
Project Name: Functional/Pipeline Simulator for simpleRISC Processor

Developer's Name:Srinadh and Keshav.
Developer's Email id:2015csb1015@iitrpr.ac.in and 2015csb1016@iitrpr.ac.in
Date: Oct 14-17.

*/

/* mySimpleSim.cpp
   Purpose of this file: implementation file for mySimpleSim
*/

#include "mySimpleSim.h"
#include <stdlib.h>
#include <stdio.h>

//Register file
static unsigned int R[16];
static int PC;
//flags
static int gt,eq;
//Branches
static unsigned int branchPc,branchtgt;
//memory
static unsigned char MEM[4000];

//intermediate datapath and control path signals
static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;
static unsigned	int immediate;
static unsigned int AluResult;
static unsigned int Ldresult;

void run_simplesim() {
  int i;
  printf("--------------------------------------SIMPLERISC STARTED RUNNING------------------------------\n");
  while((PC<4000) && (read_word(MEM,PC)!=0) ) {
    fetch();
    decode();
    execute();
     //If memory acess goes out of assumed system(i.e here 4000) we get segmentation fault,so dealing with it.
    if(((instruction_word>>27) == 14)||((instruction_word>>27) == 15)){
      if(AluResult>4000){
	printf("Memory out of bounds for assumed system. \n");
	printf("-----------------------------SIMPLERISC ENDS RUNNING DUE TO OUT OF MEMORY BOUNDS ERROR----------------------------------\n");
	break;
      }
    }
    mem();
    write_back();
  }
  for( i=0;i<16;i++){//For printing registers.
  	printf("Register %d:%d\n",i,R[i] );
  }
  printf("---------------------------------------SIMPLERISC ENDS RUNNING----------------------------------\n");
}

// it is used to set the reset values
//reset all registers and memory content to 0
void reset_proc() {
	printf("-------------------RESETTING PROCESSOR STARTS---------------------\n");
	int i;
	for(i=0;i<16;i++){                        // Initialising registers with value 0 and giving stack pointer some value.
		if(i!=14){
			R[i]=0;
		}
		else{
			R[14]=3000;	
		}
	}
	PC=0;
	for(i=0;i<4000;i=i+4){                                       // Initialising memory elements with '\0'.
		MEM[i]='\0';
	}
	branchPc=0;                                                  // Setting BranchPc to zero.
	printf("----------------------RESETTING PROCESSOR ENDS-------------------------\n");
}

//load_program_memory reads the input memory, and pupulates the instruction
// memory
void load_program_memory(char *file_name) {
  FILE *fp;
  unsigned int address, instruction;
  fp = fopen(file_name, "r");
  if(fp == NULL) {
    printf("Error opening input mem file\n");
    exit(1);
  }
  while(fscanf(fp, "%x %x", &address, &instruction) != EOF) {
    write_word(MEM, address, instruction);
  }
 
  fclose(fp);
}

//writes the data memory in "data_out.mem" file
void write_data_memory() {
  FILE *fp;
  unsigned int i;
  fp = fopen("data_out.mem", "w");
  if(fp == NULL) {
    printf("Error opening dataout.mem file for writing\n");
    return;
  }

  for(i=0; i < 4000; i = i+4){
    fprintf(fp, "%x %x\n", i, read_word(MEM, i));
  }
  fclose(fp);
}

//reads from the instruction memory and updates the instruction register
void fetch() {
	//Instruction Fetch.
	printf("----------------INSTRUCTION FETCH STARTS-------------------\n");
	instruction_word=read_word(MEM,PC);
	PC+=4;
	printf("Program Counter:%d\n",PC);
	printf("----------------INSTRUCTION FETCH ENDS-------------------\n");
}
//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void decode() {
	printf("---------------------DECODE STARTS------------------------\n");
	//branchtgt.
	unsigned int bt= ((instruction_word<<5)>>5)<<2;
	if((instruction_word<<5)>>31){//extending sign.
		bt+=(0x7<<29);
	}
	branchtgt=bt;

	//operand 1.
	if((instruction_word>>27) == 20 ){
		operand1=R[15];                       // If isRet is true then storing the address in operand 1.
	}
	else{
		operand1=R[(instruction_word<<10)>>28];          // else extracting the source 1.
	}

	//operand 2.
	if((instruction_word>>27) == 15 ){                  // checking if isSt is 1 or 0 and extracting operand accordingly.
		operand2=R[(instruction_word<<6)>>28];
	}
	else{
		operand2=R[(instruction_word<<14)>>28];
	}

	//immediate
	unsigned int immx=(instruction_word<<16)>>16;
	if((instruction_word <<14)>>30 == 0 ){                      // checking if modifier is 0 and extending accordingly.
		if(immx>>16){
			immx=immx|0xffff0000;
		}

	}
	else if((instruction_word <<14)>>30 == 1){                  // checking if modifier is 1 and extending accordingly.
		immx=immx>>16;
	} 
	else if((instruction_word <<14)>>30 == 2){                  // checking if modifier is 2 and extending accordingly.
		;
	}
	immediate=immx;

	printf("Operand1: %d\n",operand1);
	printf("Operand2: %d\n",operand2);
	//printf("Immediate: %d\n",immediate);
	printf("----------------DECODE ENDS-------------------\n");
}

//executes the ALU operation based on ALUop
void execute() {
	printf("----------------EXECUTE STARTS-------------------\n");
	//Branch pc.
	if((instruction_word>>27) == 20 ){
		branchPc=operand1;
	}
	else{
		branchPc=branchtgt;
	}

	//Alu result.
	//Add Instruction.
	if((instruction_word>>27) == 0){
		if((instruction_word<<5)>>31 == 1){
			AluResult=operand1+immediate;
		}
		else{
			AluResult=operand1+operand2;
		}
		printf("After addition, value is: %d\n",AluResult);
	}

	//Subtract Instruction.
	if((instruction_word>>27) == 1){
		if((instruction_word<<5)>>31){
			AluResult=operand1-immediate;
		}
		else{
			AluResult=operand1-operand2;
		}
		printf("After subtraction, value is: %d\n",AluResult);
	}

	//Multiply Instruction.
	if((instruction_word>>27) == 2){
		if((instruction_word<<5)>>31){
			AluResult=operand1*immediate;
		}
		else{
			AluResult=operand1*operand2;
		}
		printf("After multiplication, value is: %d\n",AluResult);
	}

	//Divide Instruction.
	if((instruction_word>>27) == 3){
		if((instruction_word<<5)>>31){
			AluResult=operand1/immediate;
		}
		else{
			AluResult=operand1/operand2;
		}
		printf("After division, value is: %d\n",AluResult);
	}

	//Mod Instruction.
	if((instruction_word>>27) == 4){
		if((instruction_word<<5)>>31){
			AluResult=operand1%immediate;
		}
		else{
			AluResult=operand1%operand2;
		}
		printf("After taking mod, value is: %d\n",AluResult);
	}

	//cmp Instruction.
	if((instruction_word>>27) == 5){
		if((instruction_word<<5)>>31){
			if(operand1==immediate){
				eq=1;
				gt=0;
			}
			if(operand1>immediate){
				gt=1;
				eq=0;
			}
		}
		else{
			if(operand1==operand2){
				eq=1;
				gt=0;
			}
			if(operand1>operand2){
				gt=1;
				eq=0;
			}
		}
		printf("flag.eq: %d, flag.gt: %d\n",eq,gt );
	}

	//and Instruction.
	if((instruction_word>>27) == 6){
		if((instruction_word<<5)>>31){
			AluResult=operand1 &immediate;
		}
		else{
			AluResult=operand1&operand2;
		}
		printf("After taking AND, value is: %d\n",AluResult);
	}

	//or Instruction.
	if((instruction_word>>27) == 7){
		if((instruction_word<<5)>>31){
			AluResult=operand1|immediate;
		}
		else{
			AluResult=operand1|operand2;
		}
		printf("After taking OR, value is: %d\n",AluResult);
	}

	//not Instruction.
	if((instruction_word>>27) == 8){
		if((instruction_word<<5)>>31){
			AluResult=~immediate;
		}
		else{
			AluResult=~operand2;
		}
		printf("After taking NOT, value is: %d\n",AluResult);
	}

	//Mov Instruction.
	if((instruction_word>>27) == 9){
		if((instruction_word<<5)>>31){
			AluResult=immediate;
		}
		else{
			AluResult=operand2;
		}
		printf("After MOVING, value is: %d\n",AluResult);
	}

	//Lsl Instruction.
	if((instruction_word>>27) == 10){
		if((instruction_word<<5)>>31){
			AluResult=operand1<<immediate;
		}
		else{
			AluResult=operand1<<operand2;
		}
		printf("After Left Shift, value is: %d\n",AluResult);
	}

	//Lsr Instruction.
	if((instruction_word>>27) == 11){
		if((instruction_word<<5)>>31){
			AluResult=operand1>>immediate;
		}
		else{
			AluResult=operand1>>operand2;
		}
		printf("After Right Shift, value is: %d\n",AluResult);
	}

	//asr Instruction.
	if((instruction_word>>27) == 12){
		if((instruction_word<<5)>>31){
			AluResult=operand1>>immediate;
			if((AluResult!=0) && (operand1>>31) ){
				unsigned int i;
				for(i=32;i<(32-immediate);i--){
					AluResult=AluResult|(1<<i);
				}
			}
		}
		else{
			AluResult=operand1>>operand2;
			if((AluResult!=0) && (operand1>>31) ){
				unsigned int i;
				for(i=32;i<(32-operand2);i--){
					AluResult=AluResult|(1<<i);
				}
			}
		}
		printf("After asr value is: %d\n",AluResult);
	}

	//nop Instruction.
	if((instruction_word>>27) == 13){
		;
	}

	//Ld Instruction.
	if((instruction_word>>27) == 14){
			AluResult=operand1+immediate;
			printf("Load value is: %d\n",AluResult);
	}

	//st Instruction.
	if((instruction_word>>27) == 15){
			AluResult=operand1+immediate;
			printf("Value that is stored is: %d\n",AluResult);
	}

	//beq Instruction.
	if((instruction_word>>27) == 16){
		if(eq){
			PC+=(branchtgt-4);
			printf("Beq executed value of PC: %d\n",PC);
		}
	}

	//bgt Instruction.
	if((instruction_word>>27) == 17){
		if(gt){
			PC+=(branchtgt-4);
			printf("Bgt executed value of PC: %d\n",PC);
		}
	}

	//b Instruction.
	if((instruction_word>>27) == 18){
		PC+=(branchtgt-4);
		printf("Branch executed value of PC: %d\n",PC);
	}

	//call Instruction.
	if((instruction_word>>27) == 19){
		R[15]=PC;
		PC+=(branchtgt-4);
		printf("Function called value of PC: %d\n",PC);
	}

	//ret Instruction.
	if((instruction_word>>27) == 20){
		PC=R[15];
		printf("ret executed value of PC: %d\n",PC);
	}
	printf("----------------EXECUTE ENDS-------------------\n");

}

//perform the memory operation
void mem() {
	printf("----------------MEM STARTS-------------------\n");
	//Ld Instruction.
	if((instruction_word>>27) == 14){             // checking if isLd is 1 or 0.
		Ldresult=MEM[4000-AluResult];
	        printf("Value loaded from memory: %d\n",Ldresult);
	}

	//st Instruction.
	if((instruction_word>>27) == 15){             // checking if isSt is 1 or 0.
		MEM[4000-AluResult]=operand2;
		printf("Value loaded in to memory: %d\n",operand2);
	}
	printf("----------------MEM ENDS-------------------\n");
}
//writes the results back to register file
void write_back() {
	printf("----------------WRITE BACK STARTS-------------------\n");
	unsigned int temp=(instruction_word>>27),reg;
	if((temp!=5)&&(temp!=13)&&(temp!=15)&&(temp!=16)&&(temp!=17)&&(temp!=18)&&(temp!=20)){           // Checking if isWb is 1 or 0.
		if((instruction_word>>27) == 19){
			reg=15;
		}
		else{
			reg=(instruction_word<<6)>>28;
		}
		if((instruction_word>>27) != 19 && (instruction_word>>27) != 14){                    // checking if isLd and isCall is 1 or 0 and act accordingly.
			R[reg]=AluResult;
			printf("Write in Register %d and value written: %d\n",reg,AluResult);
		}
		else if((instruction_word>>27) == 14){              //Last case is taken care before.
			R[reg]=Ldresult;
			printf("Write in Register %d and value written: %d\n",reg,Ldresult);
		}
		else if((instruction_word>>27) == 19){              
			printf("Write in Register 15 and value written: %d\n",PC);
		}
	}
	printf("----------------WRITE BACK ENDS-------------------\n");
}

int read_word(char *mem, unsigned int address) {
  int *data;
  data =  (int*) (mem + address);
  return *data;
}

void write_word(char *mem, unsigned int address, unsigned int data) {
  int *data_p;
  data_p = (int*) (mem + address);
  *data_p = data;
}
