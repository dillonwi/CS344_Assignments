/* Small Shell - Program 3 - By Jack Woods (woodjack@oregonstate.edu) */
#define BUF_SIZE 2049

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>


/* Linear search for space character */
int findSpace(char* str, int pos) {
  int i;
  for (i = pos; i < strlen(str) - 1; i++) {
    if (str[i] == ' ') {
      return i;
    }
  }
  return -1;
}

/* Create an arguments array from a string */
void buildArgv(char** current, char* str) {
    /* Parse command args */
  int max = strlen(str);

  int arg = 0;
  int prevPos = 0;
  int pos = findSpace(str, 0);
  while(prevPos != -1) {
    int end = pos != -1 ? pos: max;
    if (prevPos != 0) prevPos++;
    char * s = (char *) malloc((end - prevPos) * sizeof(char));
    int i;
    for(i = 0; i < end - prevPos; i++) {
      s[i] = str[prevPos + i];
    }
    current[arg] = s;
    arg++;
    prevPos = pos;
    pos = findSpace(str, pos + 1);
  }
}

int commandLen(char* cmd) {

  /* Get command length */
  /* Count the number of arguments */
  int n = 0;
  int i = 0;
  for (i = 0; i < strlen(cmd); i++) {
    if ((int)cmd[i] == 32) {
      n++;
    }
  }
  n += 2; /* There is always 1 NULL argument that is appended, and 1 uncounted
             argument */
  return n;
}

int searchPipe(int c, char* cmd) {
  /* SearchPipe can be used to find either pipe */
  /* I could have made a struct with both boolean "pipe found" variables in
     it, but I found this easier to implement */
  char pipe = c == 0 ? '>' : '<';
  int i = 0;
  while (i < strlen(cmd) - 1) {
    if (cmd[i] == pipe) return i;
    i++;
  }
  return -1;
}

void setInOut(char* cmd, int reDirOut, int reDirIn) {
  int endCmd = INT_MAX;
  char* name;
  int targetFD1;
  int targetFD2;

  if (reDirIn != -1) {
    endCmd = reDirIn;
    reDirIn += 2;
    /* Save the name of the new input */
    int end = findSpace(cmd, reDirOut) != -1 ? findSpace(cmd, reDirOut) : strlen(cmd) - 1;
    name = (char*) malloc((end - reDirIn) * sizeof(char));
    memcpy(name, &cmd[reDirIn], (end - reDirIn));

    /* Create FD and re-assign input */
    targetFD1 = open(name, O_RDONLY);
    dup2(targetFD1, 0);
  }

  if (reDirOut != -1) {
    endCmd = reDirOut < endCmd ? reDirOut : endCmd;
    reDirOut += 2;
    /* Save the name of the new output */
    int end = findSpace(cmd, reDirOut) != -1 ? findSpace(cmd, reDirOut) : strlen(cmd) - 1;
    char* name = (char*) malloc((end - reDirOut) * sizeof(char));
    memcpy(name, &cmd[reDirOut], (end - reDirOut));

    /* Create FD and re-assign input */
    targetFD2 = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(targetFD2, 1);
  }
  endCmd = endCmd < INT_MAX ? endCmd - 1 : strlen(cmd) - 1;
  char* command = (char*) malloc(endCmd * sizeof(char));
  memcpy(command, cmd, endCmd);

  /* Execute command */
  /* Build char* const* array for command */
  int n = commandLen(command);
  char* argv[n];
  buildArgv(argv, command); /* Manipulates argv */
  argv[n-1] = NULL; /* Last arg is always 0 */

  printf("%d", n);

  /* Execute command */
  if (execvp(argv[0], argv) == -1) {
     printf("Error! %s\n", strerror(errno));
     exit(1);
  }
}

int mngProc(char* cmd) {
  fflush(stdout);
  int reDirOut = searchPipe(0, cmd);
  int reDirIn = searchPipe(1, cmd);
  pid_t spawnpid = fork();
  int status = 0;
  switch (spawnpid) {
    case -1:
      perror("Hull Breach!");
      return -1;
      break;
    case 0:
      setInOut(cmd, reDirOut, reDirIn);
      break;
    default:
      wait(&status);
      return status;
  }
}

int parseCommand(char* cmd, int status) {
  fflush(stdout);

  /* Check for comment first */
  if (cmd[0] == '#' || strlen(cmd) == 1) return 0;

  /* Check for built-in commands */
  if (strcmp(cmd, "exit\n") == 0) return -1;
  else if (strcmp(cmd, "status\n") == 0) {
    printf("%d\n", status);
  }
  else if(strncmp(cmd, "cd", 2) == 0) {
    char token = '\n';
    strtok(&cmd[3], &token);
    if (chdir(&cmd[3]) == -1) {
      printf("Error! %s\n", strerror(errno));
    }
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