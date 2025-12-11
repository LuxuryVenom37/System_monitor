#ifndef TERMOF_H
#define TERMOF_H

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

void clear_screen();
void move_cursor_to(int row, int column);
void set_color(char c, int color);
void draw_frame(int row, int col);
void get_screen_size(int *row, int *col);
void hide_cursor();
void show_cursor();

#endif
