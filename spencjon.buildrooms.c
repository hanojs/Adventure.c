/*************************************
*Spencjon.BuildRooms.C
*Builds 7 Room Files to a Spencjon.Rooms.<PID> directory
*The files are created and named as their Room name is assigned
*One of the files will be a START_ROOM and one a END_ROOM
*   the others will be MID_ROOMS
*Each Room will have 3-6 connections to other rooms
*   and all the room connections will be two-ways
*************************************/

/************************************
* ROOM NAME: <room name>
* CONNECTION 1: <room name>
* …
* ROOM TYPE: <room type>
************************************/
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>


void createRooms(char rooms[][], char names[][], char directoryName[]){
    int rnd, i;
    FILE *fs;
    int closed[10] = {-1} ;
    for(i = 0; i < 7; i++){
        while(true){
            rnd = rand()%10;
            if(closed[rnd] == -1){ //the name hasn't been chosen yet
                strncpy(rooms[i], names[rnd], 8); //Copy the name to the rooms 
                closed[rnd] = 0;
                fs = fopen( ("directoryName\\%s",rooms[i]), "w+");
                fprintf(fs, "ROOM NAME: %s\n", rooms[i]); //write the file name 
                fclose(fs);
                break;
            }
        }
    }
    return;
}



}

int main(){
    int pid = getpid();
    int i = 0; 
    FILE *fs;
    char rooms[7][9]; //The chosen room names
    char directoryName[NAME_MAX+1]; //the final dirName
    const char names[10][9] = {   
                        "Lounge", 
                        "Library", 
                        "Dungeon", 
                        "Bed", 
                        "Cave", 
                        "PolisBox", 
                        "HotTub", 
                        "Cannery", 
                        "Stable", 
                        "Box"
                        };

    srand((unsigned int)time(NULL)); //seed random

    snprintf(directoryName, NAME_MAX + 1, "spencjon.rooms.%d", pid); //Create the dirctory name with the process id
    
    
    mkdir(directoryName, 0755);
    
    createRooms(rooms, names, directoryName);
    
    connectRooms(rooms, directoryName);
    
    



    return 0;
}