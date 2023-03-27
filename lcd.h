/*
    Header file for the library used to control the NHD-0420D3Z-NSW-BBW-V3
    display using the I2C protocol.
*/
#ifndef LCD_H
#define LCD_H

struct LCD {
    unsigned char address;
    unsigned char command[2];
    unsigned char row;
    unsigned char col;
};

void lcd_init(struct LCD *);

unsigned char send_command(struct LCD *, unsigned char);

unsigned char send_param_command(struct LCD *, unsigned char, unsigned char *, unsigned char);

void display_on(struct LCD *);

void display_off(struct LCD *);

void set_cursor(struct LCD *, unsigned char, unsigned char);

void home_cursor(struct LCD *);

void underline_cursor_on(struct LCD *);

void underline_cursor_off(struct LCD *);

void move_cursor_left(struct LCD *);

void move_cursor_right(struct LCD *);

void turn_on_blinking_cursor(struct LCD *);

void turn_off_blinking_cursor(struct LCD *);

void back_space(struct LCD *);

void clear_screen(struct LCD *);

void set_contrast(struct LCD *, unsigned char);

void set_backlight_brightness(struct LCD *, unsigned char);

void load_custom_character(struct LCD *, unsigned char, unsigned char *);

void shift_display_left(struct LCD *);

void shift_display_right(struct LCD *);

void display_firmware_version(struct LCD *);

void display_i2c_address(struct LCD *);

void print_character(struct LCD *, unsigned char);

void print_string(struct LCD *lcd, unsigned char *);

#endif