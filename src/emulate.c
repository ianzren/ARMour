#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "emulator_utils/execute.h"
#include "global_utils/arm.h"

/**
 * emulate.c contains a main function which handles file reading,
 * machine state initialisation and our pipeline.
 */



/**
 * main ensures the given file can be read and
 * opened. We then allocate space in the heap for our machine state,
 * initialising the contents of our registers and memory to be 0.
 * We read the contents of the file into memory.
 * We then have a while loop handling the pipelining of our fetch,
 * decode and execute instruction cycle.
 * Finally, we free memory.
 *
 * @param argc Count of the arguments supplied
 * @param argv Array of strings passed as arguments
 * @return Integer representing exit success
 */
int main(int argc, char **argv) {
  CHECK_PRED(argc != 2, "Invalid number of arguments.");

  FILE *objCode = fopen(argv[1], "rb");
  CHECK_PRED(!objCode, "File could not be opened.");

  MACHINE_STATE *state = initialise_state();

  for (int i = 0; fread(&(state->memory[i]), sizeof(BYTE), 1, objCode) == 1; i++);

  CHECK_PRED(ferror(objCode), "An error has occurred whilst file reading.");
  fclose(objCode);

  REGISTER *pc = &state->registers[15];
  assert(pc);

  INSTRUCTION fetched;
  DECODED_INSTR decoded;
  int toDecode = 0, toExecute = 0, halt = 0;

  while (!halt)
	  halt = pipeline_cycle(state, &fetched, &decoded, &toDecode, &toExecute);
  
  print_state(state);

  free(state->registers);
  free(state->memory);
  free(state);

  return EXIT_SUCCESS;
}

