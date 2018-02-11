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


#ifndef AD_VALUES
#define AD_NUM_ROOMS 7
#define AD_MIN_CONN 3
#define AD_MAX_CONN 6
#define AD_NAME_INITIAL 9
#endif


struct room {
  char roomName[AD_NAME_INITIAL];
  char roomType[20];
  int connections[AD_MAX_CONN];
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
    while ((direntPointer = readdir(directoryPointer)) != NULL) { //while there are more directories
        memset(&dirStat, 0, sizeof(dirStat)); //allocate the memorY

        if (!S_ISDIR(direntPointer.st_mode)){
          printf("Not a directory \n");
          continue; //IF IT ISN'T A directory
        }

        if (stat(direntPointer->d_name, &dirStat) < 0){
           printf("Cant be gotten into \n");
           continue; //if it can't be gotten into..
        }

        if (dirStat.st_mtime > latest) { //if the directory is newer than the old one...
            strcpy(directoryName, direntPointer->d_name);
            latest = dirStat.st_mtime;
        }

        direntPointer = readdir(directoryPointer);
        printf("Directory name: %s \n", directoryName);
    }
    closedir(directoryPointer);
    printf("Final Directory name: %s \n", directoryName);
}

//void readRooms(){}

int main(){
  char directoryName[250];
  struct room *rooms;
  struct path pathList;

  //path = malloc(sizeof(char*) * pathLength);  //We will use this



  getNewestDirectory(directoryName);
  //readRooms(directoryName, (struct room *)rooms);
  printf("%s", directoryName);



  return 0;
}
