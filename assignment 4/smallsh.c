/* Small Shell - Program 3 - By Jack Woods (woodjack@oregonstate.edu) */
#define BUF_SIZE 2048

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int parseCommand(char* cmd, int status) {
  /* Check for comment first */
  if (cmd[1] == '#') return 0;

  /* Check for built-in commands */
  if (strcmp(cmd, "exit") == 0) return -1;
  else if (strcmp(cmd, "status") == 0) {
    printf("%d", status);
    return 0;
  }
  else if(strcmp(cmd, "cd")) {

  } else {
    printf("Error: Command not found.")
    return 1;
  }

  return 0;
}

int main() {
  /* Create command buffer */
  char* cmdBuffer;
  size_t bufSize = BUF_SIZE;
  cmdBuffer = (char *) malloc(BUF_SIZE * sizeof(char));

  /* Create string that saves working directory */
  char* dirString = (char *) malloc(BUF_SIZE * sizeof(char));
  dirString[0] = '.';
  dirString[1] = '/';

  /* Other useful variables */
  int status = 0;
  long pid = getpid();

  /* Main application loop */
  int quit = 0;
  while (!quit) {

    /* Prompt user for input */
    printf(":");
    int bytesRead = getline(&cmdBuffer, &bufSize, stdin);

    /* Parse command */
    int parseResult = parseCommand(cmdBuffer, status);
    if (parseResult == -1) {
      printf("Exiting...\n");
      return 0;
    }
  }

  free(cmdBuffer);
  free(dirString);
  return 0;
}
