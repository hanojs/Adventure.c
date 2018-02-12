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
    char blackListDir[5];
    DIR *directoryPointer = opendir(".");
    struct dirent *direntPointer;
    struct stat dirStat;
    time_t latest = 0;

    blackListDir[0] = '.';
    blackListDir[1] = '\0';
    while ((direntPointer = readdir(directoryPointer)) != NULL) { //while there are more directories
        memset(&dirStat, 0, sizeof(dirStat));                     //allocate the memorY
        //printf("dir Name : %s Done \n", direntPointer->d_name);
        if (stat(direntPointer->d_name, &dirStat) >= 0)
            if (S_ISDIR(dirStat.st_mode))
                if(strcmp(direntPointer->d_name, blackListDir));
                    if (dirStat.st_mtime > latest)
                    {                   //if the directory is newer than the old one...
                        strcpy(directoryName, direntPointer->d_name);

                        printf("DirName: %s Latest: %i, ST_mtime: %i\n", direntPointer->d_name, latest, dirStat.st_mtime);
                        latest = dirStat.st_mtime;

                        //printf("Get newest 123 %s %s\n", directoryName, direntPointer->d_name);
                    }

    }

    printf("Get newestLast %s\n", directoryName);
    closedir(directoryPointer);
}

int getFileLine(char *buff, size_t nSize, int firstChar, FILE *fs){
  char *tmp = buff;
  fgets(buff, nSize, fs);
  printf("Get File Line: %s DONE\n", buff);


  while(*tmp){
    if(*tmp=='\n')
      *tmp='\0';
    tmp++;
  }

  if(!(buff[0]))
    return 0;
  return 1;
}



void storeRoom(FILE *fs, struct room *rooms, int roomNum){
    int i = 0;
    char *buff = malloc(100 * sizeof(char));
    char *name = malloc(20 * sizeof(char));
    size_t nSize = sizeof(buff);
    //Read/store the room name

    //printf("ROOM NAME %s \n", &buff[12]);
    //memcpy(rooms[roomNum].roomName, &buff[12], sizeof(rooms[roomNum].roomName));


    //Read/Store all the connections. The last fget will read the room type
    //fgets(buff, nSize, fs);
    getFileLine(buff, nSize, 12, fs);
    //printf("CONNECTION %s\n", &buff[15]);
    while(getFileLine(buff, nSize, 15, fs)){
      //memcpy(rooms[roomNum].connections[i], &buff[15], sizeof(rooms[roomNum].connections[i]));
      i++;
      //fgets(buff, nSize, fs);
      printf("ROOM TYPE %s\n", buff);
    }

    //store the roomType
    //memcpy(rooms[roomNum].roomType, &buff[12], sizeof(rooms[roomNum].roomType));

    return;
}

void readRooms(char directoryName[250], struct room *rooms){
    int i = 0;
    char buff[250];

    DIR *directoryPointer = opendir(directoryName);
    FILE *fs;
    struct dirent *direntPointer;
    printf("Directory Name: %s\n", directoryName);
    while ((direntPointer = readdir(directoryPointer)) != NULL) { //while there are more directories
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
  //readRooms(directoryName, (struct room *)rooms);

  return 0;
}
