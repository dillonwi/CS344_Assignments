/* This program runs in the background as a daemon. Upon execution,
 * this outputs an error if it cannot be run due to a network error.
 * This program performs the actual encoding. It listens on a
 * particular port that's assigned at runtime. When a connection
 * is made, this generates a socket and spawns a child process.
 * After a verification handshake, this program receives plaintext
 * and converts it to a cipher. The ciphertext is returned to the
 * client, and the connection is closed.
 *
 * Sprry for the block of text. This is pretty much copied from the
 * assignment description verbatim. -Jack
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <math.h>

 void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

 // decrypts a cipher using the key
 void decryptCipher(char* cipher, char* key, int length) {
   int i;
   for (i = 0; i < length; i++) {
     int c = (int) cipher[i];
     int k = (int) key[i];

     // encrypt cipher
     c = c != 32 ? (c - 65) : 26;
     k = k != 32 ? (k - 65) : 26;

     c -= k;

     c = (27 + c) % 27;

     // Convert back to ascii
     c += 65;

     // If r equals 91, convert it to a spacebar character (32)
     c = c == 91 ? 32 : c;

     // Save the character
     cipher[i] = (char) c;

   }

 }

 int main(int argc, char *argv[])
 {
 	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
 	socklen_t sizeOfClientInfo;
 	char buffer[256];
 	struct sockaddr_in serverAddress, clientAddress;

 	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

 	// Set up the address struct for this process (the server)
 	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
 	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
 	serverAddress.sin_family = AF_INET; // Create a network-capable socket
 	serverAddress.sin_port = htons(portNumber); // Store the port number
 	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

 	// Set up the socket
 	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
 	if (listenSocketFD < 0) error("ERROR opening socket");

 	// Enable the socket to begin listening
 	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
 		error("ERROR on binding");
 	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

  // Always listen for new connections
  while(1) {

    // Accept a connection, blocking if one is not available until one connects
    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
    if (establishedConnectionFD < 0) error("ERROR on accept");

    // Fork process to decrypt message
    pid_t child = fork();
    if(child == -1) {
      perror("Hull Breach!");

    } else if (child == 0) {

      // Child must decrypt message and close the existing socket.
      // Get the verification from the client and parse it for information
      memset(buffer, '\0', 256);
      charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
      if (charsRead < 0) error("ERROR reading from socket");
      if (strncmp(buffer, "verifyd", 7) == 0) {
        // Verified, now get length

        // Get size of incoming message
        int length = atoi(&buffer[7]);

        // Send a Success message back to the client
        charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
        if (charsRead < 0) error("ERROR writing to socket");

        // Prepare buffers
        char cipher[length];
        char key[length];
        memset(cipher, '\0', length);
        memset(key, '\0', length);

        // Recieve plaintext
        charsRead = recv(establishedConnectionFD, cipher, length, 0); // Read the client's message from the socket
        if (charsRead < 0) error("ERROR reading from socket");

        // Send a Success message back to the client
        charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
        if (charsRead < 0) error("ERROR writing to socket");

        // Recieve key
        charsRead = recv(establishedConnectionFD, key, length, 0); // Read the client's message from the socket
        if (charsRead < 0) error("ERROR reading from socket");

        decryptCipher(cipher, key, length);

        // Send an encrypted message back to the client
        charsRead = send(establishedConnectionFD, cipher, length, 0); // Send success back


        if (charsRead < 0) error("ERROR writing to socket");
        close(establishedConnectionFD); // Close the existing socket which is connected to the client
        return 0;

      } else {
        // Unsuccessful verification
        perror("ERROR: otp_enc cannot use otp_dec_d");
        // Send a Failure message back to the client
        charsRead = send(establishedConnectionFD, "FAIL", 4, 0); // Send success back
        if (charsRead < 0) error("ERROR writing to socket");
        close(establishedConnectionFD); // Close the existing socket which is connected to the client
        return(1);
      }
    }

    // Parent continues to do adult things, like listening and
    // designating tasks.

  }

 	close(listenSocketFD); // Close the listening socket
 	return 0;
 }
