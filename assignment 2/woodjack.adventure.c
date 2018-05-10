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

Room* loadRooms() {

  DIR *d;
  struct dirent *de;
  d = opendir(".");
  if (d) {
    int found = 0;
    while ((de = readdir(d)) != NULL) {

      char buffer[15];
      snprintf(buffer, 15, "%s", de->d_name);
      printf("%s\n", buffer);

      if (strcmp(buffer, "woodjack.rooms.") != 0) {
        found = 1;
        printf("yeah");
        break; /* Found it! */
      }

    }
    closedir(d);
  }

  // /* Look for directory with rooms */
  // struct dirent* de;  /* Pointer for directory entry */
  // DIR* dr = opendir(".");
  // assert (dr != NULL);
  // int found = 0;
  // while (de = readdir(dr)) {
  //   char buffer[15];
  //   snprintf(buffer, 15, "%s", de->d_name);
  //   printf("%s\n", buffer);
  //   if (strcmp(buffer, "woodjack.rooms.") != 0) {
  //     found = 1;
  //     printf("yeah");
  //     break; /* Found it! */
  //   }
  // }
  // closedir(dr);

  // /* Open the directory that was found */
  // DIR* dr2 = opendir(de->d_name);
  // closedir(dr);
  //
  // /* Load the contents of each file into a room array */
  // while ((de = readdir(dr2)) != NULL) {
  //   printf("Read dir: %s\n", de->d_name);
  //   //readContents(de->d_name);
  // }
  //
  // /* Search for the starting room */
  // int i;
  // while (i < 7) {
  //   if (rooms[i]->roomType == "START_ROOM") {
  //     /* Return a pointer to the starting room */
  //     return rooms[i];
  //   }
  // }
  // return rooms[0];
}

void printLocation(Room* r) {
  char buffer[100];
  sprintf(buffer, "Current Location: %s", r->name);
}

int main() {
  /* Load rooms into memory */
  Room* r = loadRooms();

  /* Start game loop */
  while (1) {
    /* If this isn't the end room... */
    if (r->roomType != "END_ROOM") {
        /* Print the name of the room */
        //printLocation(r);

        /* Print possible connections */
        //char** connections = printConnections(r);

        /* Ask "WHERE TO? >" */
        printf("WHERE TO? >");

        /* Parse user input */

        /* If invalid, print "HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN." */
        /* Save user's choice (if valid) to path history and step count. */
    } else {
      /* If this is the end room... */
        /* Print the name of the room, and indicate this is the end room. */
        printLocation(r);
        printf("This is the end room.\n");

        /* Print the number of steps, and the path */
        /* Print "Congratulations!" */
        printf("Congratulations!\n");
        /* Exit the application */
        break;
    }
  }

  /* Free memory associated with rooms */
  int i;
  for (i = 0; i < 7; i++) {
    free(rooms[i]);
  }

  return 0;
}
