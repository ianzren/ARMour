#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include <stdbool.h>
#include <time.h>
#include <assert.h>

char *RED = "\033[1;31m";
char *YELLOW = "\033[1;33m";
char *BLUE = "\033\[0;34m";
char *RESET = "\033\033[0m";
char *GREEN = "\033\[0;32m";
char *CYAN = "\033\[0;36m";
char *MAGENTA = "\033\[0;35m";

void welcome(void) {
  system("clear");
  printf("============WELCOME TO ARMOUR!===============\n");
  printf("Type \"help\" if you don't know what to do :)\n");
}

void getInput(char *input, int MAX_LINE_LENGTH) {
  fgets(input, MAX_LINE_LENGTH, stdin);
  input[strlen(input) - 1] = '\0';
}

Command getCommand(char *str) {
  if (SAME(str, "help") || SAME(str, "h"))
	return HELP;
  if (SAME(str, "quit") || SAME(str, "q"))
	return QUIT;
  if (SAME(str, "about") || SAME(str, "a"))
	return ABOUT;
  if (SAME(str, "info") || SAME(str, "i"))
	return INFO;
  if (SAME(str, "write") || SAME(str, "w"))
	return WRITE;
  if (SAME(str, "clear") || SAME(str, "cl"))
	return CLEAR;
  if (SAME(str, "display") || SAME(str, "d"))
	return DISPLAY;
  if (SAME(str, "next") || SAME(str, "n"))
	return NEXT;
  if (SAME(str, "run") || SAME(str, "r"))
	return RUN;
  if (SAME(str, "finish") || SAME(str, "f"))
	return FINISH;
  if (SAME(str, "state") || SAME(str, "st"))
	return STATE;
  if (SAME(str, "load") || SAME(str, "l"))
	return LOAD;
  if (SAME(str, "stop") || SAME(str, "X"))
	return STOP;
  if (SAME(str, "export") || SAME(str, "ex"))
	return EXPORT;
  if (SAME(str, "delete") || SAME(str, "del"))
	return DELETE;
  if (SAME(str, "continue") || SAME(str, "c"))
	return CONTINUE;
  if (SAME(str, "break") || SAME(str, "b"))
	return BREAK;
  if (SAME(str, "disable") || SAME(str, "da"))
	return DISABLE;
  if (SAME(str, "insert") || SAME(str, "ins"))
	return INSERT;
  if (SAME(str, "append") || SAME(str, "ap"))
	return APPEND;
  return NONE;
}

void help(Editor *state) {
  printf("--COMMANDS--\n");
  printf("%s", CYAN);
  printf("| %s |", "help");
  printf("| %s |", "quit");
  printf("| %s |", "info");
  printf("| %s |", "about");
  printf("| %s |", "clear");
  printf("| %s |", "display");
  printf("| %s |", "write");
  printf("| %s |", "insert");
  printf("| %s |", "delete");
  printf("| %s |", "load");
  printf("| %s |", "export");
  printf("| %s |", "run");
  printf("\n");
  printf("========= Running mode commands: =========\n");
  printf("| %s |", "next");
  printf("| %s |", "finish");
  printf("| %s |", "state");
  printf("| %s |", "stop");
  printf("| %s |", "continue");
  printf("| %s |", "break");
  printf("| %s |", "disable");
  printf("\n");
  printf("%s", RESET);
  printf("Type \"info <command>\" to learn more about what it does.\n");
  printf("Type \"options\" after a command when unsure about which arguments to give.\n");
}

void quit(Editor *state) {
  printf("Hope you liked it...\n");
  state->running = false;
}

void about(Editor *state) {
  printf("ARMOUR is a tool for writing, executing and debugging small assembly programs using ARM11 architecture.\n");
  printf("Made as part of an extension to an emulator and assembler for Raspberry Pi.\n");
}

void print_line(char *instr) {
  if (!strstr(instr, ":"))
	printf("%s", CYAN); // opcode
  else
	printf("%s", MAGENTA);
  bool comment = false;
  for (; *instr; instr++) {
	switch (*instr) {
	  case '=':
	  case '#': printf("%s", GREEN); // immediate values
		break;
	  case '[':
	  case ']': printf("%s", RED); // brackets
		break;
	  case 'r':
	    if (*(instr + 1) <= '9' && *(instr + 1) >= '0')
		  printf("%s", YELLOW); // registers
		
		break;
	  case '/':
	    comment = true;
	    printf("%s", RED);
	    break;
	  case ',':
	  case ' ': printf("%s", RESET); // regular
		break;
	}
	if (comment) break;	
	printf("%c", *instr);
  }
  printf("%s%s\n", instr, RESET);
}

void print_lines(Editor *state, int start, int end, bool lineNumbers) {
  char *spaces;
  for (int i = start; i < end; i++) {
	if (state->isRunning) {
	  if (state->breakpoints[i])
		printf("%s", RED);
	}
	if (i == state->currentLine)
	  printf("=>");

	if (i + 1 >= 10) {
	  spaces = " ";
	} else {
	  spaces = "  ";
	}
	printf("%s", spaces);
	if (lineNumbers) {
	  printf("%d| ", i + 1);
	}
	print_line(state->lines[i]);
  }
}

void display(Editor *state) {
  if (state->noOfLines == 0) {
	printf("Nothing has been written yet. Type \"write\" to start editing.\n");
	return;
  }
  int start, end;
  switch (state->noOfTokens) {
	case 1: start = 0;
	  end = state->noOfLines;
	  break;
	case 2:
	  if (SAME(state->tokens[1], "options")) {
		printf(
			"display <start> <end> \n prints lines corresponding to line numbers between <start> and <end> inclusive."
			" If end is unspecified then only the start line is printed.\n");
		return;
	  } else {
		start = atoi(state->tokens[1]) - 1;
		end = start + 1;
	  }
	  break;
	case 3: start = atoi(state->tokens[1]) - 1;
	  end = (SAME(state->tokens[2], "end")) ? state->noOfLines : atoi(state->tokens[2]);
	  break;
	default: printf("Too many arguments\n");
	  return;
  }

  if (start < 0 || start + 1 > state->noOfLines || end < start || end > state->noOfLines) {
	printf("Invalid line number(s)\n");
	return;
  }
  bool lineNumbers = true;
  print_lines(state, start, end, lineNumbers);
}

char *trim(char *str) {
  int i;
  for (i = 0; str[i] != '/' && i < strlen(str); i++);
  char *str2 = calloc((i + 2), sizeof(char));
  CHECK_PRED(!str2, "Allocating memory failed\n");
  strncpy(str2, str, i);
  str2[i + 1] = '\0';
  return str2;
}

void write(Editor *state) {
  if (state->isRunning) {
	printf("You can't write while you're running!\n");
	return;
  }
  int start, end;
  if (state->noOfTokens > 1) {
	if (SAME(state->tokens[1], "options")) {
	  printf("write <START (= 1)> \nYou can start writing from line START.\nNOTE: writing on a line will overwrite "
			 "what was previous there.\nTyping \"write end\" will allow you to start typing from the final line.\n"
			 "Type exit to escape write mode.\nType back to backtrack to the previous line.\n");
	  return;
	} else if (SAME(state->tokens[1], "end")) {
	  start = state->noOfLines;
	} else {
	  start = atoi(state->tokens[1]);
	  start--;
	}
  } else {
	start = 0;
  }
  end = state->MAX_LINES;
  if (start < 0 || start > state->noOfLines + 1) {
	printf("Invalid start");
	return;
  }
  int i;
  char *instr = calloc(state->MAX_LINE_LENGTH, sizeof(char));
  for (i = start; i < end; i++) {
	printf("%3d| ", i + 1);
	getInput(instr, state->MAX_LINE_LENGTH);
	if (SAME(instr, "exit")) { break; }
	if (SAME(instr, "back")) {
	  printf("%sBacktracking to the previous line.%s\n", BLUE, RESET);
	  i = (i - 1 >= 0) ? i - 2 : i - 1;
	  continue;
	}
	if (!validInstr(trim(instr))) {
	  printf("%sFailed to write line due to syntax error, try again.%s\n", RED, RESET);
	  i--;
	  continue;
	}
	strcpy(state->lines[i], instr);
  }
  if (i >= state->noOfLines) {
	state->noOfLines = i;
  }
  free(instr);
  internal_save(state);
}

void finish(Editor *state) {
  if (!state->isRunning) {
	printf("%s", "You need to run first!\n");
	return;
  }
  for (int halt = 0; !halt;
	   halt = pipelineCycle(state->machineState, state->fetched, state->decoded, state->toDecode, state->toExecute));
  printf("End of program reached. Final machine state:\n");
  currentState(state);
  stop(state);
}

void runAll(Editor *state) {
  state->isRunning = 1;
  double start = clock();
  for (int halt = 0; !halt;
	   halt = pipelineCycle(state->machineState, state->fetched, state->decoded, state->toDecode, state->toExecute));
  double end = clock();
  printf("End of program reached in %f seconds.\nFinal machine state:\n", (end - start) / CLOCKS_PER_SEC);
  currentState(state);
  stop(state);
}

// Saves the current file, assembles this file.
// Need to save assembled file.
// Assemble takes in two arguments. The file
// to read from (which we should get from save)
// and the file to write to.
// Then we pass this file into emulate, which prints the machine state.
// PRE: assemble and emulate should be compiled
// into executables
// Need absolutePath to file
void run(Editor *state) {
  if (state->isRunning) {
	printf("You're already in run mode!\n");
	return;
  }
  if (state->noOfLines <= 0) {
	printf("You haven't written / loaded anything yet!\n");
	return;
  }
  printf("%sEntering run mode.\n%s", MAGENTA, RESET);
  state->currentLine = 0;
  char *assembledBinary = state->assembled;
  // a temporary file for assemble to write to. idk we might need a path to assembledBinary
  char *assembleCommand = "cd ../src && ./assemble ../extension/text.s ../extension/temp.bin";
  FILE *objCode = fopen(assembledBinary, "w");
  fclose(objCode);
  objCode = fopen(assembledBinary, "rb");

  int status = system(assembleCommand);

  status += 0;
  //just for debugging purposes so we can see if the system() call was sucessful.

  CHECK_PRED(!objCode, "File could not be opened.");

  for (int i = 0; fread(&state->machineState->memory[i], sizeof(BYTE), 1, objCode); i++);

  CHECK_PRED(ferror(objCode), "An error has occurred whilst file reading.");
  fclose(objCode);

  *(state->toDecode) = 0;
  *(state->toExecute) = 0;

  state->breakpoints = calloc(state->noOfLines, sizeof(bool));
  if (state->noOfTokens == 2) {
	if (SAME(state->tokens[1], "all")) {
	  runAll(state);
	  return;
	} else {
	  printf("Invalid argument, run can only be called with no arguments or the argument \"all\"\n");
	  return;
	}
  } else {
	printf("Possible commands:\n next\n state\n stop\n display\n finish \nType \"info <command>\" to learn more about"
		   " what the command does. \n");
	for (int i = 0; i < 2; i++) {
	  pipelineCycle(state->machineState, state->fetched, state->decoded, state->toDecode, state->toExecute);
	}
	state->isRunning = 1;
  }
}

void next(Editor *state) {
  int n;
  switch (state->noOfTokens) {
	case 1: n = 1;
	  break;
	case 2: n = atoi(state->tokens[1]);
	  break;
	default: printf("Too many arguments.\n");
	  return;
  }
  if (state->currentLine < 0) {
	printf("%s", "You need to run first!\n");
	return;
  }
  for (int i = 0; i < n; i++) {
	do {
	  pipelineCycle(state->machineState, state->fetched, state->decoded, state->toDecode, state->toExecute);
	} while (*(state->toExecute) != 1);
  }
  int i, instrCount = 0;
  for (i = 0; instrCount < (state->machineState->registers[15] / 4) - 2; i++) {
	if (!strstr(state->lines[i], ":")) {
	  instrCount++;
	}
  }
  state->currentLine = (strstr(state->lines[i], ":")) ? i + 1 : i;
  int startLine = state->currentLine - 3 >= 0 ? state->currentLine - 3 : 0;
  int endLine = state->currentLine + 3 < state->noOfLines ? state->currentLine + 3 : state->noOfLines;
  if (state->nextLocation)
    print_lines(state, startLine, endLine, true);

  if (state->currentLine >= state->noOfLines) {
	printf("End of program reached. Final machine state:\n");
	// todo: decide whether or not we want to delete these files here.
	// remove("temp");
	// remove(state->path);
	currentState(state);
	stop(state);
  }
}

/**
 * Saves the written lines of assembly into a file, omitting comments written.
 */
void internal_save(Editor *state) {
  if (state->isRunning) {
	printf("You can't save while you're running!\n");
	return;
  }
  FILE *outputFile = fopen(state->source, "w");
  if (!outputFile) {
	fprintf(stderr, "Error opening file.");
  }
  char *trimmed;
  for (int i = 0; i < state->noOfLines; i++) {
	trimmed = trim(state->lines[i]);
	fputs(trimmed, outputFile);
	if (fputs("\n", outputFile) <= 0)
	  fprintf(stderr, "fputs failure when writing to file.\n");
	free(trimmed);
  }
  fclose(outputFile);
}

void load(Editor *state) {
  if (state->isRunning) {
	printf("You have to enter run mode before setting breakpoints!\n");
	return;
  }
  FILE *fp;
  switch (state->noOfTokens) {
	case 1: printf("%s", "Load requires one argument\n");
	  break;
	case 2: fp = fopen(state->tokens[1], "r");
	  CHECK_PRED(!fp, "Error opening file.");
	  int i = 0;
	  while (!feof(fp)) {
		fgets(state->lines[i], state->MAX_LINE_LENGTH, fp);
		state->lines[i][strlen(state->lines[i]) - 1] = '\0';
		if (EMPTY_STRING(state->lines[i])) {
		  i--;
		}
		i++;
	  }
	  fclose(fp);
	  state->noOfLines = i;
	  //state->path = state->tokens[1];
	  break;
	default: printf("Too many arguments. Load requires you to specify only a path to the file to be loaded in.\n");
  }
  internal_save(state);
}

void currentState(Editor *state) {
  if (state->currentLine < 0) {
	printf("You need to run first!\n");
	return;
  }
  printState(state->machineState);
  printf("\n");
}

void stop(Editor *state) {
  if (!state->isRunning) {
	printf("You need to run first!\n");
	return;
  }
  memset(state->machineState->registers, 0, 17 * 4);
  printf("Program stopped, all registers have been reset.\n");
  resetState(state->machineState);
  state->currentLine = -1;
  state->isRunning = 0;
}

/**
 * Saves the written lines of assembly into a file, omitting comments written.
 */
void export(Editor *state) {
  if (state->isRunning) {
	printf("You can't export while you're running!\n");
	return;
  }
  switch (state->noOfTokens) {
	case 1:printf("%sExport requires at least one argument.\n%s", RED, RESET);
	  return;
	case 2:
	  if (SAME(state->tokens[1], "options")) {
		printf("%s", "-filepath\n");
		return;
	  } else {
		FILE *outputFile = fopen(state->tokens[1], "w");
		assert(outputFile);
		if (!outputFile)
		  fprintf(stderr, "Error opening file.\n");

		for (int i = 0; i < state->noOfLines; i++) {
		  if (fputs(state->lines[i], outputFile) <= 0)
			fprintf(stderr, "fputs failure when writing to file.\n");
		  if (fputs("\n", outputFile) <= 0)
			fprintf(stderr, "fputs failure when writing to file.\n");
		}
		fclose(outputFile);
	  }
	  break;
	default:printf("%sOnly requires 1 argument.\n%s", RED, RESET);
  }
}

void none(Editor *state) {
  printf("I cannot do that :(\n");
}

void clear(Editor *state) {
  system("clear");
  welcome();
}

void delete(Editor *state) {
  if (state->isRunning) {
	printf("You can't delete while running!\n");
	return;
  }
  int start, end;
  switch (state->noOfTokens) {
	case 1: start = 0;
	  end = state->noOfLines;
	  break;
	case 2:
	  if (SAME(state->tokens[1], "options")) {
		printf("%s", "delete <start = 0> <end = END>\n");
		return;
	  }
	  start = atoi(state->tokens[1]) - 1;
	  end = start + 1;
	  break;
	case 3: start = atoi(state->tokens[1]) - 1;
	  end = (SAME(state->tokens[2], "end")) ? state->noOfLines : atoi(state->tokens[2]);
	  break;
	default: printf("Too many arguments.\n");
	  return;
  }
  if (start + 1 > end || start < 0 || end < 0 || start + 1 > state->noOfLines || end > state->noOfLines) {
	printf("%sInvalid line numbers.%s\n", RED, RESET);
	return;
  }
  int diff = end - start;
  for (int i = end; i < state->noOfLines; i++) {
	strcpy(state->lines[i - diff], state->lines[i]);
  }
  state->noOfLines -= diff;
}

void shiftBack(int start, Editor *state) {
  for (int i = state->noOfLines; i > start; i--) {
	strcpy(state->lines[i], state->lines[i - 1]);
  }
}

void insert(Editor *state) {
  if (state->isRunning) {
	printf("You can't insert while you're running!\n");
	return;
  }
  int start, end;
  if (state->noOfTokens > 1) {
	if (SAME(state->tokens[1], "options")) {
	  printf(
		  "insert <START (= 1)> \nYou can start insert from line START.\nType exit to escape write mode.\nType back to backtrack to the previous line.\n");
	  return;
	} else {
	  start = atoi(state->tokens[1]);
	  start--;
	}
  } else {
	start = 0;
  }
  end = state->MAX_LINES;
  if (start < 0 || start > state->noOfLines + 1) {
	printf("Invalid start");
	return;
  }
  int i;
  char *instr = calloc(state->MAX_LINE_LENGTH, sizeof(char));
  for (i = start; i < end; i++) {
	printf("%3d| ", i + 1);
	getInput(instr, state->MAX_LINE_LENGTH);
	if (SAME(instr, "exit"))
	  break;

	if (SAME(instr, "back")) {
	  printf("%sBacktracking to the previous line.%s\n", BLUE, RESET);
	  i = (i - 1 >= 0) ? i - 2 : i - 1;
	  continue;
	}
	shiftBack(i, state);
	state->noOfLines++;
	strcpy(state->lines[i], instr);
  }
  free(instr);
  internal_save(state);
}

void continueBreak(Editor *state) {
  if (!state->isRunning) {
	printf("%s", "No program is currently running.\n");
	return;
  }
  state->nextLocation = false;
  switch (state->noOfTokens) {
	case 1:
	  do {
		next(state);
		if (!state->isRunning) {
		  break;
		}
	  } while (!state->breakpoints[state->currentLine]);
	  if (state->isRunning) {
		printf("%s!Hit breakpoint at line %d!%s\n", BLUE, state->currentLine + 1, RESET);
	  }
	  break;
	default: printf("%scontinue doesn't take any arguments%s\n", RED, RESET);
  }
  state->nextLocation = true;
}

void setBreak(Editor *state) {
  if (!state->isRunning) {
	printf("You have to enter run mode before setting breakpoints!\n");
	return;
  }
  int lineNumber;
  switch (state->noOfTokens) {
  case 2:
    if (SAME(state->tokens[1], "options")) {
      printf("break <line-number> ... allows you to set a breakpoint at the desired line(s).\n");
      return;
    }
  default:
    for (int i = 1; i < state->noOfTokens; i++) {
      lineNumber = atoi(state->tokens[i]) - 1;
      if (strstr(state->lines[lineNumber], ":")) {
	state->breakpoints[lineNumber + 1] = true;
      } else {
	state->breakpoints[lineNumber] = true;
      }
    }
  }
}

void disableBreak(Editor *state) {
  if (!state->isRunning) {
	printf("You have to enter run mode before disabling breakpoints!\n");
	return;
  }
  int lineNumber;
  switch (state->noOfTokens) {
  case 1: printf("Disable requires a line number.\n");
    return;
  case 2:
    if (SAME(state->tokens[1], "options")) {
      printf("disable <line number of breakpoint> ... \n");
      return;
    } else if (SAME(state->tokens[1], "all")) {
      for (int i = 0; i < state->noOfLines; i++)
	state->breakpoints[i] = false;
      return;
    }
  default:	  
    for (int i = 1; i < state->noOfTokens; i++) {
      lineNumber = atoi(state->tokens[i]) - 1;
      if (strstr(state->lines[lineNumber], ":")) {
	state->breakpoints[lineNumber + 1] = false;
      } else {
	state->breakpoints[lineNumber] = false;
      }
    }

  }
}


void append(Editor *state) {
  if (state->isRunning) {
	printf("You can't append while you're running!\n");
	return;
  }
  int start, end;
  if (state->noOfTokens > 1) {
	if (SAME(state->tokens[1], "options")) {
	  printf("append <START (= 1)> \nYou can start writing from line START.\nNOTE: writing on a line will overwrite "
			 "what was previous there.\nTyping \"write end\" will allow you to start typing from the final line.\n"
			 "Type exit to escape write mode.\nType back to backtrack to the previous line.\n");
	  return;
	} else if (SAME(state->tokens[1], "end")) {
	  start = state->noOfLines;
	} else {
	  start = atoi(state->tokens[1]);
	  start--;
	}
  } else {
	start = 0;
  }
  end = state->noOfLines;
  if (start < 0 || start > state->noOfLines + 1) {
	printf("Invalid start");
	return;
  }
  int i;
  char *text = calloc(state->MAX_LINE_LENGTH, sizeof(char));  
  for (i = start; i < end; i++) {
    printf("%3d| %s", i + 1, state->lines[i]);
	getInput(text, state->MAX_LINE_LENGTH);
	if (SAME(text, "exit"))
	  break;
	if (SAME(text, "back")) {
	  printf("%sBacktracking to the previous line.%s\n", BLUE, RESET);
	  i = (i - 1 >= 0) ? i - 2 : i - 1;
	  continue;
	}
	strcat(state->lines[i], text);
  }
  if (i >= state->noOfLines) {
	state->noOfLines = i;
  }
  free(text);
  internal_save(state);
}
