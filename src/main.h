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
	size_t length; // includes \n
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
Mode mode = NORMAL; // mode indicator
WM wm; // window manager, struct that holds all the windows
Buffer buffer; // currently open buffer
char *filename; // currently open file
int cursor_x = 0; // x cursor var for main window
int cursor_y = 0; // y cursor var for main window

// function prototype(s)
Buffer open_file_to_buffer(Buffer* buffer, char *file);
void delete_char_in_buffer(Buffer *buffer, int x, int y);
void insert_char_in_buffer(Buffer *buffer, char c, int x, int y);
void add_empty_row(Buffer *buffer);
void print_buffer_to_screen(WM *WM, Buffer *buffer);
void write_buffer_to_file(Buffer *buffer, char *file);
void print_status_bar(WM *WM, char *msg);
void print_line_numbers_for_buffer(WM *WM, Buffer *buffer);
void normal_mode_keypress_handler(WM *WM, Buffer *buffer, char key);
void insert_mode_keypress_handler(WM *WM, Buffer *buffer, char key);
