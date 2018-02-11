/*************************************
*Spencjon.BuildRooms.C
*Builds 7 Room Files to a Spencjon.Rooms.<PID> directory
*The files are created and named as their Room name is assigned
*One of the files will be a START_ROOM and one a END_ROOM
*   the others will be MID_ROOMS
*Each Room will have 3-6 connections to other rooms
*   and all the room connections will be two-ways
*************************************/

//Debug statements https://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing


#include <stdio.h> // file stuffs
#include <sys/types.h> // pid
#include <unistd.h> // pid
#include <stdlib.h> //rand
#include <time.h> //time for srand
#include <limits.h> // NAME_MAX for directory max name size
#include <string.h> //memset

//BR_ to differentiate my consts from others
#ifndef BR_VALUES
#define BR_NUM_ROOMS 7 //number of rooms in the game
#define BR_NUM_NAMES 10 //number of names to choose from
#define BR_MIN_CONN 3 //minimum number of connections per room
#define BR_MAX_CONN 6 //maximum number of connections per room
#define BR_MAX_NAME_SIZE 8 //Max size of the names of each room
#define BR_DIR_MAX 250 //Normally name max is 255, so this is just limiting it for our case
#endif


struct roomConnect{
  int con[BR_MAX_CONN]; //This con array that store what rooms it is connected to
  int numCon; //Number of connections the room has. It is updated in connectRooms()
};


/************************************
* ROOM NAME: <room name>
* CONNECTION 1: <room name>
* …
* ROOM TYPE: <room type>
************************************/
void createRooms(char *rooms[BR_MAX_NAME_SIZE + 1], char *names[BR_MAX_NAME_SIZE + 1], char directoryName[BR_DIR_MAX +1]){
    int rnd, i;
    size_t destination_size;
    FILE *fs;
    srand((unsigned int)time(NULL)); //seed random
    int closed[BR_NUM_NAMES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; // all the rooms we've already tried
    for(i = 0; i < BR_NUM_ROOMS; i++){ //Do this for as many rooms as we want created
        while(1){ //until we find a suitable room name...
            rnd = rand()%BR_NUM_NAMES;
            if(closed[rnd] == -1){ //if the name hasn't been chosen yet
                printf("Create Room %i %i\n", i, rnd);
                destination_size = sizeof(rooms[i]);
                printf("Roms[i] Size: %i   Names[rnd] %s Size: %i \n    Max Name Size: %i \n", destination_size, names[rnd], sizeof(names[rnd]), BR_MAX_NAME_SIZE);
                snprintf(rooms[i], BR_MAX_NAME_SIZE, "%s", names[rnd]); //Copy the name to the rooms //Create own copy function for 8 words long real quick
                closed[rnd] = 0; //Set the closed flag so we know we have used it
                printf("Create File %i %i\n", i, rnd);
                fs = fopen( ("%s\\%s",directoryName, rooms[i]), "w+");
                fprintf(fs, "ROOM NAME: %s\n", rooms[i]); //write the file name
                fclose(fs);
                break;
            }
        }
    }
    return;
}

int graphFull(struct roomConnect roomCon[BR_NUM_ROOMS]){
  int i;
  for(i = 0; i < BR_NUM_ROOMS; i++)
    if((roomCon[i]).numCon < BR_MIN_CONN)
      return 0;
  return 1;
}

int connectionAlreadyExists(struct roomConnect roomA, int roomB){
  int i;
  for(i = 0; i < BR_MAX_CONN; i++)
    if(roomA.con[i] == roomB)
        return 1;
  return 0;
}

void connectRooms(struct roomConnect roomCon[BR_NUM_ROOMS], int roomA, int roomB){
  int i;

  for(i = 0; i < BR_MAX_CONN; i++)
    if(roomCon[roomA].con[i] == -1){
      roomCon[roomA].con[i] = roomB;
      roomCon[roomA].numCon++;
      break;
    }

  for(i = 0; i < BR_MAX_CONN; i++)
    if(roomCon[roomB].con[i] == -1){
      roomCon[roomB].con[i] = roomA;
      roomCon[roomB].numCon++;
      break;
    }

  return;
}

void addAConnecetion( struct roomConnect roomCon[BR_NUM_ROOMS]){
  int i, rndRoomA, rndRoomB;

  while(1){
    rndRoomA = rand()%BR_NUM_ROOMS;
    if(roomCon[rndRoomA].numCon < BR_MAX_CONN)
      break;
  }

  while(1){
    rndRoomB = rand()%BR_NUM_ROOMS;
    if(roomCon[rndRoomB].numCon < BR_MAX_CONN)
      if(rndRoomA != rndRoomB)
        if(!connectionAlreadyExists(roomCon[rndRoomA], rndRoomB))
          break;
  }

  connectRooms(roomCon, rndRoomA, rndRoomB);

  return;
}

/************************************
* ROOM NAME: <room name>
* CONNECTION 1: <room name>
* …
* ROOM TYPE: <room type>
************************************/
//Room name has already been written to the rooms
void writeRooms(struct roomConnect roomCon[BR_NUM_ROOMS], char *rooms[BR_MAX_NAME_SIZE + 1], char directoryName[BR_DIR_MAX + 1]){
  int i, j;
  FILE *fs;
  for(i = 0; i < BR_NUM_ROOMS; i++){ //Go through each room
    fs = fopen( ("%s\\%s",directoryName, rooms[i]), "w+");

    for(j = 0; j < roomCon[i].numCon; j++) //for every connection...
      fprintf(fs, "Connection %i: %s\n", j, rooms[roomCon[i].con[j]]); //write the connection name

    //After the connections...
    if(i == 0)
      fprintf(fs, "ROOM TYPE: START_ROOM"); //write the room type for the first
      else if(i == (BR_NUM_ROOMS - 1))
        fprintf(fs, "ROOM TYPE: END_ROOM"); //write the room type for the last
        else
          fprintf(fs, "ROOM TYPE: MID_ROOM"); //write the room type for the middle rooms

    fclose(fs); //close it and go to the next room
  }
  return;
}


int main(){
    int pid = getpid();
    int i = 0;
    FILE *fs;

    char *rooms[BR_NUM_ROOMS]; //The chosen room names
    char directoryName[(BR_DIR_MAX+1)]; //the final dirName

    struct roomConnect roomCon[BR_NUM_ROOMS];
    for(i = 0; i < BR_NUM_ROOMS; i++){ //Set all the connections to -1 for
      memset(roomCon[i].con, -1, sizeof(int) * BR_MAX_CONN);
      roomCon[i].numCon = 0;
    }

    const char *names[BR_NUM_NAMES] = {"Lounge", "Library", "Dungeon", "Bed", "Cave", "Box", "HotTub", "Cannery", "Stable", "Webs"};

    snprintf(directoryName, BR_DIR_MAX + 1, "spencjon.rooms.%d", pid); //Create the dirctory name with the process id

    mkdir(directoryName, 0755);

    createRooms((char **)rooms, (char **)names, directoryName);

    while(!graphFull(roomCon)){
      addAConnecetion(roomCon);
    }

    writeRooms(roomCon, (char **)rooms, directoryName);

    return 0;
}
