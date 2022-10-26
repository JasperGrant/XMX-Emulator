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

//Provide access to FLT_code
unsigned int FLT_code;

int main(int argc, char *argv[]) {

    //clear registers and set constants
    init_registers();
    //Init cycles as 0
    cycles = 0;
    //Init PC as 0
    PC = 0;
    //Init PSW.FLT
    PSW.FLT = 0;
    //Make initial code run at highest priority, this can be brought down by user if interrupts are required
    PSW.Current_Priority = 7;
    //Init initial FLT_code
    FLT_code = 0;
    //Init CEX state as off
    CEXstruct.state = OFF;
    //Declare file name if not given input file
    char filename[MAXLEN];
    //Detect input file, if not read from preset file input
    if (argc > 1) {
        //Load all input files
        for(int i = 1; i < argc; i++){
            load(argv[i]);
        }
    } else {
        //load("DevEx.xme");
        printf("\nNo input file detected, please load one before executing!\n\n");
    }
    //Set breakpointaddress and input
    //Outside of range of instruction space
    unsigned int breakpointaddress = 0xffff + 1;
    int input = 0;
    //Print welcome messages
    printf("Welcome to XMX Emulator\nWritten by Jasper Grant\n");
    //Main while loop
    while(1){
        //Print menu
        printf("PC: %x, Clock cycles passed: %d, Current Priority: %d\n", PC, cycles, PSW.Current_Priority);
        printf("Breakpoint address: %4x\n",breakpointaddress);
        printf("Menu:\n");
        printf("1. Load file by name\n");
        printf("2. Set a breakpoint\n");
        printf("3. Set a custom PC\n");
        printf("4. Display registers\n");
        printf("5. Display memory\n");
        printf("6. Display PSW\n");
        printf("7. Execute program\n");
        printf("8. Exit\n");
        //Ask for input
        scanf("%d", &input);
        //Switch cases based on input (Not real switch just if statements)
        if(input == 1) {
            //Load file by name
            printf("Enter filename (xme executable): \n");
            scanf("%s", filename);
            load(filename);
        }
        else if(input == 2) {
            //Set one of three kinds of breakpoints
            printf("Enter address to stop at: \n");
            scanf("%4x", &breakpointaddress);
        }
        else if(input == 3){
            //Set custom PC
            printf("Enter custom PC: \n");
            scanf("%4x", &PC);
        }
        else if(input == 4) {
            //Display registers
            printregisters();
        }
        else if(input == 5) {
            //Display memory
            printmemory();
        }
        else if(input == 6) {
            //Display PSW
            printPSW();
        }
        else if(input == 7) {
            //Main Execution
            //No control C found
            controlcfound = FALSE;
            //Bind siginthandler to SIGINT
            signal(SIGINT, (_crt_signal_t) siginthandler);
            //While no breakpoint or control c is found
            while (PC != breakpointaddress && controlcfound == FALSE){
                //Condition to see if PC needs to return from an exception
                if(PC == 0xffff){
                    //Reenter normal execution
                    return_from_exception();
                    //Start loop from beginning to ensure no missed breakpoints
                    continue;
                }
                //If Conditional execution is not a thing
                if (CEXstruct.state == OFF) {
                    //Fetch if not fault
                    if(!FLT_code) fetchinst();
                    //Decode if not fault
                    //Execute if not fault
                    if(!FLT_code) executeinst(decodeinst());
                    //Check faults
                    check_faults();
                    //Check device statuses
                    poll();

                }
                    //Conditional true case
                else if (CEXstruct.state == TRUE) {
                    //Loop while decrementing Tcounter
                    for (CEXstruct.Tcounter; CEXstruct.Tcounter > 0 && CEXstruct.state != OFF && controlcfound == FALSE; CEXstruct.Tcounter--) {
                        //Fetch if not fault
                        if(!FLT_code) fetchinst();
                        //Decode if not fault
                        //Execute if not fault
                        if(!FLT_code) executeinst(decodeinst());
                        //Check faults
                        check_faults();
                        //Check device statuses
                        poll();

                    }
                    //Loop while decrementing Fcounter
                    for (CEXstruct.Fcounter; CEXstruct.Fcounter > 0 && CEXstruct.state != OFF && controlcfound == FALSE; CEXstruct.Fcounter--) {
                        //Fetch if not fault
                        if(!FLT_code) fetchinst();
                        //Check faults
                        check_faults();
                        //Check device statuses
                        poll();
                    }
                    //Finish conditional
                    CEXstruct.state = OFF;
                }
                    //Conditional false case
                else if (CEXstruct.state == FALSE) {
                    //Loop while decrementing Fcounter
                    for (CEXstruct.Tcounter; CEXstruct.Tcounter > 0 && CEXstruct.state != OFF &&controlcfound == FALSE; CEXstruct.Tcounter--) {
                        //Fetch if not fault
                        if(!FLT_code) fetchinst();
                        //Check faults
                        check_faults();
                        //Check device statuses
                        poll();
                    }
                    //Loop while decrementing Tcounter
                    for (CEXstruct.Fcounter; CEXstruct.Fcounter > 0 && CEXstruct.state != OFF && controlcfound == FALSE; CEXstruct.Fcounter--) {
                        //Fetch if not fault
                        if(!FLT_code) fetchinst();
                        //Decode if not fault
                        //Execute if not fault
                        if(!FLT_code) executeinst(decodeinst());
                        //Check faults
                        check_faults();
                        //Check device statuses
                        poll();
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
        else if(input == 8) {
            //Exit program
            exit(1);
        }
        else {
            //Default case
            printf("Invalid selection\n");
        }
    }
}
