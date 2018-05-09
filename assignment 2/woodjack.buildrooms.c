/*
 * CS 344 Assignment 2 - Adventure Game
 * By Jack Woods - May 8th, 2018
 * woodjack@oregonstate.edu
 */

#include <stdlib.h> /* malloc */
#include <stdio.h> /* file io and sprintf */
#include <time.h> /* rand() seed */
#include <pthread.h> /* Practicing multithreading with file IO */
#include <sys/stat.h> /* mkdir */
#include <unistd.h> /* getpid() */
#include <sys/types.h> /* getpid() */
#include <dirent.h> /* enter directories */

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
    }
    emptyLoc++;
  }

  o->outgoing[emptyLoc] = index;

  /* Connect r to o */
  emptyLoc = 0;
  foundEmpty = 0;
  while (!foundEmpty) {
    if (r->outgoing[emptyLoc] == -1) {
      foundEmpty = 1;
    }
    emptyLoc++;
  }

  r->outgoing[emptyLoc] = otherLoc;
}

/* *** Generate Rooms *** */
/*
 * Start in the starting room, and generate rooms in a 10x10x10 3-D array. I
 * chose a 3-D array because each cell has 6 sides, and each room can only be
 * connected to up to 6 others (as specified by the assignment requirements).
 * Rules for creating a room:
  - A room must be created next to at least one other room.
  - Any bordering rooms will be connected.
 * Algorithm (Pseudocode):
  1. A 10x10x10 array is created, and all cells are populated with -1s.
  2. The cell located at 5,5,5 is selected by assigning x/y/z variables.
  3. The previous location is initialized to 0,0,0.
  4. The index variable is initialized to 0.
  5. While the index variable's value does not exceed 6:
    5a. Create a room at the currently selected location.
    5b. Assign a random name to the new room. Remove the name from the name list.
    5c. Assign a roomType. If index = 0, this is the starting room. If
        index = 6, this is the ending room. Else, this is a middle room.
    5d. Search neighboring cells for non-negative numbers. Store empty cells in
        a list.
      5da. Create a passageway between any neighboring rooms (unless one
      already exists).
    5e. Randomly select a cell from the list of empty cells.
    5f. Set current and previous location variables accordingly.

*/
void generateRooms() {
  /* Create array */
  int roomArr[10][10][10];
  int i;
  int j;
  int k;
  for(i = 0; i < 10; i++) {
    for(j = 0; j < 10; j++) {
      for(k = 0; k < 10; k++) {
        roomArr[i][j][k] = -1;
      }
    }
  }

  /* Initialize current and previous location variables */
  Location current;
  current.x = 5;
  current.y = 5;
  current.z = 5;

  Location prev;
  prev.x = 5;
  prev.y = 5;
  prev.z = 5;

  /* Initialize index and Begin loop (step 5) */
  int index;
  for(index = 0; index < 7; index++) {

    /* Create a room at the selected location */
    roomArr[current.x][current.y][current.z] = index;
    Room* r = (Room*) malloc(sizeof(Room));
    r->name = randomName();

    /* Nullify all outgoing connections */
    int i;
    for (i = 0; i < 6; i++) {
      r->outgoing[i] = -1;
    }

    switch(index) {
      case 0:
        r->roomType = "START_ROOM";
        break;
      case 6:
        r->roomType = "END_ROOM";
        break;
      default:
        r->roomType = "MID_ROOM";
    }

    int locations[6] = {[0 ... 5] = -1}; /* Stores any empty locations */
    int loc = 0;

    /* Check X directions */
    if (roomArr[current.x + 1][current.y][current.z] > -1) {
      /* Create connection */
      createConnection(r, index, roomArr[current.x + 1][current.y][current.z], *rooms);
    } else {
      /* Mark location as empty */
      locations[loc] = 100; /* XYZ + 100 = X+1,Y,Z */
      loc++;
    }
    if (roomArr[current.x - 1][current.y][current.z] > -1) {
      /* Create connection */
      createConnection(r, index, roomArr[current.x - 1][current.y][current.z], *rooms);
    } else {
      /* Mark location as empty */
      locations[loc] = -100; /* XYZ + -100 = X-1,Y,Z */
      loc++;
    }

    /* Check Y directions */
    if (roomArr[current.x][current.y + 1][current.z] > -1) {
      /* Create connection */
      createConnection(r, index, roomArr[current.x][current.y + 1][current.z], *rooms);
    } else {
      /* Mark location as empty */
      locations[loc] = 10;
      loc++;
    }
    if (roomArr[current.x][current.y - 1][current.z] > -1) {
      /* Create connection */
      createConnection(r, index, roomArr[current.x][current.y - 1][current.z], *rooms);
    } else {
      /* Mark location as empty */
      locations[loc] = -10;
      loc++;
    }

    /* Check Z directions */
    if (roomArr[current.x][current.y][current.z + 1] > -1) {
      /* Create connection */
      createConnection(r, index, roomArr[current.x][current.y][current.z + 1], *rooms);
    } else {
      /* Mark location as empty */
      locations[loc] = 1;
      loc++;
    }
    if (roomArr[current.x][current.y][current.z - 1] > -1) {
      /* Create connection */
      createConnection(r, index, roomArr[current.x][current.y][current.z - 1], *rooms);
    } else {
      /* Mark location as empty */
      locations[loc] = -1;
      loc++;
    }

    /* Save current room to array */
    rooms[index] = r;

    /* Set previous location */
    prev.x = current.x;
    prev.y = current.y;
    prev.z = current.z;

    /* Choose location of next room */
    int nextLoc = rand() % (loc); /* Chooses a new direction */
    nextLoc = locations[nextLoc];

    current.z = 5 + (nextLoc % 10);
    nextLoc /= 10;
    current.y = 5 + (nextLoc % 10);
    nextLoc /= 10;
    nextLoc = nextLoc - (nextLoc % 10);

  }

}

void saveRoom(char* dirName) {

  char filename[100];
  sprintf(filename, "%s/%s.txt", dirName, rooms[i]->name);
  fopen(filename, )

}

/* Saves rooms to files in a given directory. */
void saveRooms() {

  /* Make directory */
  char dirName[50];
  sprintf(dirName, "woodjack.rooms.%ld", (long) getpid());
  int result = mkdir(dirName, 0755);

  /* Open rooms file for writing */
  pthread_t threads[7];
  int results[7;]
  int i;
  for (i = 0; i < 7; i++) {
    results[i] = pthread_create(&threads[i], NULL, saveRoom, dirName);
    assert(0 == results[i]);
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
