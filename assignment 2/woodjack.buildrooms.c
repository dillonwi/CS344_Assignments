/*
 * CS 344 Assignment 2 - Adventure Game
 * By Jack Woods - May 8th, 2018
 * woodjack@oregonstate.edu
 */


#include <stdlib.h>
#include <stdio.h>

/*
 * I would normally put header and accompanying function definitions in a header
 * file, but this assignment only allows us to use 2 .c files.
 */
typedef struct Room Room;
struct Room {
  char* name;
  int* outgoing[6]; /* Since all of the rooms are stored in an array, this */
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

/* Creates a 10x10x10 array of negative numbers */
int createRoomArray() {
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
  return roomArr;
}

/* Selects a name from the list at random. If the name is null, it tries again.
   If the name is not null, that name is returned and the pointer in the names
   array is set to null, preventing that name from being chosen again. */
char* randomName() {
  while(0) {
    int nameLoc = rand() % 10;
    if (names[nameLoc] != NULL) {
      char* name = names[nameLoc];
      names[nameLoc] = NULL;
      return name;
    }
  }
}

/* This array of room pointers is just used to free memory that's dynamically
   allocated for rooms. */
Room* rooms[7];

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
Room* generateRooms(int numRooms) {
  /* Create array */
  int roomArr = createRoomArray()


  /* Create array of room pointers */
  Room* rooms[] = (Room*) malloc(sizeof(Room) * 7);

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
    r.name = randomName();

    switch(index) {
      case 0:
        r.roomType = "START_ROOM";
        break;
      case 6:
        r.roomType = "END_ROOM";
        break;
      default:
        r.roomType = "MID_ROOM";
    }

    int locations[6] = {[0 ... 5] = -1}; /* Stores any empty locations */
    int loc = 0;

    /* Check X directions */
    if (roomArr[current.x + 1][current.y][current.z] > -1) {
      /* Create connection */
      createConnection(r, roomArr[current.x + 1][current.y][current.z]);
    } else {
      /* Mark location as empty */
      locations[loc] = 100; /* XYZ + 100 = X+1,Y,Z */
      loc++;
    }
    if (roomArr[current.x - 1][current.y][current.z] > -1) {
      /* Create connection */
      createConnection(r, roomArr[current.x - 1][current.y][current.z]);
    } else {
      /* Mark location as empty */
      locations[loc] = -100; /* XYZ + -100 = X-1,Y,Z */
      loc++;
    }

    /* Check Y directions */
    if (roomArr[current.x][current.y + 1][current.z] > -1) {
      /* Create connection */
      createConnection(r, roomArr[current.x][current.y + 1][current.z]);
    } else {
      /* Mark location as empty */
      locations[loc] = 010;
      loc++;
    }
    if (roomArr[current.x][current.y - 1][current.z] > -1) {
      /* Create connection */
      createConnection(r, roomArr[current.x][current.y - 1][current.z]);
    } else {
      /* Mark location as empty */
      locations[loc] = -010;
      loc++;
    }

    /* Check Z directions */
    if (roomArr[current.x][current.y][current.z + 1] > -1) {
      /* Create connection */
      createConnection(r, roomArr[current.x][current.y][current.z + 1]);
    } else {
      /* Mark location as empty */
      locations[loc] = 001;
      loc++;
    }
    if (roomArr[current.x][current.y][current.z - 1] > -1) {
      /* Create connection */
      createConnection(r, roomArr[current.x][current.y][current.z - 1]);
    } else {
      /* Mark location as empty */
      locations[loc] = -001;
      loc++;
    }

    /* Set previous location */
    prev.x = current.x;
    prev.y = current.y;
    prev.z = current.z;

    /* Choose location of next room */
    int nextLoc = rand() % (loc + 1); /* Generates a new direction */
    nextLoc = locations[nextLoc];
    current.x = 5 + (nextLoc % 1000);
    current.y = 5 + (nextLoc % 100);
    current.z = 5 + (nextLoc % 10);

  }

  return NULL;
}

int main(int argc, char* argv[]) {

  /* Seed random number generator */
  srand(time(NULL));

  /* Generate Rooms */
  Room* rooms = generateRooms(7);

  /* Save rooms to a file */

  /* Free memory allocated to room structs. */

  return 0;
}
