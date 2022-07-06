/*
main.c
File to act as main execution for XMX emulator
Date modified: 2022-06-06
Author: Jasper Grant B00829263
*/

#include "main.h"

//Provides access to number of clock cycles
unsigned long cycles = 0;

//Volatile integer to detect ctrl-c calls
volatile int controlcfound;

//Provides access to CEXstruct
struct conditionalexecution CEXstruct;

int main(int argc, char *argv[]) {

    //clear registers and set constants
    init_registers();
    //Init cycles as 0
    cycles = 0;
    //Init PC as 0
    PC = 0;
    //Init CEX state as off
    CEXstruct.state = OFF;
    //Declare file name if not given input file
    char * filename;
    //Detect input file, if not read from preset file input
    if (argc != 1) {
        load(argv[1]);
    } else {
        load("input");
        //printf("No file name detected. Enter file name: ");
        //scanf("%s", filename);
        //load(filename);
    }
    //Set breakpointaddress and input
    unsigned short breakpointaddress = 0xffff;
    int input = 0;
    //Print welcome messages
    printf("Welcome to XMX Emulatator\nWritten by Jasper Grant\n");
    //Main while loop
    while(1){
        //Print menu
        printf("PC: %x, Clock cycles passed: %d\n", PC, cycles);
        printf("Breakpoint address: %4x\n",breakpointaddress);
        printf("Menu:\n");
        printf("1. Set a breakpoint\n");
        printf("2. Set a custom PC\n");
        printf("3. Display registers\n");
        printf("4. Display memory\n");
        printf("5. Display PSW\n");
        printf("6. Execute program\n");
        printf("7. Exit\n");
        //Ask for input
        scanf("%d", &input);
        //Switch cases based on input (Not real switch just if statements)
        if(input == 1) {
            //Set one of three kinds of breakpoints
            printf("Enter address to stop at: \n");
            scanf("%4x", &breakpointaddress);
        }
        else if(input == 2){
            //Set custom PC
            printf("Enter custom PC: \n");
            scanf("%4x", &PC);
        }
        else if(input == 3) {
            //Display registers
            printregisters();
        }
        else if(input == 4) {
            //Display memory
            printmemory();
        }
        else if(input == 5) {
            //Display PSW
            printPSW();
        }
        else if(input == 6) {
            //Main Execution
            //No control C found
            controlcfound = FALSE;
            //Bind siginthandler to SIGINT
            signal(SIGINT, (_crt_signal_t) siginthandler);
            //While no breakpoint or control c is found
            while (PC != breakpointaddress && controlcfound == FALSE){
                //If Conditional execution is not a thing
                if (CEXstruct.state == OFF) {
                    //Fetch
                    fetchinst();
                    //Decode
                    //Execute
                    executeinst(decodeinst());

                }
                    //Conditional true case
                else if (CEXstruct.state == TRUE) {
                    //Loop while decrementing Tcounter
                    for (CEXstruct.Tcounter; CEXstruct.Tcounter > 0 && CEXstruct.state != OFF && controlcfound == FALSE; CEXstruct.Tcounter--) {
                        //Fetch
                        fetchinst();
                        //Decode
                        //Execute
                        executeinst(decodeinst());

                    }
                    //Loop while decrementing Fcounter
                    for (CEXstruct.Fcounter; CEXstruct.Fcounter > 0 && CEXstruct.state != OFF && controlcfound == FALSE; CEXstruct.Fcounter--) {
                        //Fetch
                        fetchinst();

                    }
                    //Finish conditional
                    CEXstruct.state = OFF;
                }
                    //Conditional false case
                else if (CEXstruct.state == FALSE) {
                    //Loop while decrementing Fcounter
                    for (CEXstruct.Tcounter; CEXstruct.Tcounter > 0 && CEXstruct.state != OFF &&controlcfound == FALSE; CEXstruct.Tcounter--) {
                        //Fetch
                        fetchinst();
                    }
                    //Loop while decrementing Tcounter
                    for (CEXstruct.Fcounter; CEXstruct.Fcounter > 0 && CEXstruct.state != OFF && controlcfound == FALSE; CEXstruct.Fcounter--) {
                        //Fetch
                        fetchinst();
                        //Decode
                        //Execute
                        executeinst(decodeinst());
                    }
                    //Finish conditional
                    CEXstruct.state = OFF;
                }
                //Print registers and memory for debug purposes
                //printregisters();
                //printPSW();
                //printmemory();
            }
            printf("Program ended\n");
        }
        else if(input == 7) {
            //Exit program
            exit(1);
        }
        else {
            //Default case
            printf("Invalid selection\n");
        }
    }
}
