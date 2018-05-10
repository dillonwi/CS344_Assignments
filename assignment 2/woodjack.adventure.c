/*
 * CS 344 Assignment 2 - Adventure Game
 * By Jack Woods - May 8th, 2018
 * woodjack@oregonstate.edu
 * This script allows the user to traverse the rooms created in the other C
 * program.
 */

#include <stdlib.h> /* malloc */
#include <stdio.h> /* file io and sprintf */
#include <time.h> /* rand() seed */
#include <sys/stat.h> /* mkdir */
#include <fcntl.h> /* open() */
#include <unistd.h> /* getpid() and close() */
#include <sys/types.h> /* getpid() */
#include <dirent.h> /* enter directories */
#include <assert.h> /* assert */
#include <string.h> /* strlen() */

/*
* I would normally put header and accompanying function definitions in a header
* file, but this assignment only allows us to use 2 .c files.
*/
typedef struct Room Room;
struct Room {
 char* name;
 int outgoing[6]; /* Since all of the rooms are stored in an array, this */
 char* roomType;  /* stores the index of neighboring rooms. */
};

/* Create array of room pointers */
Room* rooms[7];

void loadRoom(char* filename, int i) {
  printf("%s\n", filename);
  FILE* fd = fopen(filename, "r");

  assert(fd);

  Room* r = (Room*) malloc(sizeof(Room));

  char buffer[256];

  char* name = (char*) malloc(sizeof(char) * 256);
  memset(r, '\0', sizeof(name));
  memcpy(name, &buffer[11], 255);
  r->name = name;

  while(fgets(buffer, 256, fd) && buffer[12] == ':') {
    printf("Connection");

  }

  fclose(fd);
}

void loadRooms() {
  struct dirent *de;
	DIR *dr = opendir(".");

	if (!dr) {
		printf("Could not open current directory." );
	}

  char dirName[256];
	while (de = readdir(dr)) {
    char buffer[16];
    memcpy(buffer, de->d_name, 15);

    if (strcmp(buffer, "woodjack.rooms.") == 0) {
      memcpy(dirName, de->d_name, 256);
    }
  }
	closedir(dr);

  dr = opendir(dirName);
  int i = 0;
  while (de = readdir(dr)) {
    if ((strcmp(de->d_name, ".") != 0) && (strcmp(de->d_name, "..") != 0)) {
      char filename[256];
      sprintf(filename, "%s/%s", dirName, de->d_name);
      loadRoom(filename, i);
      i++;
    }
  }
  closedir(dr);

}

void printLocation(Room* r) {
  char buffer[100];
  sprintf(buffer, "Current Location: %s", r->name);
}

int main() {
  loadRooms();

  return 0;
}
