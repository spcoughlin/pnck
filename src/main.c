#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "main.h"

Buffer open_file_to_buffer(char *file) {
	Buffer buffer;

	FILE *fp;
	fp = fopen(file, "r");

	int lines = 0;
	char ch;
	while(!feof(fp)) {
	  ch = fgetc(fp);
	  if (ch == '\n') {
	    lines++;
	  }
	}

	// make buffer rows
	buffer.num_rows = lines;
	buffer.rows = malloc(sizeof(Row) * buffer.num_rows);
	if (!buffer.rows) {
		perror("malloc");
		fclose(fp);
		return buffer;
	}
	
	// allocate mem for row content
	for (int i = 0; i < lines; i++) {
		buffer.rows[i].length = 140;
		buffer.rows[i].contents = malloc(sizeof(char) * buffer.rows[i].length);
		if (!buffer.rows[i].contents) {
			perror("malloc");
			fclose(fp);
			for (int j = 0; j < i; j++) {
				free(buffer.rows[j].contents);
			}
			free(buffer.rows);
			return buffer;
		}
	}

	// fill row content strs with file chars	
	rewind(fp);
	int cur_line = 0, char_num = 0;
	while((ch = fgetc(fp)) != EOF && cur_line < lines) { 
		if (char_num < buffer.rows[cur_line].length - 1) {
			buffer.rows[cur_line].contents[char_num++] = ch;
			if (ch == '\n') {
				buffer.rows[cur_line].contents[char_num] = '\0';
				cur_line++;
				char_num = 0;
			}
		}
	}
	fclose(fp);
	return buffer;
}

void print_buffer_to_screen(Buffer buffer) {
	for (int i = 0; i < buffer.num_rows; i++) {
		printw("%s", buffer.rows[i].contents);
	}
}

int main(int argc, char *argv[]) {
        initscr();
	Buffer buffer;
        if (argc == 2) {
                char *file = argv[1];
                // open and display file
		buffer = open_file_to_buffer(file);
        } else {
		buffer = open_file_to_buffer("empty.txt");
	}
	print_buffer_to_screen(buffer);
        printw("\nWaiting for input to close");
	refresh();
	getch();
	endwin();

        return 0;
}

