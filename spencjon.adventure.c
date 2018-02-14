/*************************************
*
*
*
*
************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h> //memset strcpy
#include <stdlib.h> //malloc

#ifndef AD_VALUES
  #define AD_NUM_ROOMS 7
  #define AD_MIN_CONN 3
  #define AD_MAX_CONN 6
  #define AD_NAME_INITIAL 9
#endif



struct room {
  char roomName[AD_NAME_INITIAL];
  char roomType[20];
  char connections[AD_MAX_CONN][AD_NAME_INITIAL];
  int numCon;
};
struct path {
  char **pathList;
  int pathLength;
  int pathSize;
};

void getNewestDirectory(char directoryName[250]){
    DIR *directoryPointer = opendir(".");
    struct stat dirStat;
    struct dirent *direntPointer;
    time_t latest = 0;
    direntPointer = readdir(directoryPointer);
    while ((direntPointer = readdir(directoryPointer)) != NULL) { //while there are more directories or files
        memset(&dirStat, 0, sizeof(dirStat)); //allocate the memorY

        if (!(stat(direntPointer->d_name, &dirStat) < 0))
            if (S_ISDIR(dirStat.st_mode)){
                if (dirStat.st_mtime > latest)
                { //if the directory is newer than the old one...
                    strcpy(directoryName, direntPointer->d_name);
                    latest = dirStat.st_mtime;
                    //printf("Directory name: %s \n", directoryName);
                }
            else
            {
              continue; //if it can't be gotten into..
            }
        }
        else
        {
          continue; //IF IT ISN'T A directory
        }
    }
    closedir(directoryPointer);
    //printf("Final Directory name: %s \n", directoryName);
}

int getFileLine(char *buff, int firstChar, FILE *fs){
  char tmp[240];
  int i;

  if(!fgets(tmp, sizeof(tmp), fs)){
    return 0;
  }

  if(tmp[5] == 'T'){
    firstChar = 11;
  }

  memcpy(buff, &tmp[firstChar], sizeof(tmp));

  i = 0;
  while(buff[i]){
    if(buff[i]=='\n')
      buff[i]='\0';
    i++;
  }

  if(tmp[5] == 'T'){
    return 0;
  }
  
  return 1;
}



void storeRoom(FILE *fs, struct room *rooms, int roomNum){
    int i = 0;
    char *buff = malloc(100 * sizeof(char));
    char *name = malloc(20 * sizeof(char));
    size_t nSize = sizeof(buff);
    //Read/store the room name

    getFileLine(buff, 11, fs);
    memcpy(rooms[roomNum].roomName, buff, sizeof(rooms[roomNum].roomName));
    //printf("ROOM NAME: %s\n", buff);

    
    //Read/Store all the connections. The last fget will read the room type
    while(getFileLine(buff, 14, fs)){
      memcpy(rooms[roomNum].connections[i], buff, sizeof(rooms[roomNum].connections[i]));
      i++; //increment the connection num
      //printf("CONNECTION: %s\n", buff);
    }
    
    //store the roomType
    memcpy(rooms[roomNum].roomType, buff, sizeof(rooms[roomNum].roomType));
    //printf("ROOM TYPE: %s\n", buff);
    

    //store the number of connections
    rooms[roomNum].numCon = i; //The while loop will iterate i to the number of connections

    return;
}

void readRooms(char directoryName[250], struct room *rooms){
    int i = 0;
    char buff[250]; //needed for fopen becuase you can't names with variables in fopen like in printf

    DIR *directoryPointer = opendir(directoryName);
    FILE *fs;
    struct dirent *direntPointer;
    //printf("Directory Name: %s\n", directoryName);
    while ((direntPointer = readdir(directoryPointer)) != NULL) { //while there are files
        if(direntPointer->d_name[0] == '.'){
          continue;
        }
        snprintf( buff, sizeof( buff ) - 1, "./%s/%s", directoryName, direntPointer->d_name);
        fs = fopen(buff, "r");
        storeRoom(fs, rooms, i);
        i++; 
        fflush(stdin); 
        if(fs == NULL){
          fclose(fs); //if the file poiinter isn't null, close it
        }
    }
    closedir(directoryPointer);
    return;
}

void displayCurrentLocation(struct room *rooms, int i){
  int j = 0;
  fflush(stdout);
  printf("CURRENT LOCATION: %s\n", rooms[i].roomName);

  //POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
  printf("POSSIBLE CONNECTIONS: %s", rooms[i].connections[j]);
  for(j = 1; j < rooms[i].numCon; j++)
    printf(", %s", rooms[i].connections[j]);
  printf(".\n");


  //WHERE TO? > (Then get the input)
  printf("WHERE TO? >");
}

int getRoomByType(struct room *rooms, char *type){
  int i;

  for(i = 0; i < AD_NUM_ROOMS; i++){
    if(strcmp(rooms[i].roomType, type)) continue;
    return i;
  }

  return 999;
}
void addToPath(struct path *playerPath, char *roomName){
  char **tmpPath;
  int i = 0;
  playerPath->pathLength++;

  //if the path is longer or equal to the path we've already allocated, double it and free the old one 
  if(playerPath->pathLength >= playerPath->pathSize){
    playerPath->pathSize = 2 * playerPath->pathSize;
    tmpPath = malloc(sizeof(char*) * playerPath->pathSize);
    for(i = 0; i < playerPath->pathSize; i++){
      tmpPath[i] = playerPath->pathList[i];
    }
    free(playerPath->pathList);
    playerPath->pathList = tmpPath;
  }

  playerPath->pathList[playerPath->pathLength - 1] = roomName;
}


void userChoice(int *currentRoom, struct room *rooms, struct path *playerPath, char *userIn){
  int i;
  char *buffer;
  size_t bufferSize = 32;

  if(!strcmp(userIn, "time")){
    //displayTime();
    printf("WHERE TO? >");
    buffer = malloc(bufferSize * sizeof(char));
    fflush(stdin);
    getline(&buffer, &bufferSize, stdin);
    printf("UserIn: ---%s---", buffer);
    userChoice(currentRoom, rooms, playerPath, buffer); //nested so that the vurrent locations doesn't play again.
    free(buffer);
    return;
  }

  for(i = 0; i < AD_NUM_ROOMS; i++){
    if(!strcmp(userIn, rooms[i].roomName)){
      *currentRoom = i;
      addToPath(playerPath, rooms[i].roomName);
      return;
    }
  }
  printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
}

int main(){
  int i, currentRoom, endRoom;
  char directoryName[250];
  char *buffer;
  size_t bufferSize = 32;
  struct room rooms[AD_NUM_ROOMS];
  struct path playerPath;
  //setup the initial path to have nothing in it. 
  playerPath.pathLength = 0;
  playerPath.pathSize = 10;
  playerPath.pathList = malloc(sizeof(char*) * playerPath.pathSize); //allocate the memoryt for up to 10 steps


  //Get the data from the room files created just before this was run
  getNewestDirectory(directoryName);
  readRooms(directoryName, (struct room *)rooms);

  //get the starting and ending rooms

  currentRoom = getRoomByType(rooms, "START_ROOM");
  endRoom = getRoomByType(rooms, "END_ROOM");

  buffer = malloc(bufferSize * sizeof(char));
  //Start the game
  while(currentRoom != endRoom){
    displayCurrentLocation(rooms, currentRoom);
    fflush(stdin);
    getline(&buffer, &bufferSize, stdin);
    printf("UserIn: %s", buffer);
    userChoice(&currentRoom, rooms, &playerPath, buffer);
  }

  free(playerPath.pathList);
  free(buffer);
  return 0;
}
