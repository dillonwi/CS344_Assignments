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
 char* outgoing[6]; /* Since all of the rooms are stored in an array, this */
 char* roomType;  /*   stores the index of neighboring rooms. */
};

/* Create array of room pointers */
Room* rooms[7];

void loadRoom(char* filename, int i) {
  /* Open this room's corresponding file */
  FILE* fd = fopen(filename, "r");
  assert(fd);

  /* Allocate memory */
  Room* r = (Room*) malloc(sizeof(Room));

  /* Initialize outgoing string pointers to null */
  int k;
  for (k = 0; k < 6; k++) {
    r->outgoing[k] = NULL;
  }

  /* Buffer used to read from tile */
  char buffer[256];

  /* Read & save room's name */
  char* name = (char*) malloc(sizeof(char) * 256);
  fgets(buffer, 256, fd);
  buffer[strcspn(buffer, "\n")] = 0;
  memset(name, '\0', sizeof(name));
  memcpy(name, &buffer[11], 255);
  r->name = name;

  /* loop through remaining lines and save connections & roomType */
  int j = 0;
  while(fgets(buffer, 256, fd)) {
    buffer[strcspn(buffer, "\n")] = 0;
    if ((char) buffer[12] == ':') {
      char* c = (char*) malloc(sizeof(char) * 256);
      memset(c, '\0', sizeof(c));
      memcpy(c, &buffer[14], 255);
      r->outgoing[j] = c;
      j++;
    }
    if ((char) buffer[9] == ':') {
      char* c = (char*) malloc(sizeof(char) * 256);
      memset(c, '\0', sizeof(c));
      memcpy(c, &buffer[11], 255);
      r->roomType = c;
    }
  }

  /* Close file */
  fclose(fd);

  /* Add to rooms array */
  rooms[i] = r;
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

Room* findStart() {
  int i = 0;
  while (strcmp(rooms[i]->roomType, "START_ROOM") != 0 && i < 7) {
    i++;
  }
  return rooms[i];
}

void printLocation(Room* r) {
  char buffer[100];
  sprintf(buffer, "Current Location: %s", r->name);
  printf("%s\n", buffer);
}

void printConnections(Room* r) {
  int j = 0;
  printf("POSSIBLE CONNECTIONS: ");
  while (r->outgoing[j] != NULL) {
    if (r->outgoing[j + 1] != NULL)
      printf("%s, ",r->outgoing[j]);
    else
      printf("%s.",r->outgoing[j]);
    j++;
  }
}

int main() {
  /* Load rooms into memory */
  loadRooms();

  /* Find start room */
  Room* r = findStart();

  /* Start game loop */
  while (1) {
    /* If this isn't the end room... */
    if (strcmp(r->roomType, "END_ROOM") != 0) {
        /* Print the name of the room */
        printLocation(r);

        /* Print possible connections */
        printConnections(r);

        /* Ask "WHERE TO? >" */
        printf("WHERE TO? >\n");
        exit(1);

        /* Parse user input */

        /* If invalid, print "HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN." */
        /* Save user's choice (if valid) to path history and step count. */
    } else {
      // /* If this is the end room... */
      //   /* Print the name of the room, and indicate this is the end room. */
      //   printLocation(r);
      //   printf("This is the end room.\n");
      //
      //   /* Print the number of steps, and the path */
      //   /* Print "Congratulations!" */
      //   printf("Congratulations!\n");
      //   /* Exit the application */
      //   break;
    }
  }

  // /* Free memory associated with rooms */
  // int i;
  // for (i = 0; i < 7; i++) {
  //   free(rooms[i]->name);
  //   free(rooms[i]->roomType);
  //   int j = 0;
  //   while(rooms[i]->outgoing[j] != NULL) {
  //     free(rooms[i]->outgoing[j]);
  //     j++;
  //   }
  //   free(rooms[i]);
  // }

  return 0;
}
