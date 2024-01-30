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
        printw("Hello World!");
        refresh();
        getch();
        endwin();

        return 0;
}

