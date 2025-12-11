#include "termof.h"


void clear_screen(){
	printf("\033[2J\033[H");
}
void move_cursor_to(int row, int column){
	printf("\033[%d;%dH", row, column);
}
void set_color(char c, int color){
	if(c == 'f' && color > 0 && color < 9)	printf("\033[%dm", 29+color);
	if(c == 'b' && color > 0 && color < 9)	printf("\033[%dm", 39+color);
	if(c == 'l' && color > 0 && color < 9)	printf("\033[%dm", 89+color);
	if(c == 'u' && color > 0 && color < 9)	printf("\033[%dm", 99+color);
	if(c == 'r' && color == 0)				printf("\033[%dm", color);
}
void draw_frame(int row, int col){
	char nazov[] = "System monitor";
	char meno[] = "by Viktor Bohunický";
	int sz_nazov, sz_meno;
	int i, j;

	sz_nazov = strlen(nazov);
	sz_meno = strlen(meno);
	int first_row = 2;
	int first_col = 2;
	int last_row = row-1;
	int last_col = col-1;

	set_color('b', 8);

	for(i = first_row; i < last_row; i++){
		move_cursor_to(i, first_col);
		printf("  ");
		move_cursor_to(i, last_col);
		printf("  ");
	}
	for(j = first_col; j <= last_col; j++){
		move_cursor_to(first_row, j);
		printf("  ");
		move_cursor_to(last_row, j);
		printf("  ");
	}
	set_color('f', 1);

	move_cursor_to(first_row, ((last_col - first_col - sz_nazov)/2) + 1);
	printf("%s", nazov);

	move_cursor_to(last_row, ((last_col - first_col - sz_meno)/2) + 1);
	printf("%s", meno);

	move_cursor_to(last_row-1, 4);
	printf("Stlač Ctrl+C na ukončenie:");

	set_color('r', 0);
}
void get_screen_size(int *row, int *col){
	struct winsize terminal;

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);

	*row = terminal.ws_row;
	*col = terminal.ws_col;
}
void hide_cursor(){
	printf("\033[?25l");
}
void show_cursor(){
	printf("\033[?25h");
}
