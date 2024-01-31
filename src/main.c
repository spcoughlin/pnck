#include <stdio.h>
#include <ncurses.h>
#include "main.h"

void openFile(char *file) {
	FILE *fp;
	int c;
	fp = fopen(file, "r");
	while ((c = fgetc(fp)) != EOF) {
		printw("%c", c);
	}
	fclose(fp);
}

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
	Row rows[lines];
	buffer.rows = rows; 

	// make row content strs
	for (int i = 0; i < buffer.num_rows; i++) {
		buffer.rows[i].length = 140;
		char contents[buffer.rows[i].length];
		buffer.rows[i].contents = contents;
	}
	// fill row content strs with file chars	
	fp = fopen(file, "r");
	int cur_line = 0;
	int char_num = 0;
	char cur_char;
	while(!feof(fp)) { 
		cur_char = fgetc(fp);
		buffer.rows[cur_line].contents[char_num] = cur_char; 
		if (cur_char == '\n') {
			cur_line++;
			char_num = 0;
		} else {
			char_num++;
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
        printw("\n Waiting for input to close");
	refresh();
	getch();
	endwin();

        return 0;
}

