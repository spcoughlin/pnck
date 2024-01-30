typedef enum {
	NORMAL,
	COMMAND,
	INSERT,
	HIGHLIGHT
} Mode;

// global var(s)
Mode mode = NORMAL;

void openFile(char *fileName);
