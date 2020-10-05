#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>

#define DX 2
#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b

struct Line {
  char* vals;
  int len;
};

struct Buffer {
  struct Line* vals;
  int len;
};

struct Line read_line(FILE *f_in) {
  int buf_len = 0, curr_len = 0;
  struct Line line;
  line.vals = NULL;
  
  char c = getc(f_in);
  if (c == EOF) {
    line.len = 0;
    return line;
  }
  while (c != EOF && c != '\n') {
    if (curr_len == buf_len) {
      buf_len = 2 * buf_len + 1;
      line.vals = (char *)realloc(line.vals, buf_len * sizeof(char));
    }
    line.vals[curr_len++] = c;
    c = getc(f_in);
  }
  if (curr_len == buf_len) {
    line.vals = (char *)realloc(line.vals, (buf_len + 1) * sizeof(char));
  }
  line.vals[curr_len] = '\0';
  line.len = curr_len;
  return line;
  
}

struct Buffer read_file(FILE *f_in) {
  int buf_len = 0, curr_len = 0;
  struct Buffer buffer;
  buffer.vals = NULL;
  
  struct Line line = read_line(f_in);
  while(line.vals != NULL) {
    if (curr_len == buf_len) {
      buf_len = 2 * buf_len + 1;
      buffer.vals = (struct Line *)realloc(buffer.vals, buf_len * sizeof(struct Line));
    }
    buffer.vals[curr_len++] = line;
    line = read_line(f_in);
  }
  
  buffer.len = curr_len;
  return buffer;
}

int main(int argc, char* argv[]) {
  FILE *f_in;
  f_in = fopen(argv[1], "r");
  struct Buffer buffer = read_file(f_in);
  
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  cbreak();
  
  printw("Program name: %s, length: %d lines", argv[1], buffer.len);
  refresh();
  
  WINDOW *win;
  int win_width = COLS - 2 * DX;
  int win_height = LINES - 2 * DX;
  win = newwin(win_height, win_width, DX, DX); 
  keypad(win, TRUE);
  scrollok (win, TRUE);
  
  int start_win = 0;
  int left_ind = 0;
  while(1) {
    werase(win);
    for (int i = start_win; i < buffer.len && i - start_win < win_height; i++) {
      int curr_len = min(left_ind, buffer.vals[i].len);
      mvwprintw(win, i - start_win + 1, 1, "%d:", (i + 1));
      waddnstr(win, buffer.vals[i].vals + curr_len, -1);
    }
    box(win, 0, 0);
    wrefresh(win);
    
    int c;
    switch(c = wgetch(win)) {
    case 27:
      endwin();
      for (int i = 0; i < buffer.len; i++) {
        free(buffer.vals[i].vals);
      }
      free(buffer.vals);
      return 0;
      break;
    case KEY_UP:
      if (start_win > 0) {
        start_win--;
      }
      break;
    case KEY_DOWN:
      start_win++;
      break;
    case KEY_RIGHT:
      left_ind++;
      break;
    case KEY_LEFT:
      if (left_ind > 0) {
        left_ind--;
      }
      break;
    }
  }
}
