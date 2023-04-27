#include "i2c.h"
#include "lcd.h"
#include <stdlib.h>
#include <util/delay.h>

// Initializes the LCD struct passed in with the default 
void lcd_init(struct LCD *lcd) {
    lcd->address = 0x50;
    lcd->command[0] = 0xFE;
    lcd->command[1] = 0x00;
    lcd->row = 0;
    lcd->col = 0;
    clear_screen(lcd);
    home_cursor(lcd);
}

// Basic function to send a command to the LCD screen. Only for commands that
// don't send any data back to the master device and don't require additional
// parameters.
unsigned char send_command(struct LCD *lcd, unsigned char command) {
    lcd->command[1] = command;
    return i2c_io(lcd->address, lcd->command, 2, NULL, 0, NULL, 0);
}

// Basic function to send a command to the LCD screen. Includes commands that
// require additional parameters.
unsigned char send_param_command(struct LCD *lcd, unsigned char command, unsigned char *params,
    unsigned char params_length) {
    lcd->command[1] = command;
    return i2c_io(lcd->address, lcd->command, 2, params, params_length, NULL, 0);
}

// This command sets the I2C address. The address must be an even number (LSB = 0). The
// address change requires 20 microseconds to take effect; therefore, the subsequent input
// must have an appropriate delay. The default I2C address can be restored if SPI or RS-232
// is selected as the communication mode. Default is 0x50.
void change_i2c_address(struct LCD *lcd, unsigned char new_address) {
    unsigned char buf[1] = { new_address };
    i2c_io(lcd->address, buf, 1, NULL, 0, NULL, 0);
    lcd->address = new_address;
    _delay_us(3000);
}

// Turns on the LCD.
void display_on(struct LCD *lcd) {
    send_command(lcd, 0x41);
    _delay_us(100);
}

// Turns off the LCD.
void display_off(struct LCD *lcd) {
    send_command(lcd, 0x42);
    _delay_us(100);
}

// Sets the cursor to the desired row and column (zero-indexed). Valid rows
// are 0-3 and valid columns are 0-19.
void set_cursor(struct LCD *lcd, unsigned char row, unsigned char col) {
    unsigned char pos[1];
    unsigned char rows[] = { 0x00, 0x40, 0x14, 0x54};
    pos[0] = rows[row] + col;
    send_param_command(lcd, 0x45, pos, 1);
    lcd->row = row;
    lcd->col = col;
    _delay_us(100);
}

// Sets the cursor to the row 0, column 0.
void home_cursor(struct LCD *lcd) {
    send_command(lcd, 0x46);
    _delay_us(1500);
}

// Turns on the underline for the cursor.
void underline_cursor_on(struct LCD *lcd) {
    send_command(lcd, 0x47);
    _delay_us(1500);
}

// Turns off the underline for the cursor.
void underline_cursor_off(struct LCD *lcd) {
    send_command(lcd, 0x48);
    _delay_us(1500);
}

// Moves the cursor position left 1 space whether the cursor is turned on or
// not. The displayed character is not altered.
void move_cursor_left(struct LCD *lcd) {
    send_command(lcd, 0x49);
    _delay_us(100);
}

// Moves the cursor position right 1 space whether the cursor is turned on or
// not. The displayed character is not altered.
void move_cursor_right(struct LCD *lcd) {
    send_command(lcd, 0x4A);
    _delay_us(100);
}

// Turns on the blinking cursor.
void turn_on_blinking_cursor(struct LCD *lcd) {
    send_command(lcd, 0x4B);
    _delay_us(100);
}

// Turns off the blinking cursor.
void turn_off_blinking_cursor(struct LCD *lcd) {
    send_command(lcd, 0x4C);
    _delay_us(100);
}

// This command is destructive backspace. The cursor is moved back one space and the
// character on the cursor is deleted.
void back_space(struct LCD *lcd) {
    send_command(lcd, 0x4E);
    _delay_us(100);
}

// This command clears the entire display and place the cursor at line 1 column 1.
void clear_screen(struct LCD *lcd) {
    send_command(lcd, 0x51);
    _delay_us(1500);
}

// This command sets the display contrast. The contrast setting can be between 1 and 50,
// where 50 is the highest contrast. Default is 40.
void set_contrast(struct LCD *lcd, unsigned char contrast) {
    unsigned char params[1] = { contrast };
    send_param_command(lcd, 0x52, params, 1);
    _delay_us(500);
}

// This command sets the backlight brightness level, value between 1 and 8.
// Default is 8.
void set_backlight_brightness(struct LCD *lcd, unsigned char brightness) {
    unsigned char params[1] = { brightness };
    send_param_command(lcd, 0x53, params, 1);
    _delay_us(100);
}

/*
There is space for eight user-defined custom characters. This command loads the custom
character into one of the eight locations. The custom character pattern is bit mapped
into 8 data bytes. The bit map for Spanish character ‘¿’ is shown in table below.

Bit   | 7 6 5 4 3 2 1 0 Hex
---------------------------
Byte1 | 0 0 0 0 0 1 0 0 0x04
Byte2 | 0 0 0 0 0 0 0 0 0x00
Byte3 | 0 0 0 0 0 1 0 0 0x04
Byte4 | 0 0 0 0 1 0 0 0 0x08
Byte5 | 0 0 0 1 0 0 0 0 0x10
Byte6 | 0 0 0 1 0 0 0 1 0x11
Byte7 | 0 0 0 0 1 1 1 0 0x0E
Byte8 | 0 0 0 0 0 0 0 0 0x00

In order to display the custom character, user would first need to call ‘Set Cursor
Position’ command, then followed by the address of the custom character (0 to 8).
*/
void load_custom_character(struct LCD *lcd, unsigned char address, unsigned char *pattern) {
    unsigned char params[9];
    params[0] = address;
    unsigned char i;
    for(i = 0; i < 8; i++)
        params[i + 1] = pattern[i];
    send_param_command(lcd, 0x54, params, 9);
    _delay_us(200);
}

// This command shifts the display to the left 1 space. The cursor position also moves with
// the display, and the display data is not altered.
void shift_display_left(struct LCD *lcd) {
    send_command(lcd, 0x55);
    _delay_us(100);
}

// This command shifts the display to the right 1 space. The cursor position also moves with
// the display, and the display data is not altered.
void shift_display_right(struct LCD *lcd) {
    send_command(lcd, 0x56);
    _delay_us(100);
}

// This command displays the firmware version.
void display_firmware_version(struct LCD *lcd) {
    send_command(lcd, 0x70);
    _delay_us(4000);
}

// This command displays the current I2C slave address.
void display_i2c_address(struct LCD *lcd) {
    send_command(lcd, 0x72);
    _delay_us(4000);
}

// This command displays the given character.
void print_character(struct LCD *lcd, char character) {
    unsigned char buf[1] = { character };
    i2c_io(lcd->address, buf, 1, NULL, 0, NULL, 0);
    _delay_us(100);
    if(++lcd->col > 19) {
        lcd->col = 0;
        if(++lcd->row > 3)
            lcd->row = 0;
        set_cursor(lcd, lcd->row, lcd->col);
    }
}

void print_string(struct LCD *lcd, char *string) {
    unsigned char index = 0;
    while(string[index] != '\0') {
        print_character(lcd, string[index]);
        index++;
    }
}

void print_array(struct LCD *lcd, char *array, unsigned short length) {
    unsigned short i;
    for (i = 0; i < length; i++) {
        print_character(lcd, array[i]);
    }
}