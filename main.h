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
#include <conio.h>
#include <time.h>
//Define crt signal for SIGINT control C handling
typedef void (__cdecl *_crt_signal_t)(int);

//Macros to mask all but a specific number of bits
#define BIT(x) ((inst>>(x))&0x1)
#define BIT2(x) ((inst>>(x))&0x3)
#define BIT3(x) ((inst>>(x))&0x7)
#define BIT4(x) ((inst>>(x))&0xF)

//Macros for specific faults
#define ILLEGAL_INST_FLT 8
#define INVALID_ADDR_FLT 9
#define PRIORITY_FLT 10
#define DOUBLE_FLT 11

//Shortcut to BP register
#define BP registers[REG][12].word
//Shortcut to LR register
#define LR registers[REG][13].word
//Shortcut to SP register
#define SP registers[REG][14].word
//Shortcut to PC register
#define PC registers[REG][15].word

//Shortcut to timer CSR
#define TIMER_CSR memory.byte[0x0000]
//Shortcut to timer DR
#define TIMER_DR memory.byte[0x0001]
//Shortcut to keyboard CSR
#define KEYBOARD_CSR memory.byte[0x0002]
//Shortcut to keyboard DR
#define KEYBOARD_DR memory.byte[0x0003]
//Shortcut to clock CSR
#define SCREEN_CSR memory.byte[0x0004]
//Shortcut to clock DR
#define SCREEN_DR memory.byte[0x0005]

//Device get macros
//Macro for getting ENA of CSR
#define ENA(x) (((x)>>4)&0x01)
//Macro for getting OF of CSR
#define OF(x) (((x)>>3)&0x01)
//Macro for getting DBA of CSR
#define DBA(x) (((x)>>2)&0x01)
//Macro for getting I/O of CSR
#define IO(x) (((x)>>1)&0x01)
//Macro for getting IE of CSR
#define IE(x) ((x)&0x01)

//Macro for 50ms
#define DELAY 500

//Device set macros
//Macro for getting ENA of CSR
#define SET_ENA 0b00010000
//Macro for getting OF of CSR
#define SET_OF 0b00001000
//Macro for getting DBA of CSR
#define SET_DBA 0b00000100
//Macro for getting I/O of CSR
#define SET_IO 0b00000010
//Macro for getting IE of CSR
#define SET_IE 0b00000001

//Shortcut to send PSW structure
//Each piece is shifted to their proper position in a word
#define PSW_WORD ((PSW.C) | (PSW.Z << 1) | (PSW.N << 2) | \
(PSW.SLP << 3) | (PSW.V << 4) | (PSW.Current_Priority << 5) | \
(PSW.FLT << 8) | (PSW.Previous_Priority << 13))

//Shortcut to send CEX structure
//Each piece is shifted to the proper position in a word
#define CEX_WORD ((CEXstruct.Fcounter) | (CEXstruct.Tcounter << 3) | (CEXstruct.state << 6))

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

//Macro to determine start of interrupt vectors
#define INT_VECT_START (0xffc0>>1)
//Macro to give priority of an interrupt vector's PSW
#define INT_VECT_CURRENT_PRIORITY ((INT_VECT>>5)&0x7)
//Macro to get bits from temporary PSW pulled from memory
#define INT_VECT_BIT(x) ((INT_VECT>>(x))&0x01)
//Macro to define interrupt address
#define INT_VECT_ADDRESS (INT_VECT_START + BIT4(0) + 2)

//Struct to make PSW bits available globally
typedef struct pswstruct{
    unsigned short V;
    unsigned short N;
    unsigned short Z;
    unsigned short C;
    unsigned short SLP;
    unsigned short Current_Priority;
    unsigned short FLT;
    unsigned short Previous_Priority;
} psw;

extern unsigned int FLT_code;

extern psw PSW;

extern unsigned short temp_Previous_Priority;

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

//Function to execute SETPRI
extern void executeSETPRI(void);

//Function to execute SVC
extern void executeSVC(void);

//Function to execute SETCC
extern void executeSETCC(void);

//Function to execute CLRCC
extern void executeCLRCC(void);

//Function to enter an exception handler
extern void exception(unsigned short address);

//Function to return from exception handler
extern void return_from_exception(void);

//Function to be called to check if faults should be called
extern void check_faults();

//Function to poll for device status
void poll(void);

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