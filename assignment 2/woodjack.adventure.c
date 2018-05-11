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
#include <pthread.h> /* multithreading */

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
  /* Since each directory has a unique PID appended to the end of its name, the
     application needs to search for the complete directory name. */
  struct dirent *de;
	DIR *dr = opendir(".");

  /* If the directory is null, throw an error. */
	assert(dr);

  /* Create directory name buffer */
  char dirName[256];

  /* Search through all directory entries for one that starts with
     woodjack.rooms. */
	while (de = readdir(dr)) {
    /* woodjack.rooms. is 15 characters long, with 1 null terminating char */
    char buffer[16];
    memcpy(buffer, de->d_name, 15);

    if (strcmp(buffer, "woodjack.rooms.") == 0) {
      memcpy(dirName, de->d_name, 256); /* Found it! */
    }

  }
	closedir(dr);
  /* Close current directory (.), and open a new directory with the new name. */
  dr = opendir(dirName);

  /* Read each file in this directory, and create a room object */
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
  /* Search room array for the starting room */
  int i = 0;
  while (strcmp(rooms[i]->roomType, "START_ROOM") != 0 && i < 7) {
    i++;
  }
  return rooms[i];
}

/* This function iterates through all of the available connections, and prints
   their names to stdout */
void printConnections(Room* r) {
  int j = 0;
  printf("POSSIBLE CONNECTIONS: ");
  while (r->outgoing[j] != NULL && j < 6) { /* Only 6 connections max */
    if (r->outgoing[j + 1] != NULL && j + 1 < 6)
      printf("%s, ",r->outgoing[j]);
    else
      printf("%s.\n",r->outgoing[j]);
    j++;
  }
}

char* getCommand() {
  /* create buffer for reading from stdin */
  char* line = malloc(sizeof(char) * 256);

  /* Initialize buffer with null terminators */
  memset(line, '\0', 256);


  if (fgets(line, 255, stdin)) {
    /* Delete newline chars */
    line[strcspn(line, "\n")] = 0;
  }

  /* Print a newline for formatting */
  printf("\n");

  return line;
}

/* This function converts the characters in the history array to integers, then
   retrieves the name of each room from the rooms array. */
void printPath(char* history) {
  int i = 0;
  printf("HISTORY: ");
  while (history[i] != '\0') {
    if (history[i + 1] != '\0')
      printf("%s, ", rooms[history[i] - 48]->name);
    else
      printf("%s.\n", rooms[history[i] - 48]->name);
    i++;
  }
}

void* saveTime() {
  /* Get time */
  time_t rawTime = time(NULL);
  struct tm* t = localtime(&rawTime);

  /* Prep buffer for write operation */
  char buffer[40];
  memset(&buffer, '\0', 40);
  strftime(buffer, 40, "%l:%M %p, %A, %B %d, %Y", t);

  /* Open file, write date string (32 characters in length) and close file */
  FILE* fd = fopen("currentTime.txt", "w");
  fwrite(buffer, 1, 32, fd);
  fclose(fd);
}

void printTime() {
  /* Prep buffer for read operation */
  char buffer[40];
  memset(&buffer, '\0', 40);

  /* Read file, and replace newline characters will null terminators. */
  FILE* fd = fopen("currentTime.txt", "r");
  fgets(buffer, 256, fd);
  buffer[strcspn(buffer, "\n")] = 0;

  /* Format and print time. */
  printf("%s\n\n", buffer);

  fclose(fd);
}

int main() {
  /* Load rooms into memory */
  loadRooms();

  /* Find start room */
  Room* r = findStart();

  /* Time file mutex */
  pthread_mutex_t t = PTHREAD_MUTEX_INITIALIZER;

  /* Initialize steps and history */
  /* History is saved by storing a room's index in this character array. Yes, I
     could have just used an integer array, but this made sense to me for some
     reason while I was working on this late at night. */
  char* history = malloc(sizeof(char) * 1024);
  memset(history, '\0', 1024);
  int h = 0;

  /* Step Counter */
  int steps = 0;

  /* Start game loop */
  while (1) {
    /* Add current room to history. */
    int k = 0;
    while (strcmp(rooms[k]->name, r->name) != 0) k++;
    history[h] = k + 48;
    h++;

    /* If this isn't the end room... */
    if (strcmp(r->roomType, "END_ROOM") != 0) {
        /* Print the name of the room */
        printf("CURRENT LOCATION: %s\n", r->name);

        /* Print possible connections */
        printConnections(r);

        /* Ask "WHERE TO? >" */
        printf("WHERE TO? >");

        /* Parse user input */
        char* cmd = getCommand();

        /* If invalid, print "HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN." */

        /* Check for time command */
        if (strcmp(cmd, "time") == 0) {
          /* This use of threading is specified by the assignment. I don't
             understand why threads were used here, other than for the sake of
             practice alone. It appears to yield no performace gain since the
             main thread is blocked until tThread completes. */

          /* Create thread */
          pthread_t tThread;
          int result = pthread_create(&tThread, NULL, saveTime, NULL);
          assert(result == 0);

          /* Block main thread until tThread resolves */
          result = pthread_join(tThread, NULL);

          /* Read the time from the newly-created file */
          printTime();

          h--; /* This command should not make an entry in the user's history */
        } else {
          /* Search through connections for possible room */
          int found = 0;
          int j = 0;
          while (r->outgoing[j] != NULL) {
            if (strcmp(cmd, r->outgoing[j]) == 0) {
              found = 1;
              break;
            }
            j++;
          }
          if (!found) {
            printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
          } else {
            /* Find the corresponding room in the rooms array */
            j = 0;
            while (strcmp(rooms[j]->name, cmd) != 0) j++;
            r = rooms[j];

            /* Increment step count. */
            steps++;

          }
        }

        /* Free memory used to save command */
        free(cmd);

    } else {
      /* If this is the end room... */
        /* Print the name of the room, and indicate this is the end room. */
        printf("CURRENT LOCATION: %s\n", r->name);
        printf("This is the end room.\n");

        /* Print the number of steps, and the path */
        printf("STEPS: %d\n", steps);
        printPath(history);

        /* Print "Congratulations!" */
        printf("Congratulations!\n");

        /* Free memory associated with rooms */
        // free(history);
        // int i;
        // for (i = 0; i < 7; i++) {
        //   free(rooms[i]->name);
        //   free(rooms[i]->roomType);
        //   int j = 0;
        //   while(rooms[i]->outgoing[j] != NULL) {
        //     free(rooms[i]->outgoing[j]);
        //     printf("1");
        //     j++;
        //   }
        //   free(rooms[i]);
        // }

        /* Exit the application */
        pthread_mutex_destroy(&t);
        break;
    }
  }

  return 0;
}
