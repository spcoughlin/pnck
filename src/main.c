#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "main.h"

Buffer open_file_to_buffer(Buffer* buffer, char *file) {
	FILE *fp;
	fp = fopen(file, "r");

	int lines = 0;
	char ch;
	while((ch = fgetc(fp)) != EOF) {
	  if (ch == '\n') {
	    lines++;
	  }
	}

	// make buffer rows
	buffer->num_rows = lines;
	buffer->rows = malloc(sizeof(Row) * buffer->num_rows);
	if (!buffer->rows) {
		perror("malloc");
		fclose(fp);
		return *buffer;
	}
	
	// allocate mem for row content
	for (int i = 0; i < lines; i++) {
		buffer->rows[i].length = 140;
		buffer->rows[i].contents = malloc(sizeof(char) * buffer->rows[i].length);
		if (!buffer->rows[i].contents) {
			perror("malloc");
			fclose(fp);
			for (int j = 0; j < i; j++) {
				free(buffer->rows[j].contents);
			}
			free(buffer->rows);
			return *buffer;
		}
	}

	// fill row content strs with file chars	
	rewind(fp);
	int cur_line = 0, char_num = 0;
	while((ch = fgetc(fp)) != EOF && cur_line < lines) { 
		if (char_num < buffer->rows[cur_line].length - 1) {
			buffer->rows[cur_line].contents[char_num] = ch;
			char_num++;
			if (ch == '\n') {
				buffer->rows[cur_line].contents[char_num] = '\0';
				buffer->rows[cur_line].length = char_num;
				cur_line++;
				char_num = 0;
			}
		}
	}
	fclose(fp);
	return *buffer;
}

// delete char at x, y in buffer
void delete_char_in_buffer(Buffer *buffer, int x, int y) {
	for (int i = x; i < buffer->rows[y].length; i++) {
		buffer->rows[y].contents[i] = buffer->rows[y].contents[i + 1];
	}
	buffer->rows[y].contents[buffer->rows[y].length] = '\0';
	buffer->rows[y].length--;
}

// insert char at x, y in buffer, shift all chars to the right
void insert_char_in_buffer(Buffer *buffer, char c, int x, int y) {
	for (int i = buffer->rows[y].length + 1; i > x; i--) {
		buffer->rows[y].contents[i] = buffer->rows[y].contents[i - 1];
	}
	buffer->rows[y].contents[x] = c;
	buffer->rows[y].length++;
}

// delete row at y in buffer
void delete_row_in_buffer(Buffer *buffer, int y) {
	// move every row up
	for (int i = y; i < buffer->num_rows - 1; i++) {
		buffer->rows[i] = buffer->rows[i + 1];
	}

	// realloc for less rows
	buffer->num_rows--;
	buffer->rows = realloc(buffer->rows, sizeof(Row) * buffer->num_rows);
	if (!buffer->rows) {
		perror("realloc");
		exit(1);
	}

}

// add a row to the buffer at cursor_y with contents from cursor_x to end of line
void add_row(Buffer *buffer) {
	// realloc for new row
	buffer->num_rows++;
	buffer->rows = realloc(buffer->rows, sizeof(Row) * buffer->num_rows);
	if (!buffer->rows) {
		perror("realloc");
		exit(1);
	}
	// shift all rows down
	for (int i = buffer->num_rows - 1; i > cursor_y + 1; i--) {
		buffer->rows[i] = buffer->rows[i - 1];
	}

	// malloc for new row
	buffer->rows[cursor_y + 1].length = 1;
	buffer->rows[cursor_y + 1].contents = malloc(sizeof(char) * 140);
	if (!buffer->rows[cursor_y + 1].contents) {
		perror("malloc");
		exit(1);
	}
	buffer->rows[cursor_y + 1].contents[0] = '\n';
	buffer->rows[cursor_y + 1].contents[1] = '\0';

}

// write buffer to file
void write_buffer_to_file(Buffer *buffer, char *file) {
	FILE *fp;
	fp = fopen(file, "w");
	for (int i = 0; i < buffer->num_rows; i++) {
		fputs(buffer->rows[i].contents, fp);
	}
	fclose(fp);
}

// only call after initscr()
void print_buffer_to_screen(WM *WM, Buffer *buffer) {
	wclear(WM->main_win);
	for (int i = 0; i < buffer->num_rows; i++) {
		for (int j = 0; j < buffer->rows[i].length; j++) {
			if (buffer->rows[i].contents[j] == '\n') {
				wprintw(WM->main_win, "\n");
			} else if (buffer->rows[i].contents[j] != '\0') {
				wprintw(WM->main_win, "%c", buffer->rows[i].contents[j]);
			}
		}
	}
	wrefresh(WM->main_win);
}

void print_status_bar(WM *WM, char *msg) {
	wclear(WM->status_win);
	wprintw(WM->status_win, msg);
	wmove(WM->main_win, cursor_y, cursor_x);
	wrefresh(WM->status_win);
	wrefresh(WM->main_win);
}

void print_line_numbers_for_buffer(WM *WM, Buffer *buffer) {
	wclear(WM->line_num_win);
	for (int i = 1; i <= buffer->num_rows; i++) {
		wprintw(WM->line_num_win, "%d\n", i);
	}
	wrefresh(WM->line_num_win);
}

// keypress handlers for normal mode
void normal_mode_keypress_handler(WM *WM, Buffer *buffer, char key) {
	print_status_bar(WM, " -- NORMAL -- ");
	switch (key) {
		case 'h': // move left
			if (cursor_x > 0) {
				wmove(WM->main_win, cursor_y, --cursor_x);
				wrefresh(WM->main_win);
			}
			break;
		case 'j': // move down
			if (cursor_y < buffer->num_rows - 1) {
				wmove(WM->main_win, ++cursor_y, cursor_x);
				wrefresh(WM->main_win);
			}
			break;
		case 'k': // move up
			if (cursor_y > 0) {
				wmove(WM->main_win, --cursor_y, cursor_x);
				wrefresh(WM->main_win);
			}
			break;
		case 'l': // move right
			if (cursor_x < buffer->rows[cursor_y].length - 1 && cursor_x < buffer->rows[cursor_y].length) {
				wmove(WM->main_win, cursor_y, ++cursor_x);
				wrefresh(WM->main_win);
			}
			break;
		case 'i': // insert mode
			mode = INSERT;
			print_status_bar(WM, " -- INSERT -- ");
			break;
		case 'x': // delete char
			delete_char_in_buffer(buffer, cursor_x, cursor_y);	
			print_buffer_to_screen(WM, buffer);
			wmove(WM->main_win, cursor_y, cursor_x);
			wrefresh(WM->main_win);
			break;
		case 'w': // write buffer to file
			write_buffer_to_file(buffer, filename);
			break;
	}
}

// keypress handlers for insert mode
void insert_mode_keypress_handler(WM *WM, Buffer *buffer, char key) {
	print_status_bar(WM, " -- INSERT -- ");
	switch (key) {
		case 27:
			mode = NORMAL;
			print_status_bar(WM, " -- NORMAL -- ");
			break;
		case 127:
			if (buffer->rows[cursor_y].length <= 1) {
				delete_row_in_buffer(buffer, cursor_y);
			} else {
				delete_char_in_buffer(buffer, cursor_x - 1, cursor_y);
			}
			print_buffer_to_screen(WM, buffer);
			wmove(WM->main_win, cursor_y, --cursor_x);
			wrefresh(WM->main_win);
			break;
		case 10:
			add_row(buffer);
			print_buffer_to_screen(WM, buffer);
			print_line_numbers_for_buffer(WM, buffer);
			wrefresh(WM->main_win);
			wrefresh(WM->line_num_win);
			wmove(WM->main_win, ++cursor_y, 0);
			wrefresh(WM->main_win);
			break;
		default:
			insert_char_in_buffer(buffer, key, cursor_x, cursor_y);
			print_buffer_to_screen(WM, buffer);
			wmove(WM->main_win, cursor_y, ++cursor_x);
			wrefresh(WM->main_win);
			break;
	}
}


int main(int argc, char *argv[]) {
        initscr();
	noecho();
	raw();

	// initialize windows
	WM *WMptr = &wm;
	getmaxyx(stdscr, WMptr->gen_win_x, WMptr->gen_win_y);
	WMptr->main_win_x = WMptr->gen_win_x;
	WMptr->main_win_y = WMptr->gen_win_y;
	WMptr->main_win = newwin(WMptr->gen_win_x, WMptr->gen_win_y, 0, 5);
	WMptr->status_win_x = 1;
	WMptr->status_win_y = WMptr->gen_win_y;
	WMptr->status_win = newwin(WMptr->status_win_x, WMptr->status_win_y, WMptr->gen_win_x - 1, 0);
	WMptr->line_num_win_x = WMptr->gen_win_x;
	WMptr->line_num_win_y = 4;
	WMptr->line_num_win = newwin(WMptr->line_num_win_x, WMptr->line_num_win_y, 0, 0); 
	refresh();

	// initialize buffer
	Buffer *buffer_ptr = &buffer;
        if (argc == 2) {
                char *file = argv[1];
		open_file_to_buffer(buffer_ptr, file);
		filename = file;
        } else {
		open_file_to_buffer(buffer_ptr, "empty.txt");
		filename = "empty.txt";
	}
	print_buffer_to_screen(WMptr, buffer_ptr);
	print_line_numbers_for_buffer(WMptr, buffer_ptr);
	print_status_bar(WMptr, " -- NORMAL -- ");
	char ch;
	wmove(WMptr->main_win, 0, 0);
	while((ch = getch()) != 'q') {
		if (mode == NORMAL) {
			normal_mode_keypress_handler(WMptr, buffer_ptr, ch);
		} else if (mode == INSERT) {
			insert_mode_keypress_handler(WMptr, buffer_ptr, ch);
		}
	}
	endwin();
	return 0;
}

