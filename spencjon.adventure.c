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
    while ((direntPointer = readdir(directoryPointer)) != NULL) { //while there are more directories
        memset(&dirStat, 0, sizeof(dirStat)); //allocate the memorY

        if (!(stat(direntPointer->d_name, &dirStat) < 0))
            if (S_ISDIR(dirStat.st_mode))
                if (dirStat.st_mtime > latest)
                { //if the directory is newer than the old one...
                    strcpy(directoryName, direntPointer->d_name);
                    latest = dirStat.st_mtime;
                }
            else
                continue; //if it can't be gotten into..
        else
            continue; //IF IT ISN'T A directory
    }
    closedir(directoryPointer);
}

void storeRoom(FILE *fs, struct room *rooms, int roomNum){
    int i = 0;
    char buff[255];
    char name[20];

    //Read/store the room name
    fgets(buff, sizeof(buff), fs);

    memcpy(rooms[roomNum].roomName, &buff[12], sizeof(rooms[roomNum].roomName));

    //Read/Store all the connections. The last fget will read the room type
    fgets(buff, sizeof(buff), fs);
    while(buff[0] == 'C'){
      memcpy(rooms[roomNum].connections[i], &buff[15], sizeof(rooms[roomNum].connections[i]));
      i++;
      fgets(buff, sizeof(buff), fs);
    }

    //store the roomType
    memcpy(rooms[roomNum].roomType, &buff[12], sizeof(rooms[roomNum].roomType));

    return;
}

void readRooms(char directoryName[250], struct room *rooms){
    int i = 0;
    char buff[250];
    DIR *directoryPointer = opendir(directoryName);
    FILE *fs;
    struct dirent *direntPointer;

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
  readRooms(directoryName, (struct room *)rooms);

  return 0;
}
