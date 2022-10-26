/*
memoryandPSW.c
File to define memory and PSW functions of the XMX emulator
Date modified: 2022-06-06
Author: Jasper Grant B00829263
*/
#include "main.h"

//Specific instance of memory
union mem memory;

//Address and data words to access bus
unsigned short mar, mbr;

//Provides access to PSW bits
extern psw PSW;

//Union defining register files
union borw registers[2][16];

//Carry matrix
unsigned short carry[2][2][2] = {0, 0, 1, 0, 1, 0, 1, 1};

//Overflow matrix
unsigned short overflow[2][2][2] = {0, 1, 0, 0, 0, 0, 1, 0};

void bus(enum ACTION rw, enum SIZE bw) {
    //Increment clock cycles by 3
    cycles += 3;
    /*
    - Bus (and memory access) emulator
    - mar - memory address - refers to a byte location (word is mar >> 1)
    - mbr - pointer to data to read/write
    - rw - READ|WRITE
    - bw - BYTE|WORD
    */

    if (rw == read) {
        //Check for device addresses
        switch (mar) {
            case 0x0000: //Timer CSR
                //In this case is it important to get data before changing bits
                mbr = TIMER_CSR;
                //Set DBA to 0
                TIMER_CSR &=(~SET_DBA);
                //Set OF to 0
                TIMER_CSR &=(~SET_OF);
                return;
            case 0x0002: //Keyboard CSR
                //Set overflow to 0
                KEYBOARD_CSR&=(~SET_OF);
                break;
            case 0x0003: //Keyboard DR
                //Set overflow to 0
                KEYBOARD_CSR&=(~SET_OF);
                break;
            case 0x0004: //Screen CSR
                //Set overflow to 0
                SCREEN_CSR&=(~SET_OF);
                break;
            case 0x0005: //Screen DR
                //Set overflow to 0
                SCREEN_CSR&=(~SET_OF);
                break;
        }
        //If word and even number address
        if (bw == word) {
            mbr = memory.word[mar >> 1];
        }
            //If byte
        else if (bw == byte) {
            mbr = memory.byte[mar];
        } else {
            printf("Word attempted to write to odd number address\n");
            return;
        }
    }
        //If write
    else {
        //Check for device addresses
        switch (mar) {
            /*case 0x0002: //Keyboard CSR
                //Make sure only ENA and IE are written to
                if((mbr|(0x01)|(0x01<<4)) != (KEYBOARD_CSR|(0x01)|(0x01<<4))){
                    printf("Tried to write to read only bits of address 0x0002\n");
                    return;*/
            case 0x0003: //Keyboard DR
                printf("Tried to write to address 0x0003. This is read only\n");
                return;
            /*case 0x0004: //Screen CSR
                //Make sure only ENA and IE are written to
                if((mbr|(0x01)|(0x01<<4)) != (SCREEN_CSR|(0x01)|(0x01<<4))){
                    printf("Tried to write to read only bits of address 0x0004\n");
                    return;
                }*/
            case 0x0005: //Screen DR
                //Set screen.dba to 0
                SCREEN_CSR&=(~SET_DBA);
                break;
        }
        if (bw == word) {
            memory.word[mar >> 1] = mbr;
        } else {
            memory.byte[mar] = mbr;
        }
        //printf("Memory in address %x is now %x\n", mar, memory.word[mar >> 1]);
    }
}

void init_registers(void) {
    //Initialize data registers
    registers[0][0].word = 0;
    registers[0][1].word = 0;
    registers[0][2].word = 0;
    registers[0][3].word = 0;
    registers[0][4].word = 0;
    registers[0][5].word = 0;
    registers[0][6].word = 0;
    registers[0][7].word = 0;

    registers[0][8].word = 0;
    registers[0][9].word = 0;
    registers[0][10].word = 0;
    registers[0][11].word = 0;
    registers[0][12].word = 0;
    registers[0][13].word = 0;
    registers[0][14].word = 0;
    registers[0][15].word = 0;

    registers[1][0].word = 0;
    registers[1][1].word = 1;
    registers[1][2].word = 2;
    registers[1][3].word = 4;
    registers[1][4].word = 8;
    registers[1][5].word = 16;
    registers[1][6].word = 32;
    registers[1][7].word = -1;

    registers[1][8].word = 0;
    registers[1][9].word = 0;
    registers[1][10].word = 0;
    registers[1][11].word = 0;
    registers[1][12].word = 0;
    registers[1][13].word = 0;
    registers[1][14].word = 0;
    registers[1][15].word = 0;

}

void update_psw(unsigned short src, unsigned short dst, unsigned short res, unsigned short wb) {
    /*
    - Update the PSW bits (V, N, Z, C)
    - Using src, dst, and res values and whether word or byte
    - ADD, ADDC, SUB, and SUBC
    */
    //Z
    if (wb == 0) {
        PSW.Z = (res == 0);
    } else {
        PSW.Z = ((res & 0xFF) == 0);
    }

    //All others require MSBres, so may as well calculate all three values here to minimize repeated code.
    unsigned int MSBsrc, MSBdst, MSBres;
    if (wb == 0) {
        MSBsrc = (src >> 15) & 0x01;
        MSBdst = (dst >> 15) & 0x01;
        MSBres = (res >> 15) & 0x01;
    } else {
        MSBsrc = (src >> 7) & 0x01;
        MSBdst = (dst >> 7) & 0x01;
        MSBres = (res >> 7) & 0x01;
    }

    //V
    //Find overflow from array
    PSW.V = overflow[MSBsrc][MSBdst][MSBres];

    //N
    PSW.N = MSBres;

    //C
    //Find carry from array
    PSW.C = carry[MSBsrc][MSBdst][MSBres];
}

void printPSW(void) {
    printf("Bits: V: %d, N: %d, Z: %d, C: %d, SLP: %d, FLT: %d\n", PSW.V, PSW.N, PSW.Z, PSW.C, PSW.SLP, PSW.FLT);
}

void update_logic_psw(unsigned short res, unsigned short wb) {
    if (wb == 0) {
        PSW.N = (res >> 15) & 0x01;
    } else {
        PSW.N = (res >> 7) & 0x01;
    }
    PSW.Z = (res == 0);

}

void printregisters(void) {
    //Loop 8 times through each set of data, address, and constant addresses to display for debugging
    for (unsigned short i = 0; i < 8; i++)
        printf("R%d: %04x, A%d: %04x, C%i: %04x\n", i, registers[REG][i].word, i, registers[REG][i + 8].word, i,
               registers[1][i].word);
}

void printmemory(void) {
    //Print memory of choice
    printf("Enter lower and upper bound of memory to print out: (Rounded to #10)\n");
    int lower, upper;
    scanf("%x %x", &lower, &upper);
    //Print memory of choice
    for (; lower < upper; lower += 0x10) {
        printf("%04x |", lower);
        for (int i = 0; i < 0x10; i++) {
            printf(" %02x", memory.byte[lower + i]);
        }
        //Make some space
        printf("   ");
        //Printf ASCII characters
        for (int i = 0; i < 0x10; i++) {
            if(memory.byte[lower + i] >='A' && memory.byte[lower + i] <='z') {
                printf(" %c", memory.byte[lower + i]);
            }
            else{
                printf(" .");
            }
        }
        printf("\n");
    }
}