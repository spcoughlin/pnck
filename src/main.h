#include <ctype.h>

typedef enum {
	NORMAL,
	COMMAND,
	INSERT,
	HIGHLIGHT
} Mode;

typedef struct {
	char *contents;
	size_t index;
	size_t length;
} Row;

typedef struct {
	Row *rows;
	size_t row_len;
	size_t num_rows;
} Buffer;

// global var(s)
Mode mode = NORMAL;

// function prototype(s)
Buffer open_file_to_buffer(Buffer* buffer, char *filename);
void print_buffer_to_screen(Buffer* buffer);
