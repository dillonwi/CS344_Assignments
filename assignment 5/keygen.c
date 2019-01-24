/* This program creates a key file of specified length. The characters in the
 * file generated will be any of the 27 allowed characters (A-Z and space),
 * generated using the standard UNIX randomization methods.
 * As per the assignment specifications, there are no spaces between every five
 * characters, as has been historically done. Also, no fancy random number
 * generation occurs; this assignment isn't about cryptographically secure
 * random number generation. The last character keygen outputs is a newline.
 * All error text is output to stderr, if any.
 */

 #include <stdlib.h>
 #include <stdio.h>
 #include <time.h>

/* Generates a key of length n */
char* generate(int n) {
  /* Allocate key string */
  char* key = (char *) malloc(sizeof(char) * n);

  /* Generate characters, one at a time */
  srand(time(NULL));

  int i;
  for (i = 0; i < n; i++) {
    int r = rand() % 27;

    /* Convert to ascii values */
    r += 65;

    /* If r equals 91, convert it to a spacebar character (32) */
    r = r == 91 ? 32 : r;

    /* Save the character */
    key[i] = (char) r;
  }


  return key;
}


int main(int argc, char* argv[]) {
  /* Ensure there are enough arguments */
  if (argc < 2) {
    perror("Error: Not enough arguments.");
    return 1;
  }

  /* Get the length of the key to generate */
  int length = atoi(argv[1]);

  /* Generate key */
  char* key = generate(length);

  /* Write key to stdout with terminating newline character*/
  printf("%s\n", key);

  /* Free key */
  free(key);

  return 0;
}
