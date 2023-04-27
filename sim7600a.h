/*
    Reads a response from the cellular module until it reads OK or ERROR in the stream.
    Puts the response into the buffer and returns the number of bytes read.
    The response will include an OK line or an ERROR line that could possibly have an error
    code that can be numeric or verbose, so assign enough space for the buffer.
*/
unsigned char response(unsigned char *, unsigned short, unsigned short *);

// Sends AT to the cellular module
// If the command buffer is not NULL, the command sent to the cellular module
// is AT+command_in_buffer, determined by the given length.
void sendAT(unsigned char *, unsigned char);