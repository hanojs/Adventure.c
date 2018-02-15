/*************************************
*PTHREAD CREATION IN LINE: 285, 265
*MUTEX LOCK IN LINE:       258, 235
*MUTEX UNLOCKS ARE A FEW LINES UNDER EACH LOCK
************************************/
#include <sys/types.h>  
#include <sys/stat.h> //pid
#include <unistd.h> 
#include <dirent.h> //for dirent pointer
#include <stdio.h>  
#include <string.h> //memset strcpy
#include <stdlib.h> //malloc
#include <time.h>
#include <pthread.h> //multithreading


//various commonly used numbers so I can standardize and avoid some human error in the code
#ifndef AD_VALUES 
  #define AD_NUM_ROOMS 7
  #define AD_MIN_CONN 3
  #define AD_MAX_CONN 6
  #define AD_NAME_INITIAL 9
#endif

FILE *currentTime; //global file pointer to be used with mutexs later to satisfy the assignment
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //This sn't exactly the most efficient use of mutual exclusion, but it is a use 

//stores the name, type, and connections of a room
struct room {
  char roomName[AD_NAME_INITIAL]; //Easily refrenced name that is one bigger than the biggest name size to accomidate null
  char roomType[20];//room type is less tan 20 long, so that worked
  char connections[AD_MAX_CONN][AD_NAME_INITIAL]; //t
  int numCon;
};

//stores the path a player takes
struct path {
  char **pathList;//dynamic pathlength as that is decided by the user
  int pathLength; //the path length the user took (starts at 0)
  int pathSize; //the size of the current dynamic array of path list. USed to increase the size if needed. 
};

//gets the newest directory (that isn't the current dir or parent dirs) 
void getNewestDirectory(char directoryName[250]){
    DIR *directoryPointer = opendir("."); //look in the current directory...
    struct stat dirStat;
    struct dirent *direntPointer;
    time_t latest = 0;
    direntPointer = readdir(directoryPointer); //Get the first file/directory to test
    while ((direntPointer = readdir(directoryPointer)) != NULL) { //while there are more directories or files
        memset(&dirStat, 0, sizeof(dirStat)); //allocate the memory fpr the stat struct of that folder/file

        if (!(stat(direntPointer->d_name, &dirStat) < 0))
            if (S_ISDIR(dirStat.st_mode)){ //test to make sure it's a directory
                if (dirStat.st_mtime > latest)
                { //if the directory is newer than the old one, replace the old with the new and continue on
                    strcpy(directoryName, direntPointer->d_name);
                    latest = dirStat.st_mtime;
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
}

//Gets a line from a file for storeRoom
int getFileLine(char *buff, int firstChar, FILE *fs){
  char tmp[240];
  int i;

  if(!fgets(tmp, sizeof(tmp), fs)){
    return 0;
  }

  if(tmp[5] == 'T'){ //if the file line is reading Room Type, you copy over the 11th and after chars
    firstChar = 11;
  }

  memcpy(buff, &tmp[firstChar], sizeof(tmp)); //copy over the specified chars

  i = 0;
  while(buff[i]){ //remove all newlines
    if(buff[i]=='\n')
      buff[i]='\0';
    i++;
  }

  if(tmp[5] == 'T'){ //if the room tpye was read, that was the last line and return 0
    return 0;
  }
  
  return 1; //if it wasn't the room type return 1
}


//Copys the room name, connections, and toom type to a specified room struct
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
        //skip if the directory name is the current directory or any previous for linux
        if(direntPointer->d_name[0] == '.'){
          continue;
        }
        //create the file path
        snprintf( buff, sizeof( buff ) - 1, "./%s/%s", directoryName, direntPointer->d_name);
        //open the file and store the room
        fs = fopen(buff, "r");
        storeRoom(fs, rooms, i);
        i++; 
        fflush(stdin); //Got rid of occasional errors with stdin corrupting the fclose 
        if(fs == NULL){
          fclose(fs); 
        }
    }
    closedir(directoryPointer);
    return;
}

//prints the current location, and connected rooms. Also asks "Where to"
void displayCurrentLocation(struct room *rooms, int i){
  int j = 0;
  fflush(stdout);
  printf("\nCURRENT LOCATION: %s\n", rooms[i].roomName);

  //POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
  printf("POSSIBLE CONNECTIONS: %s", rooms[i].connections[j]);
  for(j = 1; j < rooms[i].numCon; j++)
    printf(", %s", rooms[i].connections[j]);
  printf(".\n");


  //WHERE TO? > (Then get the input)
  printf("WHERE TO? >");
}

//Returns the room number (0-6) that contains the room type given in the second arg
int getRoomByType(struct room *rooms, char *type){
  int i;
  //go through each until the room is found
  for(i = 0; i < AD_NUM_ROOMS; i++){
    if(strcmp(rooms[i].roomType, type)) continue; //continues searching if strings aren't equal
    return i;
  }

  return 1;
}

//Adds a room to a path so the path will be extended
void addToPath(struct path *playerPath, char *roomName){
  char **tmpPath;
  int i = 0;
  playerPath->pathLength++; //A thing was added so we need to increment that path length

  //if the path is longer or equal to the path we've already allocated, double it and free the old one 
  if(playerPath->pathLength >= playerPath->pathSize){
    playerPath->pathSize = 2 * playerPath->pathSize; //double the space in the path
    tmpPath = malloc(sizeof(char*) * playerPath->pathSize); 
    for(i = 0; i < playerPath->pathSize; i++){ //copy the old list to the new one with more space
      tmpPath[i] = playerPath->pathList[i];
    }
    free(playerPath->pathList); //clean the old one
    playerPath->pathList = tmpPath; //the OG gets the new path
  }

  playerPath->pathList[playerPath->pathLength - 1] = roomName;
}

//We need to get user input and take out the newlines so comparisons work well and. 
void getUserInput(char **buffer, size_t *bufferSize){
  int i;
  char *tmpBuffer = malloc(*bufferSize * sizeof(char)); //freed in main after every loop, or in getUser choice if time is selected
  
  fflush(stdin); //user input 1/10 times would get corrupted by the stdin buffer not being clear
  getline(&tmpBuffer, bufferSize, stdin);
  i = 0;
  //go through the entire buffer and replace newlines with NULL
  while(i < *bufferSize){
    //printf("%i %c\n", i,tmpBuffer[i]);
    if(tmpBuffer[i]=='\n')
      tmpBuffer[i]='\0';
    i++;
  }
  //the tmpBuffer becomes the OG buffer
  *buffer = tmpBuffer;
}

//Prints the time (assuming it is the one and only line in the file) from a file while in a thread
void* displayTime(){
  char buffer[250]; //the 250 buffer size is arbitrary. Larger than the time, and deosn't provide too much overhead for this application
  printf("\n");
  pthread_mutex_lock(&mutex); //LOCK the file so it can't be written to while it's being read
  currentTime = fopen("currentTime.txt","r");
  fgets(buffer, sizeof(buffer), currentTime);
  printf("%s\n\n", buffer); //write the file name
  fclose(currentTime);
  pthread_mutex_unlock(&mutex);//unlock the file //this had to be second so that the writing will happen first
  return;
}

//Will write the time (while locking the file pointer). This also starts the display thread so I make sure this runs and locks before the display can lock the file
void* writeTime(void *arg){
  pthread_t *displayingTime = (pthread_t*) arg;
  char curTime[250];
  time_t rawtime;
  struct tm * timeinfo;
  
  //Get and setup buffer to then print into the file
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  // 1:03pm, Tuesday, September 13, 2016 ---- Hour is 0 padded, and days are 0 padded
  strftime (curTime,250,"%I:%M%p, %A, %B, %d, %G",timeinfo);

  //Lock the file and write to it so others can't interfere
  pthread_mutex_lock(&mutex);
  currentTime = fopen("currentTime.txt","w+");
  fprintf(currentTime, "%s", curTime); //write the file name
  fclose(currentTime);
  pthread_mutex_unlock(&mutex);

  //Display the time written. This is after the mutex because otherwise we can't tell which willfor sure lock the file first.
  pthread_create(displayingTime, NULL, displayTime, NULL); //we need the write to lock the file first
  pthread_join(*displayingTime, NULL); //after you write it, wait for it to be read.

  return;
}


void userChoice(pthread_t *displayingTime, pthread_t *writingTime, int *currentRoom, struct room *rooms, struct path *playerPath, char *userIn){
  int i, flag = 0;
  char *buffer;
  size_t bufferSize = 32;

  //print time...

  /***********************************************
   * PTHREAD CREAT / JOINs 
   * MUTEXs are in the 2 functions directly above -- WriteTime() and Displaytime()
   * *******************************************/
  if(!strcmp(userIn, "time")){

    pthread_create(writingTime, NULL, writeTime, displayingTime);
    pthread_join(*writingTime, NULL);   
    free(buffer); //clean up before we get user input again

    //Re-Print the where to, get user input, and then run userChoice again
    printf("WHERE TO? >");
    getUserInput(&buffer, &bufferSize);
    userChoice(displayingTime, writingTime, currentRoom, rooms, playerPath, buffer); //nested so that the vurrent locations doesn't play again.
    return;//main will free buffer, or another instance of this option will.
  }

  //test if the asked for room is in the list of connections, if it is, we can't go to that room anyways
  for(i = 0; i < rooms[*currentRoom].numCon; i++){
    if(strcmp(userIn, rooms[*currentRoom].connections[i])) continue;
    flag = 1;
    break;
  }

  //if the room is in the list of connections, find what room number that, go to it, and add it to the path right away
  if(flag){ //I'm using a flag because it's a bit more readable for me than an extra for loop that runs after a room has been matched
    for(i = 0; i < AD_NUM_ROOMS; i++){
      if(strcmp(userIn, rooms[i].roomName)) continue;
      *currentRoom = i;
      addToPath(playerPath, rooms[i].roomName);
      return;
    }
  }

  //if not time or a connected room...
  printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
}


//prints the end statements and loops through the path to print the whole path. 
void printEndOfGame(struct path *playerPath){
  int i;
  printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
  printf("YOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS:\n", playerPath->pathLength);
  for(i = 0; i < playerPath->pathLength; i++){
    printf("%s\n", playerPath->pathList[i]);
  }
}


int main(){
  int i, currentRoom, endRoom;
  char directoryName[250]; //buffer for the directory name that was last created
  char *buffer; //buffer is a dynamic array. Memory is allocated in 
  size_t bufferSize = 32; 
  pthread_t writingTime; //will be used later to create threads
  pthread_t displayingTime; 
  struct room rooms[AD_NUM_ROOMS]; //list of rooms
  struct path playerPath; //holds the path


  //setup the initial path to have nothing in it because the path is empty
  playerPath.pathLength = 0;
  playerPath.pathSize = 10;
  playerPath.pathList = malloc(sizeof(char*) * playerPath.pathSize); //allocates the memoryt for up to 10 steps initially that can be expanded if the path grows longer


  //Get the data from the room files created just before this was run 
  getNewestDirectory(directoryName);
  readRooms(directoryName, (struct room *)rooms);

  //get the starting and ending rooms so we can test to see if we've won or if we should keep going
  currentRoom = getRoomByType(rooms, "START_ROOM");
  endRoom = getRoomByType(rooms, "END_ROOM");

  //Start the game --- I'm using this loop structure to keep from getting too many recursive calls.
  while(currentRoom != endRoom){
    displayCurrentLocation(rooms, currentRoom);
    getUserInput(&buffer, &bufferSize);
    userChoice(&displayingTime, &writingTime, &currentRoom, rooms, &playerPath, buffer);
    free(buffer); //clean up buffer
  }

  //end the game 
  printEndOfGame(&playerPath);

  //clean up.. Buffer has been cleaned in the main while loop
  free(playerPath.pathList);
  return 0;
}
