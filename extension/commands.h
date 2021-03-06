#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../src/global_utils/arm.h"

#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_PATH_LENGTH 100

#define CHECK_PRED(pred, msg) \
 do { \
if ((pred)) { \
fprintf(stderr, \
"%s\n", (msg)); exit(EXIT_FAILURE);} \
} while (0)

#define SAME(s1, s2) (strcmp(s1, s2) == 0)

#define PRINT(s1) do { printf("%s", s1) } while (0)

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"


typedef enum command {
  HELP,
  QUIT,
  ABOUT,
  INFO,
  CLEAR,
  WRITE,
  DISPLAY,
  RUN,
  FINISH,
  NEXT,
  STATE,
  STOP,
  LOAD,
  EXPORT,
  DELETE,
  INSERT,
  CONTINUE,
  BREAK,
  DISABLE,
  APPEND,
  NONE
} Command;

typedef struct Editor {
  bool running;
  char **tokens;
  int noOfTokens;
  int MAX_LINE_LENGTH;
  int MAX_LINES;
  char **lines;
  int noOfLines;
  int currentLine;
  char *source;
  char *assembled;
  MACHINE_STATE *machineState;
  INSTRUCTION *fetched;
  DECODED_INSTR *decoded;
  int *toDecode, *toExecute;
  int isRunning;
  bool *breakpoints;
  bool nextLocation;
  int CYCLES_LIMIT;
} Editor;

typedef void (*commands)(Editor *);

void welcome(void);
void get_input(char *input, int MAX_LINE_LENGTH);
Command get_command(char *str);
void help(Editor *state);
void quit(Editor *state);
void about(Editor *state);
void info(Editor *state);
void write(Editor *state);
void display(Editor *state);
void run(Editor *state);
void finish(Editor *state);
void next(Editor *state);
void internal_save(Editor *state);
void current_state(Editor *state);
void clear(Editor *state);
void stop(Editor *state);
void load(Editor *state);
void export(Editor *state);
void delete(Editor *state);
void insert(Editor *state);
void continue_break(Editor *state);
void set_break(Editor *state);
void disable_break(Editor *state);
void append(Editor *state);
void none(Editor *state);

#endif
