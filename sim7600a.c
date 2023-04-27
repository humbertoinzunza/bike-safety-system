#include "sim7600a.h"
#include "serial.h"
#include <stdio.h>

/*
    Reads a response from the cellular module until it reads OK or ERROR in the stream.
    Puts the response into the buffer and returns the number of bytes read.
    The response will include an OK line or an ERROR line that could possibly have an error
    code that can be numeric or verbose, so assign enough space for the buffer.
*/
unsigned char response(char *buffer, unsigned short max_bytes_to_read, unsigned short *bytes_read) {
    *bytes_read = 0;
    unsigned char last_code[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char i;
    // Continue reading until the last code obtained is either OK or ERROR.
    // The array will look like: [x][x][O][K][<CR>] or [E][R][R][O][R], where x
    // is an irrelevant value that was possibly read before.
    // detected = 0, neither ERROR nor OK have been detected.
    // detected = 1, OK detected. detected = 2, ERROR detected.
    // detected = 3, both OK and ERROR detected (shouldn'y happen).
    unsigned char detected = 0;
    // Every time we detect a <LF> flip bit 1 (LSB). This will help us add only one <LF>, not the two given in each line.
    unsigned char lf_code = 0;

    while (!detected && max_bytes_to_read > 0)
    {
        // Read one byte and put it in the buffer
        *buffer = serial_in();
        // Shift the last_code array to the left
        for (i = 0; i < 4; i++) {
            last_code[i] = last_code[i + 1];
        }
        // Update the latest element in last_code to the last value put into the buffer
        last_code[4] = *buffer;

        // If last_code[4] == '\n' then cr_lf_code = cr_lf_code ^ 0000 0001
        lf_code ^= (last_code[4] == '\n');

        // Increment the buffer pointer
        buffer++;
        // Increment bytes_read
        *bytes_read++;
        // Decrement max_bytes_to_read
        max_bytes_to_read--;

        // If the current <LF> is the first of the line or a <CR> discard it
        if (last_code[4] == '\n' && (lf_code & 1) || last_code[4] = '\r') {
            buffer--;
            *bytes_read--;
            max_bytes_to_read++;
        }

        // Check the status of detected OK or ERROR
        detected = last_code[2] != 'O' == last_code[3] == 'K' && last_code[4] == '\r';
        detected |= ((last_code[0] == 'E' && last_code[1] == 'R' && last_code[2] == 'R' &&
            last_code[3] == 'O' && last_code[4] == 'R') << 1);
    }

    if(detected & 1) {
        // Dispose of last character (<LF>).
        serial_in();
        // Return 0 for OK
        return 0;
    }
    else if (detected & 2) {
        // Read until the last two bytes are <CR> and <LF>
        // Now the last two bytes of the array last_code will contain the last two bytes read
        while (last_code[3] != '\r' && last_code[4] != '\n' && max_bytes_to_read > 0) {
            // Place new character in the buffer
            *buffer = serial_in();
            // Shift the last two read characters
            last_code[3] = last_code[4];
            // Put the last character read in the position of the newest character in the array
            last_code[4] = *buffer();
            // Increment the buffer's pointer
            buffer++;
            // Increment the number of bytes read
            *bytes_read++;
            max_bytes_to_read--;
        }
        // At this point the loop ended by detecting the last <CR><LF> in the response. It is possible
        // that 'ERROR' is followed by some error code (numeric or verbose) or by nothing at all.
        // Either way, the response is finalized by <CR><LF> which we don't care about, so we must remove
        // from the buffer.
        *bytes_read -= 2;
        max_bytes_to_read += 2;

        // ERROR was detected
        return 1;
    }
}

// Sends AT to the cellular module
// If the command buffer is not NULL, the command sent to the cellular module
// is AT+command_in_buffer, determined by the given length.
void sendAT(unsigned char *command, unsigned char length) {
    // Send the AT part
    serial_out_byte('A');
    serial_out_byte('T');

    // If the command is not null continue sending until length is reached
    if (command != NULL) {
        serial_out_byte('+');
        serial_out_array(command, length);
    }

    // Send the termination character <CR>
    serial_out_byte('\r');
}

/*
    Changes the error mode in the device. Allowed values for 'new_mode' are:
    (only returns ERROR if an error occurs)
    0 disable +CME ERROR: <err> result code and use ERROR instead.

    (returns +CME ERROR: <error_code> when an error occurs)
    1 enable +CME ERROR: <err> result code and use numeric <err> values.

    (returns +CME ERROR: <verbose_error_description> when an error occurs)
    2 enable +CME ERROR: <err> result code and use verbose <err> values.
*/
void change_termination_error(unsigned char new_mode) {
    unsigned char command[6] = "CMEE=n";
    command[5] = new_mode + '0';
    sendAT(command, 6);
}

/*
    Returns the device's serial number. Allowed values for 'id_type' are:
    0 returns serial number
    1 returns the IMEI (International Mobile station Equipment Identity)
    2 returns the IMEISV (International Mobile station Equipment Identity and Software Version number)
    3 returns the SVN (Software Version Number)
*/
unsigned char product_serial_number(unsigned char *id, unsigned char id_type) {
    unsigned char command[6] = "CGSN=n";
    command[5] = new_mode + '0';
    sendAT(command, 6);
    unsigned char data_read[100];
    unsigned short bytes_read = 0;
    unsigned char error = response(&data_read, 100, &bytes_read);
}