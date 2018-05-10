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


int main() {
  /* Load rooms into memory */

  /* Start game loop */
    /* If this isn't the end room... */
      /* Print the name of the room */
      /* Print possible connections */
      /* Ask "WHERE TO? >" */
      /* Parse user input */
      /* If invalid, print "HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN." */
      /* Save user's choice (if valid) to path history and step count. */
    /* If this is the end room... */
      /* Print the name of the room, and indicate this is the end room. */
      /* Print the number of steps, and the path */
      /* Print "Congratulations!" */
      printf("Congratulations!\n")
      /* Exit the application */
      return 0;
}
