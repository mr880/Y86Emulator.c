/*Y86 Emulator*/
/*Michael Russo*/
/*mr880*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <limits.h>
#include "y86emul.h"

/******Global Variables*******/

//this is memory
int* emem;
//%eax,%ecx,%edx,%ebx,%esp,%ebp,%esi,%edi <- held here
int reg[8];
//OF ZF CF
int flagArray[3];
//pop
int pop; 
//Status code : AOK, HTL, ADR, INS
int cpuState = 0;
//program counter address
int counterAddr;
//op1 value
int op1Val;
//global temp value
int temp2;
//Where instructions end
int closingVariable;
//store mem size for ADR 
int totalLength;
//instructions
int curInstruction;

//hex 2 decimal
int hex2Dec(char* arr, int i, int size) {

	//exponent var
	int exp = size - 1;
	//return var
	int val = 0;

	//loops calculations for conversion
	while (isalnum(arr[i]) != 0) {

		char temp = arr[i];

		if(temp == '0'){
			val = val;
		}
		else if(temp == '1'){
			val = val + pow(16, exp);
			
		}
		else if(temp == '2'){
			val = val + (2 * pow(16, exp));
			
		}
		else if(temp == '3'){
			val = val + (3 * pow(16, exp));
			
		}
		else if(temp == '4'){
			val = val + (4 * pow(16, exp));
			
		}
		else if(temp == '5'){
			val = val + (5 * pow(16, exp));
			
		}
		else if(temp == '6'){
			val = val + (6 * pow(16, exp));
			
		}
		else if(temp == '7'){
			val = val + (7 * pow(16, exp));
			
		}
		else if(temp == '8'){
			val = val + (8 * pow(16, exp));
			
		}
		else if(temp == '9'){
			val = val + (9 * pow(16, exp));
			
		}
		else if(temp == 'a'||temp =='A'){
			val = val + (10 * pow(16, exp));
			
		}
		else if(temp == 'b'||temp =='B'){
			val = val + (11 * pow(16, exp));
			
		}
		else if(temp == 'c'||temp =='C'){
			val = val + (12 * pow(16, exp));
			
		}
		else if(temp == 'd'||temp =='D'){
			val = val + (13 * pow(16, exp));
			
		}
		else if(temp == 'e'||temp =='E'){
			val = val + (14 * pow(16, exp));
			
		}
		else if(temp == 'f'||temp =='F'){
			val = val + (15 * pow(16, exp));
			
		}

		else{
			fprintf(stderr, "ERROR: Invalid Hex!\n");
			exit(1);
			break;
		}
		i++;
		exp--;
	}
	return val;

}

//char 2 decimal
int char2Dec(char* arr, int i, int sizeVar) {

	//exponent
	int exp = sizeVar - 1;
	//return variable 
	int val = 0;

	//loop to handle conversions
	while (isalnum(arr[i]) != 0) {

		char temp = arr[i];

		if(temp == '0'){
			val = val;
			
		}
		else if(temp == '1'){
			val = val + pow(10, exp);
			
		}
		else if(temp == '2'){
			val = val + (2 * pow(10, exp));
			
		}
		else if(temp == '3'){
			val = val + (3 * pow(10, exp));
		
		}
		else if(temp == '4'){
			val = val + (4 * pow(10, exp));
		
		}
		else if(temp == '5'){
			val = val + (5 * pow(10, exp));
			
		}
		else if(temp == '6'){
			val = val + (6 * pow(10, exp));
			
		}
		else if(temp == '7'){
			val = val + (7 * pow(10, exp));
		
		}
		else if(temp == '8'){
			val = val + (8 * pow(10, exp));
		
		}
		else if(temp == '9'){
			val = val + (9 * pow(10, exp));
			
		}
		else{
			fprintf(stderr, "ERROR: Invalid Dec!\n");
			exit(1);
			break;
		}
		i++;
		exp--;
	}
	return val;
}
//prints our errors for the program
void errorMessages() {

	switch(cpuState){
		
		case 0:
			printf("AOK: No errors!\n");
			break;
		case 1:
			printf("HLT: Halt Execution!\n");
			break;
		case 2:
			printf("ADR: Wrong Address Entered!\n");
			break;
		case 3:	
			printf("INS: Incorrect Instructions\n");
			break;
	}

}
//program counter validity check
int addressChecker(int pVar) {
	//keeps in range
	if ((pVar >= 0) && (pVar < totalLength)){
		return 0;
	}

	else {

		cpuState = 2;
		return 1;
	}
}
//validity error check
int instructionCheck(int num) {

	if (num != 0) {
		cpuState = 3;
		return 1;
	}
	else
		return 0;
}
//gets half of instructions by conversion
int dec2Hex() {

	//return variable
	int retVal = (curInstruction % 16);
	//perform maths
	curInstruction = curInstruction - retVal;
	//divide by 16
	curInstruction = curInstruction / 16;
	//return retVal;
	return retVal;

}
//Bit Shift Function
int shiftTheBits() {

	//basic variables x and y
	int x = 3;
	int y = 0;
	int val = 0; //return value
	int pVar = counterAddr + 3;

	//traverse the 4 bytes
	while (y < 4){

		val |= emem[pVar - y] << (x * 8);
		x--;
		y++;
	}
	return val;
}
//32 bit constant value
int getBitConstants(int position) {

	//initiate variables
	int x, y;
	int q = 0;
	int r = 0;
	//return value
	int val = 0;
	//recreate first half and second half
	int section1, section2;
	//32 bit array
	int arrC[] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	//traverse bits
	for(x = 3; x != -1; x--){
		
		curInstruction = emem[position + x];
		section2 = dec2Hex();
		section1 = dec2Hex();
		arrC[q] = section1;
		q++;
		arrC[q] = section2;
		q++;
	}
	//int array to return var
	for( y = 7; y != -1; y--){
		val = val + (arrC[r] * pow(16, y));
		r++;
	}
	return val;

}
//32 bit from int value 
void int2bits(int place, int regHolder) {

	//array for hexadecimal values
	int arrX[] = {0, 0, 0, 0, 0, 0, 0, 0};
	int remainder = 0;
	int i, j;
	int temp;
	int p = 0;

	//dec2hex
	for(i=7; i != -1; i--){
		
		remainder = regHolder % 16;
		regHolder = regHolder - remainder;
		arrX[i] = remainder;
		regHolder = regHolder / 16;
	}

	//hex2dec
	for(j = 7; j != -1; j = j -2){
		
		temp = ((arrX[j - 1] * 16) + arrX[j]);
		emem[place + p] = temp;
		p++;
	}
}
//Flags
void flagSet(int section2, int reg1, int reg2) {

	//initialize flags
	flagArray[0] = 0;
	flagArray[1] = 0;
	flagArray[2] = 0;

	//zero check
	if (op1Val == 0) {

		flagArray[0] = 0; 
		flagArray[1] = 1;
		flagArray[2] = 0;
	}
	//negative check
	else if (op1Val < 0) {

		flagArray[0] = 0;
		flagArray[1] = 0;
		flagArray[2] = 1;
	}
	//addl
	if (section2 == 0) {
		//check for overflow
		if (((reg2 > INT_MAX - reg1)) || ((reg1 < 0) && (reg2 < INT_MIN - reg1) && (reg1 > 0))) {

			flagArray[0] = 1;
			flagArray[1] = 0;
			flagArray[2] = 0;
		}
	}
	//READX
	else if (section2 == 12) {

		flagArray[0] = 0;
		flagArray[1] = 0;
		flagArray[2] = 0;
	}
	//mull
	else if (section2 == 4) {
		//check for overflow
		if ((reg1 != 0) && ((op1Val / reg1) != reg2)) {

			flagArray[0] = 1;
			flagArray[1] = 0;
			flagArray[2] = 0;
		}
	}
	//subl
	else if (section2 == 1 || section2 == 5) {
		//check for overflow
		if (((reg2 < 0) && (reg1 < INT_MIN - reg2)) || ((reg2 > 0) && (reg1 > INT_MAX - reg2))) {

			flagArray[0] = 1;
			flagArray[1] = 0;
			flagArray[2] = 0;
		}
	}
	//andl & xorl
	else if (section2 == 2 || section2 == 3){
		flagArray[0] = 0;
	}
}
//pop
void popA() {

	int position = reg[4];
	//32 bit array vvv
	int arrC[] = {0, 0, 0, 0, 0, 0, 0, 0};

	//initiate variables
	int x, y;
	int q = 0;
	int r = 0;
	//return value
	int val = 0;
	//recreate first half and second half
	int section1, section2;

	//traverse 32 bits
	for(x = 3; x != -1; x--){
		
		curInstruction = emem[position + x];
		
		section2 = dec2Hex();
		
		section1 = dec2Hex();

		arrC[q] = section1;
		q++;
		arrC[q] = section2;
		q++;
	}
	//array to return var
	for( y = 7; y != -1; y--){
		val = val + (arrC[r] * pow(16, y));
		r++;
	}
	//pop
	pop = val;
	reg[4] = reg[4] + 4;

}
//add 
void addl_(int harbor, int regHolder, int addrB){

	//perform register addition
	op1Val = reg[harbor] + reg[regHolder];
	//set flags/ check overflow
	flagSet(addrB, reg[harbor], reg[regHolder]);
	//final value to harbor register
	reg[harbor] = op1Val;

	return;
}
//sub 
void subl_(int harbor, int regHolder, int addrB){

	//perform register subtraction
	op1Val = reg[harbor] - reg[regHolder];
	//set flags/ check overflow
	flagSet(addrB, reg[harbor], reg[regHolder]);
	//save the final value in the harbor register
	reg[harbor] = op1Val;

	return;
}
//and 
void andl_(int harbor, int regHolder, int addrB){

	op1Val = reg[harbor] & reg[regHolder];
	//set flags/ check overflow
	flagSet(addrB, reg[harbor], reg[regHolder]);
	//save the final value in the harbor register
	reg[harbor] = op1Val;
	return;
}
//xorl 
void xorl_(int harbor, int regHolder, int addrB){

	op1Val = reg[harbor] ^ reg[regHolder];
	//set flags/ check overflow
	flagSet(addrB, reg[harbor], reg[regHolder]);
	//final value to harbor register
	reg[harbor] = op1Val;
	return;
}
//multiplication function
void mull_(int harbor, int regHolder, int addrB){

	//register multiplication
	op1Val = reg[harbor] * reg[regHolder];
	//set flags/ check overflow
	flagSet(addrB, reg[harbor], reg[regHolder]);
	//save the final value in the harbor register
	reg[harbor] = op1Val;
	return;
}
//compare funciton
void cmpl_(int harbor, int regHolder, int addrB){

	//cmp on registers
	op1Val = reg[harbor] - reg[regHolder];
	//set flags/ check overflow
	flagSet(addrB, reg[harbor], reg[regHolder]);
	return;
}
//arbitrary check
int check1(int addrB){
	
	//make sure addressB is 0
	if (instructionCheck(addrB) != 0)
		return 0;

	counterAddr++;

	//check validity
	if (addressChecker(counterAddr) != 0)
		return 0;

	//next instruciton...
	curInstruction = emem[counterAddr];
	

	return 1;
}
//runs the program!
void runProg() {

	//first int part of the retrieved address and Second
	int addrA, addrB;
	int temp;
	int regHolder;
	int harbor;
	
	//run loop to pass through functions
	while (counterAddr != closingVariable) {


		//instruction
		curInstruction = emem[counterAddr];
		//second half of instructions
		addrB = dec2Hex();
		//first half of instructions
		addrA = dec2Hex();

		//error check
		if (addressChecker(counterAddr) != 0)
			break;

		//NOPE
		if (addrA == 0) {

			//error check
			if (instructionCheck(addrB) == 0)
				counterAddr++;

			else
			
				break;

		}

		//HALT
		else if (addrA == 1) {

			//error check
			if (instructionCheck(addrB) != 0)
				//exit
				break;

			else
				//status = HLT
				cpuState = 1;
			
				break;

		}

		//RRMOVL
		else if (addrA == 2) {
			
			int aaa;
			aaa = check1(addrB);
			if (aaa==0)
				break;
			
			harbor = dec2Hex();
			//find register variable
			regHolder = dec2Hex();

			//move info in registers
			reg[harbor] = reg[regHolder];
		
			counterAddr++;
			

		}

		//IRMOVL 
		else if (addrA == 3) {
			
			int aab;
			aab = check1(addrB);
			if (aab == 0)
				break;

			//find the placement variable
			harbor = dec2Hex();
			//get register var
			regHolder = dec2Hex();
			
			counterAddr++;

			//Check the bits
			if (addressChecker(counterAddr) != 0)
				break;
			else if (addressChecker(counterAddr + 1) != 0)
				break;
			else if (addressChecker(counterAddr + 2) != 0)
				break;
			else if (addressChecker(counterAddr + 3) != 0)
				break;
			

			
			reg[harbor] = getBitConstants(counterAddr);

			counterAddr++;
			counterAddr++;
			counterAddr++;
			counterAddr++;


		}

		//RMMOVL
		else if (addrA == 4) {
			
			int aac;
			aac = check1(addrB);
			if(aac==0)
				break;

			
			//find the placement 
			harbor = dec2Hex();
			//find the register 
			regHolder = dec2Hex();
		
			counterAddr++;

			//Check the bits
			if (addressChecker(counterAddr) != 0)
				break;
			else if (addressChecker(counterAddr + 1) != 0)
				break;
			else if (addressChecker(counterAddr + 2) != 0)
				break;
			else if (addressChecker(counterAddr + 3) != 0)
				break;

			temp = shiftTheBits();

			//add register address to 32 bit value
			temp = temp + reg[harbor];

			//make sure that the address being called is valid
			if (addressChecker(temp) != 0)
				break;

			//save value at temp
			emem[temp] = reg[regHolder];

	
			counterAddr++;
			counterAddr++;
			counterAddr++;
			counterAddr++;

		}

		//MRMOVL
		else if (addrA == 5) {
			
			int aad;
			aad = check1(addrB);
			if (aad == 0)
				break;

			harbor = dec2Hex();
			//get register var
			regHolder = dec2Hex();
			
			counterAddr++;

			//Check the bits
			if (addressChecker(counterAddr) != 0)
				break;
			else if (addressChecker(counterAddr + 1) != 0)
				break;
			else if (addressChecker(counterAddr + 2) != 0)
				break;
			else if (addressChecker(counterAddr + 3) != 0)
				break;

			temp = shiftTheBits();
			//add register address to 32 bit value
			temp = temp + reg[harbor];

			//error check
			if (addressChecker(temp) != 0)
				break;

			//place into register
			reg[regHolder] = emem[temp];

			counterAddr++;
			counterAddr++;
			counterAddr++;
			counterAddr++;

		}

		//OP1
		else if (addrA == 6) {

			counterAddr++;
			//CHECK address
			if (addressChecker(counterAddr) != 0)
				break;

			//next instruciton...
			curInstruction = emem[counterAddr];
			//find placement
			harbor = dec2Hex();
			//get register var
			regHolder = dec2Hex();

			
			switch(addrB){
				
				case 0: 
					addl_(harbor, regHolder, addrB);
					break;
				case 1:
					subl_(harbor, regHolder, addrB);
					break;
				case 2:
					andl_(harbor, regHolder, addrB);
					break;
				case 3:
					xorl_(harbor, regHolder, addrB);
					break;
				case 4:
					mull_(harbor, regHolder, addrB);
					break;
				case 5:
					cmpl_(harbor, regHolder, addrB);
					break;
			}
		
			counterAddr++;

		}

		//JXX
		else if (addrA == 7) {

			counterAddr++;

			//Check bits
			if (addressChecker(counterAddr) != 0)
				break;
			else if (addressChecker(counterAddr + 1) != 0)
				break;
			else if (addressChecker(counterAddr + 2) != 0)
				break;
			else if (addressChecker(counterAddr + 3) != 0)
				break;

			temp = shiftTheBits();

			//error check
			if (addressChecker(temp) != 0)
				break;

			//JMP
			if (addrB == 0) {

				//counter
				counterAddr = temp;
				continue;
			}

			//JLE
			else if (addrB == 1) {

				//error check
				if (flagArray[2] == 1 || flagArray[1] == 1) {

					//counter
					counterAddr = temp;
					continue;

				}

			}

			//JL
			else if (addrB == 2) {

				//error check
				if (flagArray[2] == 1) {

					//counter
					counterAddr = temp;
					continue;

				}

			}

			//JE
			else if (addrB == 3) {

				//error check
				if (flagArray[1] == 1) {
					
					//counter
					counterAddr = temp;
					continue;

				}


			}

			//JNE
			else if (addrB == 4) {

				//error check
				if (flagArray[1] == 0) {

					//counter
					counterAddr = temp;
					continue;

				}

			}

			//JGE
			else if (addrB == 5) {

				//error check
				if (flagArray[2] == 0) {

					//counter
					counterAddr = temp;
					continue;

				}

			}

			//JG
			else if (addrB == 6) {

				//error check
				if ((flagArray[2] == 0) && (flagArray[1] == 0)) {

					//counter
					counterAddr = temp;
					continue;

				}

			}
			//increase counter by 4
			counterAddr++;
			counterAddr++;
			counterAddr++;
			counterAddr++;

		}

		//CALL
		else if (addrA == 8) {
			//error check
			if (instructionCheck(addrB) != 0)
				break;

			counterAddr++;
			int temp = shiftTheBits();
			counterAddr+=4;
			//push
			reg[4] = reg[4] - 4;
			//put bits into register
			int2bits(reg[4], counterAddr);
			counterAddr = temp;
		}

		//RET
		else if (addrA == 9) {

			//error check
			if (instructionCheck(addrB) != 0)
				break;

			//set program counter back to original value using 'pop' function
			popA();

			counterAddr = pop;

		}

		//PUSHL
		else if (addrA == 10) {

			//make sure addrB is 0
			if (instructionCheck(addrB) != 0)
				break;

			counterAddr++;

			//error check
			if (addressChecker(counterAddr) != 0)
				break;

			//get next program instruction
			curInstruction = emem[counterAddr];
			//get harbor value
			harbor = dec2Hex();
			//get register var
			regHolder = dec2Hex();

			//Push
			reg[4] = reg[4] - 4;
			//store new 32 bit value into register 4, ESP
			int2bits(reg[4], (reg[regHolder]));

			counterAddr++;

		}

		//POPL
		else if (addrA == 11) {

			//error check
			if (instructionCheck(addrB) != 0)
				break;

			counterAddr++;

			//error check
			if (addressChecker(counterAddr) != 0)
				break;

			//get next program instruction
			curInstruction = emem[counterAddr];
			//get harbor variable
			harbor = dec2Hex();
			//get register var
			regHolder = dec2Hex();

			//pop
			popA();

			//store the value in the source register
			reg[regHolder] = pop;


			counterAddr++;

		}

		//READX
		else if (addrA == 12) {

			int ZF;
			char storeChar;
			int storeInt;

			counterAddr++;

			//error check
			if (addressChecker(counterAddr) != 0)
				break;

			//next instruction...
			curInstruction = emem[counterAddr];
			//get harbor variable
			harbor = dec2Hex();
			//get register var
			regHolder = dec2Hex();

		
			counterAddr++;

			//check 32-bit addresses are correct
			if (addressChecker(counterAddr) != 0)
				break;
			else if (addressChecker(counterAddr + 1) != 0)
				break;
			else if (addressChecker(counterAddr + 2) != 0)
				break;
			else if (addressChecker(counterAddr + 3) != 0)
				break;

			temp = getBitConstants(counterAddr);

			//add register address to 32 bit value
			temp = temp + reg[regHolder];

			//set the appropriate flags
			flagSet(addrA, reg[harbor], reg[regHolder]);

			//READB
			if (addrB == 0) {
				//Zero Flag
				ZF = scanf("%c", &storeChar);
			
				if (ZF == EOF){
					flagArray[1] = 1;
				}

				else {

					//error check
					if (addressChecker(temp) != 0){
						break;
					}

					emem[temp] = storeChar;

				}

			}

			//READL
			else if (addrB == 1) {

				//ZF check
				ZF = scanf("%d", &storeInt);

				if (ZF == EOF)
					//set zero flag
					flagArray[1] = 1;

				else {

					//error check
					if (addressChecker(temp) != 0)
						break;

					//set int to bits
					int2bits(temp, storeInt);

				}

			}

			counterAddr = counterAddr + 4;

		}

		//WRITEX
		else if (addrA == 13) {

			counterAddr++;

			//check
			if (addressChecker(counterAddr) != 0)
				break;

			//get next program instruction
			curInstruction = emem[counterAddr];
			//get harbor variable
			harbor = dec2Hex();
			//get register var
			regHolder = dec2Hex();

			counterAddr++;

			//check 32-bit addresses are correct
			if (addressChecker(counterAddr) != 0)
				break;
			else if (addressChecker(counterAddr + 1) != 0)
				break;
			else if (addressChecker(counterAddr + 2) != 0)
				break;
			else if (addressChecker(counterAddr + 3) != 0)
				break;

			temp = getBitConstants(counterAddr);

			//add register address to 32 bit value
			temp = temp + reg[regHolder];

			//check
			if (addressChecker(temp) != 0)
				break;

			//WRITEB
			if (addrB == 0)
			
				printf("%c", emem[temp]);

			//WRITEL
			else if (addrB == 1)
		
				printf("%d", emem[temp]);

		
			counterAddr = counterAddr + 4;

		}

		//MOVSBL
		else if (addrA == 14) {

			//check Addr for 0
			if (instructionCheck(addrB) != 0)
				break;

			counterAddr++;
			//check register address
			if (addressChecker(counterAddr) != 0){
				break;
			}

			//next instruction...
			curInstruction = emem[counterAddr];
			//get harbor
			harbor = dec2Hex();
			//get register
			regHolder = dec2Hex();

			counterAddr++;

			//Check the bits
			if (addressChecker(counterAddr) != 0)
				break;
			else if (addressChecker(counterAddr + 1) != 0)
				break;
			else if (addressChecker(counterAddr + 2) != 0)
				break;
			else if (addressChecker(counterAddr + 3) != 0)
				break;

			temp = getBitConstants(counterAddr);

			//add register address to 32 bit value
			temp = temp + reg[harbor];

			//error check
			if (addressChecker(temp) != 0)
				break;

			//place into reg
			reg[regHolder] = getBitConstants(temp);

			counterAddr++;
			counterAddr++;
			counterAddr++;
			counterAddr++;


		}
		else {

			//INS
			cpuState = 3;
			break;

		}
	}
}
//main function
int main(int argc, char **argv) {

	//temp mem var
	int tempMem;
	//temporary i variable
	int tempi;
	//extra temp variable
	int tempVar;
	// array for file lines
	char lineArr[4096];
	//text counter
	int textCount = 0;
	//size variable
	int m = 0;
	
	

	//nitial argument
	if (argc <= 1) {

		fprintf(stderr, "ERROR: No Argument Detected! Enter '-h' for Help.)\n");
		exit(1);
	}

	//multiple arguments
	else if (argc >= 3) {

		fprintf(stderr, "ERROR: Too Many Arguments! Enter '-h' for Help.\n");
		exit(1);
	}

	//help directives provided here
	if (strcmp(argv[1], "-h") == 0) {

		printf( "Usage: y86emul <y86 input file>\n" );
		exit(0);
	}

	//pointer to our Y86 file
	FILE *fp;
	//open Y86 file
	fp = fopen(argv[1], "r");
	
	if (fp == NULL) {

		fprintf(stderr, "ERROR: Missing File! Enter '-h' for Help.\n");
		exit(1);

	}
	int i;
	//first line
	if (fgets(lineArr, 4096, fp) != NULL) {

		//.size
		if (memcmp(lineArr, ".size", 5) == 0) {
			
			int v = 6;
			int sizer = 0;
			for(v=6; lineArr[v] != '\0'; v++){
				sizer++;
			}
			sizer--;
			v = 6;

			m = hex2Dec(lineArr, v, sizer);
			//find size of emem to be stored
			totalLength = m;

			if (m >= 1)
				emem = (int*)malloc(sizeof(int) * m);
		}
		else {

			fprintf(stderr, ".SIZE ERROR: directive absent from first line!\n");
			exit(1);
		}
	}
	//beging finding lines after the first
	while (fgets(lineArr, 4096, fp) != NULL) {

		//.string
		if (memcmp(lineArr, ".string", 7) == 0) {

			//set up i
			i = 7;

			//must start with tab
			if (lineArr[i] == '\t') {
				
				i++;
				//set size back to 0
				m = 0;
				//locate end of addr
				while (lineArr[i] != '\t') {

					m++;
					i++;

				}
				i++;
				//hex2Dec
				tempMem = hex2Dec(lineArr, 8, m);

			}
			else {

				fprintf(stderr, ".String ERROR: No tab detected!\n");
				exit(1);

			}
			//check that we start with a '"'
			if (lineArr[i] == '\"') {
				
				i++;
				//loop until the end
				while (lineArr[i] != '\"') {

					emem[tempMem] = lineArr[i];
					i++;
					tempMem++;
				}
			}
			else {

				fprintf(stderr, ".STRING ERROR: No quotes detected!\n");
				exit(1);
			}
		}
		//.long
		else if (memcmp(lineArr, ".long", 5) == 0) {
		
			i = 5;

			if (lineArr[i] == '\t') {
				
				i++;
				//reset size back to 0
				m = 0;
				//locate end of addr
				while (lineArr[i] != '\t') {

					m++;
					i++;

				}

				i++;
				//hex2dec
				tempMem = hex2Dec(lineArr, 6, m);

			}
			else {

				fprintf(stderr, ".LONG ERROR: No tab detected!\n");
				exit(1);

			}
			//reset size
			m = 0;
			tempi = i;

			while (isalnum(lineArr[i]) != 0) {

				m++;
				i++;
			}
			i++;
			tempVar = char2Dec(lineArr, tempi, m);
			emem[tempMem] = tempVar;

		}
		// byte check
		else if (memcmp(lineArr, ".byte", 5) == 0) {

			//start with i after byte
			i = 5;
			//check for the necessary tab
			if (lineArr[i] == '\t') {
				i++;
				m = 0;
				
				//find end of address
				while (lineArr[i] != '\t') {

					m++;
					i++;

				}
				i++;
				//Hex2Dec
				tempMem = hex2Dec(lineArr, 6, m);
			}

			else {
				fprintf(stderr, ".BYTE ERROR: tab not detected!\n");
				exit(1);
			}

			//reset
			m = 0;
			tempi = i;

			while (isalnum(lineArr[i]) != 0) {

				m++;
				i++;
			}
			i++;
			tempVar = hex2Dec(lineArr, tempi, m);
			emem[tempMem] = tempVar;

		}

		//.text check
		else if ((memcmp(lineArr, ".text", 5) == 0)) {

			//.text check
			if (textCount >= 2) {

				fprintf(stderr, ".TEXT ERROR: multiple directives!\n");
				exit(1);

			}
			i = 5;

			textCount++;

			if (lineArr[i] == '\t') {
				
				i++;
				//set size back to 0
				m = 0;
				//locate end of addr
				while (lineArr[i] != '\t') {

					m++;
					i++;

				}
				i++;
				//hex2Dec
				tempMem = hex2Dec(lineArr, 6, m);

			}

			else {

				fprintf(stderr, ".TEXT ERROR: No tab detected!\n");
				exit(1);

			}
			//reset
			counterAddr = tempMem;

			//seperate bits
			while(isalnum(lineArr[i]) != 0) {

				tempVar = hex2Dec(lineArr, i, 2);
				//store in mem
				emem[tempMem] = tempVar;
				
				i++;
				i++;
				tempMem++;
			}
			// create the closing varible 
			closingVariable = tempMem;

		}
		else {

			fprintf(stderr, "ERROR: Incorrect Directive or Missing!\n");
			exit(1);

		}
	}
	//run
	runProg();
	//report error messages
	errorMessages();
	//close file
	fclose(fp);
	//free up memory
	free(emem);
	//l'exit
	exit(0);

}