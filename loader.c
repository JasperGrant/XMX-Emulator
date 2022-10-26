/*
loader.c
File to define loading and fetching functions of the XMX emulator
Date modified: 2022-06-06
Author: Jasper Grant B00829263
*/
#include "main.h"

void load(char *filename){
//Declare file and storage variables
    //Input file is opened here
    FILE * fin = fopen(filename, "r");
    if(!fin){
        printf("File not found\n");
        return;
    }
    //Declare string which contains entire S-record
    char SRecord[MAXLEN];
    //Declare two characters for S and 0,1 or 9
    char SCheck, Type;
    //Declare integers for count address, running sum to be compared with the chksum, and high and low address bytes.
    int count, address, chksum, ah, al;

    //For line in file:
    while(fscanf(fin, "%s", SRecord) != EOF) {
        //Zero checksum
        chksum = 0;
        //Specify line scanned
        //printf("S-Record Analyzed: %s\n", SRecord);
        //Use sscanf to put values into code
        sscanf(SRecord, "%c%c%2x%2x%2x%s", &SCheck, &Type, &count, &ah, &al, SRecord);
        int data[count - ADDRESS_CORRECTION]; //ADDRESS_CORRECTION is taken away from count because address has already been taken off of string.
        //Fix address
        address = ah << 8 | al;

        //Bad Length Case
        if(HEX_TO_TWO_CHARS*(count - ADDRESS_CORRECTION) != strlen(SRecord)){
            printf("Bad Count\n");
            continue;
        }

        //Get data
        int tempaddress = address;
        for(int i = 0; i < count - ADDRESS_CORRECTION; i++){ //ADDRESS_CORRECTION is taken away from count because address has already been taken off of string.
            sscanf(SRecord, "%2x%s", &data[i], SRecord);
            //Account for the fact that the checksum should not be included in the sum or count as a potential storage location
            if(i < count - ADDRESS_CORRECTION - CHKSUM_CORRECTION) {
                //store instruction in memory
                //If S-record is type 1
                if (Type == '1') {
                    //store instruction in memory
                    memory.byte[tempaddress] = data[i];
                }
                chksum+=data[i];
                tempaddress++;
            }
        }

        //Add address and count to chksum
        chksum+= (ah + al + count);

        //mask upper bytes of chksum
        chksum = chksum & 0xFF;

        //XOR sum to make it the same as the checksum stored in the last element of data
        chksum = chksum ^ 0xFF;

        //Bad chksum case
        if(chksum != data[count - ADDRESS_CORRECTION -1 ]){
            printf("Bad Chksum\n");
            continue;
        }

        //Check name
        if((SCheck == 'S') && (Type == '0')){
            //S0
            //Display the name of the source module
            printf("Source Module Name: ");
            for(int i = 0; i < count - ADDRESS_CORRECTION - CHKSUM_CORRECTION; i++){
                printf("%c", data[i]);
            }
            printf("\n");
        }
        else if((SCheck == 'S') &&(Type == '1'));
            //Nothing specific has to be done here!
        else if((SCheck == 'S') &&(Type == '9')){
            //S9
            //Assign program counter
            PC = address;
            //Display the starting address
            //printf("Starting Address: %x\n", address);

        }
        else{
            //Bad type case
            printf("Bad Type\n");
            continue;
        }
        //Closing messages
        //printf("All good!\n");

    }
}

void fetchinst(void){
    //Increment clock cycles
    cycles++;
    if(PC%2!=0){
        printf("Invalid address Fault Detected\n");
        FLT_code = INVALID_ADDR_FLT;
        return;
    }
    //Set mar to program counter
    mar = PC;
    //Use bus to get data at that location
    bus(read, word);
    //Put mbr received from bus into inst
    inst = mbr;
    //Increment counter by 2
    PC+=2;
}