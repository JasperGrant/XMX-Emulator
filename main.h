/*
main.h
File to act as main header for XMX emulator
Date modified: 2022-06-06
Author: Jasper Grant B00829263
*/


#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

//Define crt signal for SIGINT control C handling
typedef void (__cdecl *_crt_signal_t)(int);

//Macros to mask all but a specific number of bits
#define BIT(x) ((inst>>(x))&0x1)
#define BIT2(x) ((inst>>(x))&0x3)
#define BIT3(x) ((inst>>(x))&0x7)
#define BIT4(x) (inst>>(x))&0xF

//Shortcut to LR register
#define LR registers[REG][13].word

//Shortcut to PC register
#define PC registers[REG][15].word

//Define max array length
#define MAXLEN 100

//Define correction for address being removed from count
#define ADDRESS_CORRECTION 2

//Define correction fro chksum being removed from count
#define CHKSUM_CORRECTION 1

//Define correction going from count in pairs of hex values to characters
#define HEX_TO_TWO_CHARS 2

//Macro to make it clear when a register is only register and not a constant
#define REG 0

//Macros to make it obvious what is going into executeADDSUB, could have been an enum
#define SUBTRACTION 1
#define ADDITION 0
#define NOCARRY 0
#define ALWAYSCARRY 1

//Macro to mask all but destination of Opcode
#define DEST inst&0x7 | (DRAbit << 3)
//Macro to mask all but source of Opcode
#define SC (inst>>3)&0x7 | (SRAbit << 3)
//Macro to get RC bit of Opcode
#define RC BIT(7)
//Macro to get WB bit of Opcode
#define WB BIT(6)
//Macro to get BYTE 8 bits of Opcode
#define BYTE (inst>>3)&0xFF
//Macro to get offset of BL
#define BLOFFSET inst&0x1FFF
//Macro to get offset of BR
#define BROFFSET inst&0x3FF
//Macro to get offset for LDR and STR
#define LDRSTROFFSET (inst>>7)&0x1F
//Macro to aid in sign extension
#define NEGSIGN(x)	(0xFFFF << (x))
//Macro to get condition of CEX Opcode
#define CONDITION (inst>>6)&0xF
//Macro to get true of CEX Opcode
#define IFTRUE (inst>>3)&0x7
//Macro to get false of CEX Opcode
#define IFFALSE inst&0x7
//Macro to get DI bit of opcode
#define DI BIT(10)
//Macro to get PRPO bit of opcode
#define PRPO BIT(8)
//Macro to get ID bit of opcode
#define ID BIT(7)

//Struct to make PSW bits available globally
typedef struct pswstruct{
    unsigned short V;
    unsigned short N;
    unsigned short Z;
    unsigned short C;
} psw;

extern psw PSW;

//Enum to provide, a true, false and off state to conditional execution
enum status{
    TRUE = 1, FALSE = 0, OFF = 2
};

//Struct to make the state, the true count and the false count of condtional execution available globally
struct conditionalexecution{
    enum status state;
    unsigned short Fcounter;
    unsigned short Tcounter;
};

//Globally available 16 bit instructions and clock cycle count
extern unsigned short inst;
extern unsigned long cycles;

//Enum that covers every instruction
enum INST{
    BL, BR, CEX, SWPB, SKT, SETPRI, SVC, SETCC, CLRCC, SRA, RRC, ADD, ADDC, SUB, SUBC, CMP, XOR, AND, OR, BIT, BIS, BIC,
    MOV, SWAP, LD, ST, MOVL, MOVLZ, MOVLS ,MOVH, LDR, STR, AADD, ASUB, ACMP
};

//Function to decode an opcode
//Returns an unsigned integer representing which command was decoded
extern unsigned int decodeinst(void);

//Function to load inputted instructions into memory
//Takes the name of the input file
extern void load(char *filename);

//Function to fetch an instruction from memory
extern void fetchinst(void);

//Function to execute a fetched instruction
//Takes an integer representing an instruction
extern void executeinst(unsigned int decode);

//Functions for execution of each instruction
//Generally have no inputs and no outputs

//Function to execute BL
extern void executeBL(void);

//Function to execute BR
extern void executeBR(void);

//Function to execute ADD
//Takes an unsigned int representing subtraction or addition along with another representing the value of the carry
extern void executeADDSUB(unsigned short subtraction, unsigned short carry);

//Function to execute SWPB
extern void executeSWPB(void);

//Function to execute XOR
extern void executeXOR(void);

//Function to execute AND
extern void executeAND(void);

//Function to execute OR
extern void executeOR(void);

//Function to execute MOVL
extern void executeMOVL(void);

//Function to execute MOVLZ
extern void executeMOVLZ(void);

//Function to execute MOVLS
extern void executeMOVLS(void);

//Function to execute MOVH
extern void executeMOVH(void);

//Function to execute MOV
extern void executeMOV(void);

//Function to execute SWAP
extern void executeSWAP(void);

//Function to execute CEX
extern void executeCEX(void);

//Function to execute SXT
extern void executeSXT(void);

//Function to execute LDR
extern void executeLDR(void);

//Function to execute STR
extern void executeSTR(void);

//Function to execute BIT
extern void executeBIT(void);

//Function to execute BIC or BIS
//Takes a 1 or a 0 to represent whether the bit should be set or cleared
extern void executeBIvalue(unsigned short value);

//Function to execute SRA
extern void executeSRA(void);

//Function to execute RRC
extern void executeRRC(void);

//Function to execute LD
extern void executeLD(void);

//Function to execute ST
extern void executeST(void);

//Lab 3: Accessing Memory

//Declare address and variable to hold data to be used by the bus
extern unsigned short mar, mbr;

//Declare memory that can be accessed as bytes or words
union mem{
    unsigned char byte[0x1000];
    unsigned short word[0x8000];
};

//Declare instance of our memory
extern union mem memory;

//Access byte with memory.byte[MAR]
//Access word with memory.word[MAR>>1]

//Enum to make read or write setting obvious
enum ACTION{
    read, write
};

//Enum to make word or byte setting obvious
enum SIZE{
    word = 0, byte = 1
};

//Function to read or write data from the memory
//Take two enums representing read/write and word/byte
extern void bus(enum ACTION rw, enum SIZE bw);

//Lab 3: The Register File

//Union allowing data to be called as two bytes or a word
union borw{
    unsigned char byte[2];//2 because two bytes make up a word
    unsigned short word;
};

//Array of borws to be used as a register file
extern union borw registers[2][16];

//Function to populate the array or borws with 0s and constants
extern void init_registers(void);

//Lab 3: Updating the PSW

//Function to update the PSW in ADD, SUB, ADDC, and SUBC
//Takes unsigned shorts representinf the operands, results and word/byte of an arithmetic operation
extern void update_psw(unsigned short src, unsigned short dst, unsigned short res, unsigned short wb);

//Function to print out the bits of the PSW
extern void printPSW(void);

//Function to print out all address, data and constant registers
extern void printregisters(void);

//Function to print out all non zero memory
extern void printmemory(void);

//Function to update the PSW in other logical instructions
//Takes the result of an operation and whether it is a word or a byte
extern void update_logic_psw(unsigned short res, unsigned short wb);

//Function to sign extend an offset
//Takes the offset and the positon of it's sign
//Returns the sign extended offset
extern unsigned short sign_ext(unsigned short offset, unsigned short signbit);

//Volatile integer to detect control-c entered
extern volatile sig_atomic_t controlcfound;

//Function to be called when control-c is entered
extern void siginthandler(void);

#endif