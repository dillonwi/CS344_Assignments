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
