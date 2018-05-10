/*
 * CS 344 Assignment 2 - Adventure Game
 * By Jack Woods - May 8th, 2018
 * woodjack@oregonstate.edu
 * This file builds rooms for the adventure game.
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

char* names[] = {
  "Foyer",
  "Graveyard",
  "Kitchen",
  "Bedroom 1",
  "Bedroom 2",
  "Living Room",
  "Office",
  "Basement",
  "Wine Cellar",
  "Backyard"
};
int taken[10];

/* Create array of room pointers */
Room* rooms[7];

/* Selects a name from the list at random. If the name is null, it tries again.
   If the name is not null, that name is returned and the pointer in the names
   array is set to null, preventing that name from being chosen again. */
char* randomName() {

  while(1) {
    int nameLoc = rand() % 10;
    if (taken[nameLoc] != 1) {
      char* name = names[nameLoc];
      taken[nameLoc] = 1;
      return name;
    }
  }
}

int createConnection(int index, int otherLoc) {
  Room* r = rooms[index];
  Room* o = rooms[otherLoc];

  /* Connect o to r */
  int loc1 = 0;
  while (loc1 < 6) {
    if (o->outgoing[loc1] == -1) {
      o->outgoing[loc1] = index;
      break;
    }
    loc1++;
  }

  /* Connect r to o */
  int loc2 = 0;
  while (loc2 < 6) {
    if (r->outgoing[loc2] == -1) {
      r->outgoing[loc2] = otherLoc;
      break;
    }
    loc2++;
  }
}

/* *** Generate Rooms *** */
/*
 * Before, I had a more complex algorithm that guaranteed that the floorplan
 * of the rooms geographically made sense. Due to time constraints and a
 * requirement that all rooms must have 3 connections, I created this simpler
 * algorithm instead.
 *
 * Algorithm:
   1. Loop from i = 0-6
 * 2. Malloc memory for a room & insert its pointer into the array at i.
 * 3. Assign the room a random name.
 * 4. If this is the first room being created, assign "START_ROOM" as the room
 *    type. If it is the last, assign "END_ROOM". Else, assign "MID_ROOM".
 * 5. Randomly assign 3-6 connections to each room.
 *    No duplicate connections should be allowed.
 */

 void generateRooms() {

   /* 1 */
   int i;
   for (i = 0; i < 7; i++) {
     /* 2 */
     rooms[i] = (Room*) malloc(sizeof(Room));
     /* 3 */
     rooms[i]->name = randomName();
     /* 4 */
     switch(i) {
       case 0:
        rooms[i]->roomType = "START_ROOM";
        break;
      case 6:
        rooms[i]->roomType = "END_ROOM";
        break;
      default:
        rooms[i]->roomType = "MID_ROOM";
      }

      /* Initialize all connections to empty */

      int j;
      for (j = 0; j < 6; j++) {
        rooms[i]->outgoing[j] = -1;
      }
    }
   /* 5 */
   int j;
   for (j = 0; j < 7; j++) {
     int numConnections = rand() % 4 + 3; /* 3 - 6 connections */

     for (i = 0; i < numConnections; i++) {

      int roomNum;
      do {
        roomNum = rand() % 7;
      } while (roomNum == j);

      int found = 0;
      int loc = 0;
      while (loc < 6) {
       if (rooms[j]->outgoing[loc] == roomNum) {
         found = 1;
         break;
       }
       loc++;
      }

      if (!found) {
        createConnection(j, roomNum);
      }
     }
   }
 }

/* Saves rooms to files in a given directory. */
void saveRooms() {

  /* Make directory */
  char dirName[50];
  sprintf(dirName, "woodjack.rooms.%ld", (long) getpid());
  int result = mkdir(dirName, 0755);

  /* Open rooms file for writing */
  int i;
  for (i = 0; i < 7; i++) {
    char filename[100];
    sprintf(filename, "%s/%s.txt", dirName, rooms[i]->name);

    int fd;
    fd = open(filename, O_WRONLY | O_CREAT, 0755 | O_TRUNC);
    if (fd < 0) {
      fprintf(stderr, "Could not open %s\n", filename);
      perror("Error in main()");
      exit(1);
    } else {
      char name[100];
      sprintf(name, "ROOM NAME: %s\n", rooms[i]->name);
      write(fd, name, strlen(name) * sizeof(char));

      int j = 0;
      while (rooms[i]->outgoing[j] > -1 && j < 6) {
        char buffer[100];
        sprintf(buffer, "CONNECTION %d: %s\n", j, rooms[rooms[i]->outgoing[j]]->name);
        write(fd, buffer, strlen(buffer) * sizeof(char));
        j++;
      }

      char type[100];
      sprintf(type, "ROOM TYPE: %s\n", rooms[i]->roomType);
      write(fd, type, strlen(type) * sizeof(char));
    }
    close(fd);
  }

}

int main(int argc, char* argv[]) {

  /* Initialize taken such that no names are taken. */
  int i;
  for (i = 0; i < 10; i++) {
    taken[i] = 0;
  }
  /* Seed random number generator */
  srand(time(NULL));

  /* Generate Rooms */
  generateRooms();

  /* Save rooms to a file */
  saveRooms();

  /* Free memory allocated to room structs. */
  for (i = 0; i < 7; i++) {
    free(rooms[i]);
  }

  return 0;
}
