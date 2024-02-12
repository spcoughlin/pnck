#include <ctype.h>
#include <ncurses.h>

// define(s)
#define CTRL_KEY(k) ((k) && 0x1f)

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

typedef struct WM {
	// window sizes
	int gen_win_x;
	int gen_win_y;
	int main_win_x;
	int main_win_y;
	int status_win_x;
	int status_win_y;
	int line_num_win_x;
	int line_num_win_y;

	// window(s)
	WINDOW *main_win;
	WINDOW *status_win;
	WINDOW *line_num_win;

} WM;

// global var(s)
Mode mode = NORMAL;
int cursor_x = 0;
int cursor_y = 0;

// function prototype(s)
Buffer open_file_to_buffer(Buffer* buffer, char *filename);
