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
			buffer->rows[cur_line].contents[char_num++] = ch;
			if (ch == '\n') {
				buffer->rows[cur_line].contents[char_num] = '\0';
				cur_line++;
				char_num = 0;
			}
		}
	}
	fclose(fp);
	return *buffer;
}
// add an empty row to the buffer at cursor_y
void add_empty_row(Buffer *buffer) {
	// realloc for new row
	buffer->num_rows++;
	buffer->rows = realloc(buffer->rows, sizeof(Row) * buffer->num_rows);
	if (!buffer->rows) {
		perror("realloc");
		exit(1);
	}
	// malloc for new row
	buffer->rows[cursor_y + 1].length = 140;
	buffer->rows[cursor_y + 1].contents = malloc(sizeof(char) * buffer->rows[cursor_y + 1].length);
	if (!buffer->rows[cursor_y + 1].contents) {
		perror("malloc");
		exit(1);
	}
	// shift everything down
	for (int i = buffer->num_rows - 1; i > cursor_y + 1; i--) {
		buffer->rows[i] = buffer->rows[i - 1];
	}

}

// only call after initscr()
void print_buffer_to_screen(Buffer *buffer) {
	clear();
	for (int i = 0; i < buffer->num_rows; i++) {
		printw("Buffer Row %i: %s", i, buffer->rows[i].contents);
	}
	refresh();
}

// keypress handlers for normal mode
void normal_mode_keypress_handler(Buffer *buffer, char key) {
	switch (key) {
		case 'h':
			if (cursor_x > 0)
				move(cursor_y, --cursor_x);
			break;
		case 'j':
			if (cursor_y < buffer->num_rows - 1)
				move(++cursor_y, cursor_x);
			break;
		case 'k':
			if (cursor_y > 0)
				move(--cursor_y, cursor_x);
			break;
		case 'l':
			if (cursor_x < buffer->rows[cursor_y].length - 1)
				move(cursor_y, ++cursor_x);
			break;
		case 'i':
			mode = INSERT;
			break;
		case 'x':
			buffer->rows[cursor_y].contents[cursor_x] = ' ';
			// shift everything to the left
			for (int i = cursor_x; i < buffer->rows[cursor_y].length - 1; i++) {
				buffer->rows[cursor_y].contents[i] = buffer->rows[cursor_y].contents[i + 1];
			}
			print_buffer_to_screen(buffer);
			break;
	}
}

// keypress handlers for insert mode
void insert_mode_keypress_handler(Buffer *buffer, char key) {
	switch (key) {
		case 27:
			mode = NORMAL;
			break;
		case 127:
			buffer->rows[cursor_y].contents[cursor_x] = ' ';
			move(cursor_y, cursor_x - 1);
			print_buffer_to_screen(buffer);
			break;
		case 13:
			add_empty_row(buffer);
			move(cursor_y + 1, 0);
			print_buffer_to_screen(buffer);
			break;

		default:
			buffer->rows[cursor_y].contents[cursor_x] = key;
			move(cursor_y, cursor_x + 1);
			print_buffer_to_screen(buffer);
			break;
	}
}


int main(int argc, char *argv[]) {
        initscr();
	noecho();
	raw();
	Buffer buffer;
	Buffer *buffer_ptr = &buffer;
        if (argc == 2) {
                char *file = argv[1];
		open_file_to_buffer(buffer_ptr, file);
        } else {
		open_file_to_buffer(buffer_ptr, "empty.txt");
	}
	print_buffer_to_screen(buffer_ptr);
	char ch;
	while((ch = getch()) != 'q') {
		if (mode == NORMAL) {
			normal_mode_keypress_handler(buffer_ptr, ch);
		} else if (mode == INSERT) {
			insert_mode_keypress_handler(buffer_ptr, ch);
		}
	}
	endwin();
	return 0;
}

