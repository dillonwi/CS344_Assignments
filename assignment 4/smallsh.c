/* Small Shell - Program 3 - By Jack Woods (woodjack@oregonstate.edu) */
#define BUF_SIZE 2048

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


/* Linear search for space character */
int findSpace(char* str) {
  int i;
  for (i = 0; i < strlen(str); i++) {
    if (str[i] == ' ') return i;
  }
  return -1;
}

/* Create an arguments array from a string */
void buildArgv(char** current, char* str, int n) {
    /* Parse command args */
  int pos = 0;
  while(pos = findSpace(&str[pos]) != -1) {
    int i;
    for(i = 0; i < n; i++) {
      current[i] = (char*) &str[pos + i];
    }
  }
}

void executeCommand(char* cmd) {
  /* Build char* const* array for command */
  /* Get command length */
  int cSize = findSpace(cmd) != -1 ? cSize : strlen(cmd);
  char c[cSize];
  int i;
  for (i = 0; i < cSize - 1; i++) {
    c[i] = cmd[i]; /* Populate char array with constant pointers */
  }
  c[cSize - 1] = NULL;
  printf("1%s1", c);

  /* Count the number of arguments */
  int n = 0;
  i = 0;
  for (i = 0; i < strlen(cmd); i++) {
    if ((int)cmd[i] == 32) {
      n++;
    }
  }
  n += 2; /* There is always 1 NULL argument that is appended, and 1 uncounted
             argument */

  char* argv[n];
  if (n > 2) printf ("WTF");
  if (n > 2)
    buildArgv(argv, cmd, n); /* Manipulates argv */
  else
    argv[0] = c;

  argv[n-1] = NULL; /* Last arg is always 0 */


  /* Execute command */
  if (execlp(c, argv[0], NULL) == -1) {
     printf("Error! %s\n", strerror(errno));
  }
}

int mngProc(char* cmd) {
  pid_t spawnpid = fork();
  int status = 0;
  switch (spawnpid) {
    case -1:
      perror("Hull Breach!");
      return -1;
      break;
    case 0:
      executeCommand(cmd);
      break;
    default:
      wait(&status);
      return status;
  }
}

int parseCommand(char* cmd, int status) {
  fflush(stdout);

  /* Check for comment first */
  if (cmd[1] == '#' || strlen(cmd) == 0) return 0;

  /* Check for built-in commands */
  if (strcmp(cmd, "exit\n") == 0) return -1;
  else if (strcmp(cmd, "status\n") == 0) {
    printf("%d\n", status);
  }
  else if(strncmp(cmd, "cd", 2) == 0) {
    chdir(&cmd[2]);
  } else {
    /* Call another process and manage it */
    return mngProc(cmd);
  }

  return 0;
}

int main() {
  /* Create command buffer */
  char* cmdBuffer;
  size_t bufSize = BUF_SIZE;
  cmdBuffer = (char *) malloc(BUF_SIZE * sizeof(char));

  /* Other useful variables */
  int status = 0;

  /* Main application loop */
  int quit = 0;
  while (!quit) {

    /* Prompt user for input */
    printf(": ");
    int bytesRead = getline(&cmdBuffer, &bufSize, stdin);

    /* Parse command */
    int parseResult = parseCommand(cmdBuffer, status);
    if (parseResult < 0) {
      printf("Exiting...\n");
      return parseResult;
    }
  }

  free(cmdBuffer);
  return 0;
}
