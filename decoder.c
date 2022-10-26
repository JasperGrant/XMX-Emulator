/*
decoder.c
File to define decoding functions of the XMX emulator
Date modified: 2022-07-11
Author: Jasper Grant B00829263
*/
#include "main.h"

//Instruction received from fetch
unsigned short inst;

unsigned int decodeinst(void) {
    //For debug purposes print out instruction
    //printf("Address: %4x, Instruction: %4x\n", PC-2, inst);
    //Increment clock cycles
    cycles++;
    //Enter logic tree to decide instruction based on opcode
    //Each decision is based on the binary values of the Opcode
    switch (BIT3(13)) {
        case 7: //111
            //printf("STR\n");
            return STR;
        case 6: //110
            //printf("LDR\n");
            return LDR;
        case 5: //101
            if ((BIT(12)) == 1) {
                //printf("MOVH\n");
                return MOVH;
            } else {
                //printf("MOVLS\n");
                return MOVLS;
            }
        case 4: //100
            if ((BIT(12)) == 1) {
                //printf("MOVLZ\n");
                return MOVLZ;
            } else {
                //printf("MOVL\n");
                return MOVL;
            }
        case 3://011
            switch (BIT2(10)) {
                case 0: //00
                    //printf("AADD\n");
                    return AADD;
                case 1: //01
                    //printf("ASUB\n");
                    return ASUB;
                case 2: //10
                    //printf("ACMP\n");
                    return ACMP;

            }
            break;
        case 2: //010
            if ((BIT(11)) == 1) {
                //printf("ST\n");
                return ST;
            } else {
                //printf("LD\n");
                return LD;
            }
        case 1: //001
            if ((BIT(12)) == 1) {
                //ADD to BIC
                if ((BIT3(9)) == 0x6) {
                    //printf("MOV\n");
                    return MOV;
                } else if ((BIT3(9)) == 0x7) {
                    //printf("SWAP\n");
                    return SWAP;
                } else {
                    switch (BIT4(8)) {
                        case (0): //0000
                            if ((BIT(7)) == 1) {
                                //printf("SRA\n");
                                return RRC;
                            } else {
                                //printf("RRC\n");
                                return SRA;
                            }
                        case (1): //0001
                            //printf("ADD\n");
                            return ADD;
                        case (2): //0010
                            //printf("ADDC\n");
                            return ADDC;
                        case (3): //0011
                            //printf("SUB\n");
                            return SUB;
                        case (4): //0100
                            //printf("SUBC\n");
                            return SUBC;
                        case (5): //0101
                            //printf("CMP\n");
                            return CMP;
                        case (6): //0110
                            //printf("XOR\n");
                            return XOR;
                        case (7): //0111
                            //printf("AND\n");
                            return AND;
                        case (8): //1000
                            //printf("OR\n");
                            return OR;
                        case (9): //1001
                            //printf("BIT\n");
                            return BIT;
                        case (10): //1010
                            //printf("BIS\n");
                            return BIS;
                        case (11): //1011
                            //printf("BIC\n");
                            return BIC;
                    }
                }
            } else {
                if ((BIT2(10)) == 1) {
                    //printf("CEX\n");
                    return CEX;
                } else if ((BIT2(10)) == 0) {
                    //printf("BR\n");
                    return BR;
                } else {
                    //SWPB to CLRCC
                    switch (BIT3(5)) {
                        case 0: //000
                            if ((BIT(4))) {
                                //printf("SETPRI\n");
                                return SETPRI;
                            } else {
                                if ((BIT(3)) == 1) {
                                    //printf("SKT\n");
                                    return SKT;
                                } else {
                                    //printf("SWPB\n");
                                    return SWPB;
                                }
                            }
                        case 1: //001
                            //printf("SVC\n");
                            return SVC;
                        case 2: //010
                            //printf("SETCC\n");
                            return SETCC;
                        case 3: //011
                            //printf("CLRCC\n");
                            return CLRCC;
                    }
                }
            }
        case 0: //000
            //printf("BL\n");
            return BL;
    }
    printf("Illegal Instruction Fault Detected\n");
    FLT_code = ILLEGAL_INST_FLT;

}