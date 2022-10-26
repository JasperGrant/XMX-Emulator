/*
executor.c
File to define execution functions of the XMX emulator
Date modified: 2022-07-11
Author: Jasper Grant B00829263
*/

#include <conio.h>
#include "main.h"

//Variables available over the entire file used to tell whether the DRA and SRA bits are set in an opcode
unsigned short DRAbit, SRAbit;

unsigned short temp_Previous_Priority;

//Provides access to PSW bits
psw PSW;

//Provides access to CEXstruct
extern struct conditionalexecution CEXstruct;

void executeinst(unsigned int decode) {
    //Increment clock cycles
    cycles++;
    //Set DRA and SRA bits to be 0 by default
    DRAbit = 0;
    SRAbit = 0;
    //Use enum given by decode to decide which instruction should be executed
    switch (decode) {
        case BL:
            executeBL();
            break;
        case BR:
            executeBR();
            break;
        case CEX:
            executeCEX();
            break;
        case SWPB:
            executeSWPB();
            break;
        case SKT:
            executeSXT();
            break;
        case SETPRI:
            //printf("Instruction not emulated\n");
            executeSETPRI();
            break;
        case SVC:
            //printf("Instruction not emulated\n");
            executeSVC();
            break;
        case SETCC:
            //printf("Instruction not emulated\n");
            executeSETCC();
            break;
        case CLRCC:
            //printf("Instruction not emulated\n");
            executeCLRCC();
            break;
        case SRA:
            executeSRA();
            break;
        case RRC:
            executeRRC();
            break;
        case ADD:
            executeADDSUB(ADDITION, NOCARRY);
            break;
        case ADDC:
            executeADDSUB(ADDITION, PSW.C);
            break;
        case SUB:
            executeADDSUB(SUBTRACTION, ALWAYSCARRY);
            break;
        case SUBC:
            executeADDSUB(SUBTRACTION, PSW.C);
            break;
        case CMP:
            executeADDSUB(SUBTRACTION, ALWAYSCARRY);
            break;
        case XOR:
            executeXOR();
            break;
        case AND:
            executeAND();
            break;
        case OR:
            executeOR();
            break;
        case BIT:
            executeBIT();
            break;
        case BIS:
            executeBIvalue(1);
            break;
        case BIC:
            executeBIvalue(0);
            break;
        case MOV:
            executeMOV();
            break;
        case SWAP:
            executeSWAP();
            break;
        case LD:
            executeLD();
            break;
        case ST:
            executeST();
            break;
        case MOVL:
            executeMOVL();
            break;
        case MOVLZ:
            executeMOVLZ();
            break;
        case MOVLS:
            executeMOVLS();
            break;
        case MOVH:
            executeMOVH();
            break;
        case LDR:
            executeLDR();
            break;
        case STR:
            executeSTR();
            break;
            //Assignment 2
        case AADD:
            //Set SRA and DRA bits
            SRAbit = BIT(9);
            DRAbit = BIT(8);
            executeADDSUB(ADDITION, NOCARRY);
            break;
        case ASUB:
            //Set SRA and DRA bits
            SRAbit = BIT(9);
            DRAbit = BIT(8);
            executeADDSUB(SUBTRACTION, ALWAYSCARRY);
            break;
        case ACMP:
            //Set SRA and DRA bits
            SRAbit = BIT(9);
            DRAbit = BIT(8);
            executeADDSUB(SUBTRACTION, ALWAYSCARRY);
            break;
            //Execute nothing if invalid instruction passed through decode
        default:
            return;
    }

}

void executeBL(void) {
    //Assign current program counter to link register
    LR = PC;
    //Set PC to effective address
    PC += (sign_ext(BLOFFSET, 12) << 1);
    //Return CEX state to off
    CEXstruct.state = OFF;
}

void executeBR(void) {
    //Set PC to effective address
    PC += (sign_ext(BROFFSET, 9) << 1);
    //Return CEX state to False
    CEXstruct.state = OFF;
}


void executeADDSUB(unsigned short subtraction, unsigned short carry) {
    //Declare variables to enter into update_psw
    union borw src;
    union borw dst;
    union borw res;
    //Give src and dst their values
    src.word = (subtraction) ? ~registers[RC][SC].word : registers[RC][SC].word;
    dst.word = registers[REG][DEST].word;
    //If byte
    if (WB) {
        //Add bytes in registers
        res.byte[0] = src.byte[0] + dst.byte[0] + carry;
        //Update PSW
        update_psw(src.byte[0], dst.byte[0], res.byte[0], WB);
        //If not CMP or ACMP
        if ((BIT3(8) == 0b101 && BIT3(13) == 0b001) || (BIT3(10) == 0b110 && BIT3(13) == 0b011)) {
            return;
        } else {
            //Update destination
            registers[REG][DEST].byte[0] = res.byte[0];
        }

    }
        //If word
    else {
        //Add words in registers
        res.word = src.word + dst.word + carry;
        //Update PSW
        update_psw(src.word, dst.word, res.word, WB);
        //If not CMP or ACMP
        if ((BIT3(8) == 0b101 && BIT3(13) == 0b001) || (BIT3(10) == 0b110 && BIT3(13) == 0b011)) {
            return;
        } else {
            //Update destination
            registers[REG][DEST].word = res.word;
        }
    }

}

void executeSWPB(void) {
    //Temp register to allow switching
    unsigned char tempreg;
    //Assign tempreg to value of LSByte
    tempreg = registers[REG][DEST].byte[0];
    //Assign LSByte to value of MSByte
    registers[REG][DEST].byte[0] = registers[REG][DEST].byte[1];
    //Assign MSByte to value of tempreg (original value of LSByte)
    registers[REG][DEST].byte[1] = tempreg;
}

void executeXOR(void) {
    //If byte
    if (WB) {
        //XOR bytes in registers
        registers[REG][DEST].byte[0] = registers[RC][SC].byte[0] ^ registers[REG][DEST].byte[0];
        update_logic_psw(registers[REG][DEST].byte[0], byte);
    }
        //If word
    else {
        //XOR words in registers
        registers[REG][DEST].word = registers[RC][SC].word ^ registers[REG][DEST].word;
        update_logic_psw(registers[REG][DEST].word, word);
    }
}

void executeAND(void) {
    //If byte
    if (WB) {
        //AND bytes in registers
        registers[REG][DEST].byte[0] = registers[RC][SC].byte[0] & registers[REG][DEST].byte[0];
        update_logic_psw(registers[REG][DEST].byte[0], byte);
    }
        //If word
    else {
        //AND words in registers
        registers[REG][DEST].word = registers[RC][SC].word & registers[REG][DEST].word;
        update_logic_psw(registers[REG][DEST].word, word);
    }
}

void executeOR(void) {
    //If byte
    if (WB) {
        //OR bytes in registers
        registers[REG][DEST].byte[0] = registers[RC][SC].byte[0] | registers[REG][DEST].byte[0];
        update_logic_psw(registers[REG][DEST].byte[0], byte);
    }
        //If word
    else {
        //OR words in registers
        registers[REG][DEST].word = registers[RC][SC].word | registers[REG][DEST].word;
        update_logic_psw(registers[REG][DEST].word, word);
    }
}

void executeMOVL(void) {
    //Decide DRA bit
    DRAbit = BIT(11);
    //Assign BBBBBBBB to LSByte
    registers[REG][DEST].byte[0] = BYTE;
}

void executeMOVLZ(void) {
    //Decide DRA bit
    DRAbit = BIT(11);
    //Assign BBBBBBBB to LSByte
    registers[REG][DEST].byte[0] = BYTE;
    //Clear MSByte
    registers[REG][DEST].byte[1] = 0;
}

void executeMOVLS(void) {
    //Decide DRA bit
    DRAbit = BIT(11);
    //Assign BBBBBBBB to LSByte
    registers[REG][DEST].byte[0] = BYTE;
    //Set MSByte
    registers[REG][DEST].byte[1] = 0xFF;
}

void executeMOVH(void) {
    //Decide DRA bit
    DRAbit = BIT(11);
    //Assign BBBBBBBB to MSByte
    registers[REG][DEST].byte[1] = BYTE;
}

void executeMOV(void) {
    //Decide DRA and SRA bits
    DRAbit = BIT(7);
    SRAbit = BIT(8);
    //If byte
    if (WB) {
        //Move source byte to destination
        registers[REG][DEST].byte[0] = registers[REG][SC].byte[0];
    }
        //If word
    else {
        //Move source word to destination
        registers[REG][DEST].word = registers[REG][SC].word;
    }

}

void executeSWAP(void) {
    //Decide DRA and SRA bits
    DRAbit = BIT(7);
    SRAbit = BIT(8);
    //Temp register to allow switching
    unsigned char tempreg;
    //Assign tempreg to value of destination
    tempreg = registers[REG][DEST].word;
    //Assign destination to value of source
    registers[REG][DEST].word = registers[REG][SC].word;
    //Assign source to value of tempreg (original value of destination)
    registers[REG][SC].word = tempreg;
}

void executeCEX(void) {
    //Initialize state as false
    CEXstruct.state = FALSE;
    //If condition is met state is changed to true
    switch (CONDITION) {
        case 0://0b0000 EQ
            CEXstruct.state = (PSW.Z == 1);
            break;
        case 1://0b0001 NE
            CEXstruct.state = (PSW.Z == 0);
            break;
        case 2://0b0010 CS / HS
            CEXstruct.state = (PSW.C == 1);
            break;
        case 3://0b0011 CC / LO
            CEXstruct.state = (PSW.C == 0);
            break;
        case 4://0b0100 MI
            CEXstruct.state = (PSW.N == 1);
            break;
        case 5://0b0101 PL
            CEXstruct.state = (PSW.N == 0);
            break;
        case 6://0b0110 VS
            CEXstruct.state = (PSW.V == 1);
            break;
        case 7://0b0111 VC
            CEXstruct.state = (PSW.V == 0);
            break;
        case 8://0b1000 HI
            CEXstruct.state = ((PSW.C == 1 && PSW.Z == 0));
            break;
        case 9://0b1001 LS
            CEXstruct.state = ((PSW.C == 0 && PSW.Z == 1));
            break;
        case 10://0b1010 GE
            CEXstruct.state = (PSW.C == PSW.V);
            break;
        case 11://0b1011 LT
            CEXstruct.state = (PSW.C != PSW.V);
            break;
        case 12://0b1100 GT
            CEXstruct.state = ((PSW.Z == 0) && (PSW.N == PSW.V));
            break;
        case 13://0b1101 LE
            CEXstruct.state = ((PSW.Z == 1) && (PSW.N != PSW.V));
            break;
        case 14://0b1110 TR
            CEXstruct.state = 1;
            break;
        case 15://0b1111 FL
            CEXstruct.state = 0;
            break;
    }
    //Update false and true counts for main program
    CEXstruct.Fcounter = IFFALSE;
    CEXstruct.Tcounter = IFTRUE;


}

void executeSXT(void) {
    //Extend sign in MSBit of LSByte
    registers[REG][DEST].word = sign_ext(registers[REG][DEST].word, 7);
}

void executeLDR(void) {
    //Always use address register for source
    SRAbit = 1;
    //Read DRA bit
    DRAbit = BIT(12);

    //If byte
    if (WB) {
        //Create effective address
        mar = registers[REG][SC].word + (sign_ext(LDRSTROFFSET, 5));
        //Get data to load through bus
        bus(read, byte);
        //Load in destination register
        registers[REG][DEST].byte[0] = mbr;
    }
        //If word
    else {
        //Create effective address
        mar = registers[REG][SC].word + (sign_ext(LDRSTROFFSET, 5));
        //Get data to load through bus
        bus(read, word);
        //Load in destination register
        registers[REG][DEST].word = mbr;
    }
}


void executeSTR(void) {
    //Always use address register for destination
    DRAbit = 1;
    //Read SRA bit
    SRAbit = BIT(12);

    //If byte
    if (WB) {
        //Set data to store
        mbr = registers[REG][SC].byte[0];
        //Create effective address
        mar = registers[REG][DEST].word + (sign_ext(LDRSTROFFSET, 5));
        //Store data at effective address
        bus(write, byte);
    }
        //If word
    else {
        //Set data to store
        mbr = registers[REG][SC].word;
        //Create effective address
        mar = registers[REG][DEST].word + (sign_ext(LDRSTROFFSET, 5));
        //Store data at effective address
        bus(write, word);
    }
}

void executeBIT(void) {
    //If byte
    if (WB) {
        //Check bit in LSByte based on number given in source, send this response to PSW.Z
        PSW.Z = (0 == ((registers[REG][DEST].byte[0] >> registers[RC][SC].byte[0]) & 0x01));
    }
        //If word
    else {
        //Check bit in word based on number given in source, send this response to PSW.Z
        PSW.Z = (0 == ((registers[REG][DEST].word >> registers[RC][SC].byte[0]) & 0x01));
    }
}

void executeBIvalue(unsigned short value) {
    //If byte
    if (WB) {
        //Set or clear bit based on value of value
        registers[REG][DEST].byte[0] = value ? registers[REG][DEST].byte[0] | (1 << registers[RC][SC].byte[0]) :
                                       registers[REG][DEST].byte[0] & (~(1 << registers[RC][SC].byte[0]));
        update_logic_psw(registers[REG][DEST].byte[0], byte);
    }
        //If word
    else {
        //Set or clear bit based on value of value
        registers[REG][DEST].word = value ? registers[REG][DEST].word | (1 << registers[RC][SC].word) :
                                    registers[REG][DEST].word & (~(1 << registers[RC][SC].word));
        update_logic_psw(registers[REG][DEST].word, word);
    }
}

void executeSRA(void) {
    //If byte
    if (WB) {
        //Set carry bit to LSBit to be discarded
        PSW.C = registers[REG][DEST].byte[0] & 0x01;
        //Make register equal to destination value shifted right and preserved MSbit of the nonshifted data
        registers[REG][DEST].byte[0] = (registers[REG][DEST].byte[0] >> 1) | (registers[REG][DEST].byte[0] & 0x80);
    }
        //If word
    else {
        //Set carry bit to LSBit to be discarded
        PSW.C = registers[REG][DEST].word & 0x01;
        //Make register equal to destination value shifted right and preserved MSbit of the nonshifted data
        registers[REG][DEST].word = (registers[REG][DEST].word >> 1) | (registers[REG][DEST].word & 0x8000);
    }

}

void executeRRC(void) {
    //Store value of carry
    unsigned short C = PSW.C;
    //Store LSBit of register in carry
    PSW.C = (registers[REG][DEST].word & 0x01);
    //If byte
    if (WB) {
        //Shift one destination value one right with the previous carry taking over the MSBit
        registers[REG][DEST].byte[0] = (registers[REG][DEST].byte[0] >> 1) | (C << 7);
    }
        //If word
    else {
        //Shift one destination value one right with the previous carry taking over the MSBit
        registers[REG][DEST].word = (registers[REG][DEST].word >> 1) | (C << 15);
    }

}

void executeLD(void) {
    //Always use address register for source
    SRAbit = 1;
    //Read DRA bit
    DRAbit = BIT(9);
    //Direct Addressing case
    //If DI == 0 do nothing
    if (DI == 0);
        //If DI == 1
    else {
        //If PRP0 == 0
        if (PRPO == 0) {
            //If ID == 0
            if (ID == 0) {
                //Add 1 or 2 based on word or byte
                registers[REG][SC].word = registers[REG][SC].word + ((WB == 0) ? 2 : 1);
            }
                //If ID == 1
            else {
                //Subtract 1 or 2 based on word or byte
                registers[REG][SC].word = registers[REG][SC].word - ((WB == 0) ? 2 : 1);
            }
        }
    }
    //Regardless of what happened mar can be taken from address register
    mar = registers[REG][SC].word;
    if (WB) {
        //Load data at effective address
        bus(read, byte);
        //Load data into register
        registers[REG][DEST].byte[0] = mbr;
    }
        //If word
    else {
        //Load data at effective address
        bus(read, word);
        //Load data into register
        registers[REG][DEST].word = mbr;
    }
    //If DI == 1 and PRPO == 0
    if (DI == 1 && PRPO == 1) {
        //If ID == 0
        if (ID == 0) {
            //Add 1 or 2 based on word or byte
            registers[REG][SC].word = registers[REG][SC].word + ((WB == 0) ? 2 : 1);
        }
            //If ID == 1
        else {
            //Subtract 1 or 2 based on word or byte
            registers[REG][SC].word = registers[REG][SC].word - ((WB == 0) ? 2 : 1);
        }
    }

}


void executeST(void) {
    //Always use address register for destination
    DRAbit = 1;
    //Read SRA bit
    SRAbit = BIT(9);
    //Direct Addressing case
    //If DI == 0 do nothing
    if (DI == 0);
        //If DI == 1
    else {
        //If PRP0 == 0
        if (PRPO == 0) {
            //If ID == 0
            if (ID == 0) {
                //Add 1 or 2 based on word or byte
                registers[REG][DEST].word = registers[REG][DEST].word + ((WB == 0) ? 2 : 1);
            }
                //If ID == 1
            else {
                //Subtract 1 or 2 based on word or byte
                registers[REG][DEST].word = registers[REG][DEST].word - ((WB == 0) ? 2 : 1);
            }
        }
    }
    //Regardless of what happened mar can be taken from address register
    mar = registers[REG][DEST].word;
    if (WB) {
        //Set data to store
        mbr = registers[REG][SC].byte[0];
        //Store data at effective address
        bus(write, byte);
    }
        //If word
    else {
        //Set data to store
        mbr = registers[REG][SC].word;
        //Store data at effective address
        bus(write, word);
    }
    //If DI == 1 and PRPO == 0
    if (DI == 1 && PRPO == 1) {
        //If ID == 0
        if (ID == 0) {
            //Add 1 or 2 based on word or byte
            registers[REG][DEST].word = registers[REG][DEST].word + ((WB == 0) ? 2 : 1);
        }
            //If ID == 1
        else {
            //Subtract 1 or 2 based on word or byte
            registers[REG][DEST].word = registers[REG][DEST].word - ((WB == 0) ? 2 : 1);
        }
    }
}

void executeSETPRI(void) {
    //If encoded priority is less than current priority
    if (BIT3(0) < PSW.Current_Priority) {
        //Set current priority to encoded priority
        PSW.Current_Priority = BIT3(0);
    } else {
        //CPU Priority Fault occurs
        printf("Priority Fault Detected\n");
        FLT_code = PRIORITY_FLT;
    }

}

void executeSVC(void) {
    //Initialize short to temporarily hold PSW from vector
    //Relative address is used to determine vector
    //Start of interrupt vectors is added to encoded interrupt call
    unsigned short INT_VECT = memory.word[INT_VECT_START + BIT4(0)];
    //Read word containing interrupt PSW
    //If current priority is less then interrupt priority
    if (PSW.Current_Priority < INT_VECT_CURRENT_PRIORITY) {
        exception(BIT4(0));
    } else {
        //CPU Priority Fault occurs
        printf("Priority Fault Detected\n");
        FLT_code = PRIORITY_FLT;
    }
}

void executeSETCC(void) {
    //Set PSW bits if corresponding instruction bit is set
    //If bit is not set do nothing
    PSW.V = BIT(4) ? 1 : PSW.V;
    //If current priority is not 7
    if (PSW.Current_Priority != 7) PSW.SLP = BIT(3) ? 1 : PSW.SLP;
    PSW.N = BIT(2) ? 1 : PSW.N;
    PSW.Z = BIT(1) ? 1 : PSW.Z;
    PSW.C = BIT(0) ? 1 : PSW.C;
}

void executeCLRCC(void) {
    //Clear PSW bits if corresponding instruction bit is set
    //If bit is not set do nothing
    PSW.V = BIT(4) ? 0 : PSW.V;
    //If current priority is not 7
    if (PSW.Current_Priority != 7) PSW.SLP = BIT(3) ? 0 : PSW.SLP;
    PSW.N = BIT(2) ? 0 : PSW.N;
    PSW.Z = BIT(1) ? 0 : PSW.Z;
    PSW.C = BIT(0) ? 0 : PSW.C;
}

unsigned short sign_ext(unsigned short offset, unsigned short signbit) {
    //If offset has set sign bit
    if (offset & (1 << signbit)) {
        //Offset  is ORed with 0xffff shifted signbit bits
        offset |= NEGSIGN(signbit);
    }
    //Else return offset as is
    return offset;
}

void siginthandler() {
    //Changing this integer from 0 to 1 will halt execution
    controlcfound = TRUE;
    //Reset
    signal(SIGINT, (_crt_signal_t) siginthandler);
}

void exception(unsigned short address) {
    //Convert second fault to double fault
    if (PSW.FLT) {
        printf("Double Fault detected\n");
        address = DOUBLE_FLT;
    }
    //If fault was detected change PSW fault bit
    if (address == ILLEGAL_INST_FLT || address == INVALID_ADDR_FLT || address == PRIORITY_FLT) {
        PSW.FLT = 1;
    }

    //Temporary variable to act as storage for values pulled from memory
    unsigned int INT_VECT = memory.word[INT_VECT_START + address];
    //Push PC
    memory.word[SP >> 1] = PC;
    //Decrement stack pointer by word
    SP -= 2;
    //Push LR
    memory.word[SP >> 1] = LR;
    //Decrement stack pointer by word
    SP -= 2;
    //Push PSW
    memory.word[SP >> 1] = PSW_WORD;
    //Decrement stack pointer by word
    SP -= 2;
    //Push CEX state
    memory.word[SP >> 1] = CEX_WORD;
    //Previous priority = PSW.Current_Priority
    temp_Previous_Priority = PSW.Current_Priority;
    //PSW = PSW of handler
    PSW.V = INT_VECT_BIT(4);
    PSW.N = INT_VECT_BIT(2);
    PSW.C = INT_VECT_BIT(0);
    PSW.Z = INT_VECT_BIT(1);
    PSW.Current_Priority = INT_VECT_CURRENT_PRIORITY;

    //Clear sleep bit of PSW
    PSW.SLP = 0;
    //Make PSW previous priority equal to prior set Previous Priority
    PSW.Previous_Priority = temp_Previous_Priority;
    //PC = Address in handler
    PC = memory.word[INT_VECT_START + address + 1];
    //Make link register value to #FFFF
    LR = 0xffff;
    //Clear CEX state
    CEXstruct.state = OFF;
}

void return_from_exception(void) {
    //Clear PSW.FLT because exception has concluded
    PSW.FLT = 0;
    //If no pending exceptions or exceptions have lower priority then PSW.Previous
    //if(){
    //Variable to hold things pulled out of memory (Only named this way for ease of macro use)
    unsigned int INT_VECT = memory.word[SP >> 1];
    //Pull out specific bits of INT_VECT for CEXstruct
    CEXstruct.state = (INT_VECT >> 6) & 0x02;
    CEXstruct.Tcounter = (INT_VECT >> 3) & 0x07;
    CEXstruct.Fcounter = INT_VECT & 0x07;
    //Increment SP by word
    SP += 2;
    //Put PSW into INT_VECT
    INT_VECT = memory.word[SP >> 1];
    //Pull PSW
    PSW.V = INT_VECT_BIT(4);
    PSW.N = INT_VECT_BIT(2);
    PSW.C = INT_VECT_BIT(0);
    PSW.Z = INT_VECT_BIT(1);
    PSW.Current_Priority = INT_VECT_CURRENT_PRIORITY;
    //Increment SP by word
    SP += 2;
    //Pull LR
    LR = memory.word[SP >> 1];
    //Increment SP by word
    SP += 2;
    //Pull PC
    PC = memory.word[SP >> 1];


    //}
    //ELSE
    //Only inplement for device interrupts
}

void check_faults(void) {
    //If fault has been found
    if (FLT_code) {
        //Trigger fault
        exception(FLT_code);
        //Reset fault code
        FLT_code = 0;
    }
}

//Declare global timer variable
clock_t timer_end = 0;

void poll(void) {

    //Poll clock
    //If timer is enabled
    if (ENA(TIMER_CSR)) {
        //If time is up
        if (clock() > timer_end) {
            //If DBA is already set, set overflow
            if (DBA(TIMER_CSR) == 1) TIMER_CSR |= SET_OF;
            //Set DBA to 1
            TIMER_CSR |= SET_DBA;
            //If timer data register is 0 make it 1
            if (TIMER_DR == 0) TIMER_DR = 1;
            //Move ahead timer end by time in data register
            timer_end = clock() + ((long) 1000) / ((long) TIMER_DR);
        }
    }

    //Poll keyboard
    //If keyboard was hit and keyboard is enabled
    if (kbhit() && ENA(KEYBOARD_CSR)) {
        if (DBA(KEYBOARD_CSR) == 1) {
            //Set overflow for keyboard
            KEYBOARD_CSR |= SET_OF;
        }
        //Set data or buffer available of keyboard (shows it is full)
        KEYBOARD_CSR |= SET_DBA;
        //Send character to keyboard
        KEYBOARD_DR = getch();
        //printf("Character printed from printf: %c\n", KEYBOARD_DR);
    }

    //Poll screen
    //If screen is enabled
    if (ENA(SCREEN_CSR) && !DBA(SCREEN_CSR)) {
        //Delay 50ms
        clock_t current_time = clock();
        while (clock() < current_time + DELAY);
        //Set DBA bit
        SCREEN_CSR |= SET_DBA;
        //Print character
        putchar(SCREEN_DR);
        fflush(stdout);
    }

}
