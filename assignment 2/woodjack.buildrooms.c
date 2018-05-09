/*
 * CS 344 Assignment 2 - Adventure Game
 * By Jack Woods - May 8th, 2018
 * woodjack@oregonstate.edu
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

typedef struct Location Location;
struct Location {
  int x;
  int y;
  int z;
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

void createConnection(Room* r, int index, int otherLoc, Room* rooms) {
  Room* o = &rooms[otherLoc];

  /* Connect o to r */
  int emptyLoc = 0;
  int foundEmpty = 0;
  while (!foundEmpty) {
    if (o->outgoing[emptyLoc] == -1) {
      foundEmpty = 1;
      printf("Adding %d to %d.\n", index, otherLoc);
      o->outgoing[emptyLoc] = index;
    }
    emptyLoc++;
  }

  /* Connect r to o */
  emptyLoc = 0;
  foundEmpty = 0;
  while (!foundEmpty) {
    if (r->outgoing[emptyLoc] == -1) {
      foundEmpty = 1;
      printf("Adding %d to %d.\n", otherLoc, index);
      r->outgoing[emptyLoc] = otherLoc;
    }
    emptyLoc++;
  }
}

/* *** Generate Rooms *** */


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
      while (rooms[i]->outgoing[j] != -1) {
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

  /* TODO: add at least 3 outgoing connections. */

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
