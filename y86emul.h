#ifndef Y86EMUL_H
#define Y86EMUL_H

int hex2Dec(char* arr, int i, int size);

int char2Dec(char* arr, int i, int size);

void errorMessages();

int addressCheck(int pVar);

int instructionCheck(int num);

int dec2Hex();

int shiftTheBits();

int getConstant(int position);

void int2bits(int dest, int regHolder);

void flagSet(int section2, int reg1, int reg2);

void popA();

void addl_(int harbor, int regHolder, int addrB);

void subl_(int harbor, int regHolder, int addrB);

void andl_(int harbor, int regHolder, int addrB);

void xorl_(int harbor, int regHolder, int addrB);

void mull_(int harbor, int regHolder, int addrB);

void cmpl_(int harbor, int regHolder, int addrB);

int check1(int addrB);

void runProg();

int main(int argc, char **argv);
#endif