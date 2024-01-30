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

int main(int argc, char *argv[]) {
        initscr();
        if (argc == 2) {
                char *file = argv[1];
                // open and display file
		openFile(file);
        }
	mode = NORMAL;
	switch (mode) {
		case NORMAL:
			printw("Normal mode");
			break;
		case INSERT:
			printw("Insert mode");
			break;
		case COMMAND:
			printw("Command mode");
			break;
	}
        printw("\n Waiting for input to close");
	refresh();
	getch();
	endwin();

        return 0;
}

