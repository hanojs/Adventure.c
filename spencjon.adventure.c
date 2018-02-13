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
};
struct path {
  char **path;
  int pathLength;
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
  fgets(tmp, sizeof(tmp), fs);
  printf("Get File Line TMP: ---%s---\n", tmp);
  memcpy(buff, &tmp[firstChar], sizeof(tmp));
  printf("Get File Line BUFF: ---%s---\n", buff);

  i = 0;
  while(buff[i]){
    if(buff[i]=='\n')
      buff[i]='\0';
    i++;
  }

  printf("Get File Line buff AFTER: ---%s---\n", buff);

  
  if(buff)
    return 1;
  return 0;
}



void storeRoom(FILE *fs, struct room *rooms, int roomNum){
    int i = 0;
    char *buff = malloc(100 * sizeof(char));
    char *name = malloc(20 * sizeof(char));
    size_t nSize = sizeof(buff);
    //Read/store the room name

    printf("ROOM NAME %s \n", &buff[12]);
    memcpy(rooms[roomNum].roomName, &buff[12], sizeof(rooms[roomNum].roomName));


    //Read/Store all the connections. The last fget will read the room type
    fgets(buff, nSize, fs);
    //getFileLine(buff, nSize, 12, fs);
    //printf("CONNECTION %s\n", &buff[15]);
    while(getFileLine(buff, 15, fs)){
      //memcpy(rooms[roomNum].connections[i], &buff[15], sizeof(rooms[roomNum].connections[i]));
      //i++;
      //fgets(buff, nSize, fs);
      //printf("ROOM TYPE %s\n", buff);
    }

    //store the roomType
    //memcpy(rooms[roomNum].roomType, &buff[12], sizeof(rooms[roomNum].roomType));

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
        snprintf( buff, sizeof( buff ) - 1, "./%s/%s", directoryName, direntPointer->d_name);
        fs = fopen(buff, "r");
        storeRoom(fs, rooms, i);
        i++; 
        fclose(fs);
    }
    closedir(directoryPointer);
    return;
}

int main(){
  char directoryName[250];
  struct room rooms[AD_NUM_ROOMS];
  struct path pathList;

  //path = malloc(sizeof(char*) * pathLength);  //We will use this
  getNewestDirectory(directoryName);
  printf("Main Dir Name %s\n", directoryName);
  readRooms(directoryName, (struct room *)rooms);

  return 0;
}
