#include <stdint.h>
#include "../../binary_utils/bitmanipulation.h"
#include "../state.c"
#include "instruction.h"

INSTR_TYPE findType(INSTRUCTION instr) {
  // read first 2 bits. 01 - single data transfer
  // 10 - branch.
  //case 00: if 001 - data processing
  // if 000 we need to read bits 4 - 7 as well, i.e top half of 
  // last byte? If bits 4 - 7 are 1001 then multiply instruction
  // if not 1001, it is either data processing or halt
  // get second nibble, shift right. then cases are 0000 (multiply, halt or dataprocessing), 0001 (dataprocessing), 0010 (single data transfer)
  // 0011 (single data transfer), 0100 (nothing), 0101 (branch), 0111 (nothing)
  if (instr == 0) {
    return HALT;
  }

  if (getBit(instr, 27)) {
    return BRANCH;
  } else if (getBit(instr, 26)) {
      return TRANSFER;
  } else if (getBit(instr, 25)) {
      return PROCESSING;
  } else { // 0
    if (getNibble(instr, 1) == 9) {
      return MULTIPLY;
    } else {
      return PROCESSING; 
    }
  }
  //should be unreachable
  return HALT;
}

// This function returns a word from memory (in big endian)
// from the starting address in the register  
WORD getWord(REGISTER *pc, MACHINE_STATE state) {
  int32_t sum = 0;
  for (int i = 0; i < 4; i++) {
    sum += state.memory[*pc + i] << 8 * i;
  }
  return sum;
}

// Stores the given word in at the address given in big endian
void storeWord(WORD word, ADDRESS address, MACHINE_STATE state) {
  for (int i = 0; i < 4; i++) {
    state.memory[address + i] = getByte(word, i);
  }
}

int willExecute(CONDITION cond) {
  int cpsr = getNibble(state.registers[16], 7);
  int N = getBit(cpsr, 31);
  int Z = getBit(cpsr, 30);
  int C = getBit(cpsr, 29);
  int V = getBit(cpsr, 28);

  switch (cond) {
    case EQ:
      return Z;
    case NE:
      return !Z;
    case GE:
      return N == V;
    case LT:
      return N != V;
    case GT:
      return !Z && N == V;
    case LE:
      return Z || N != V;
    case AL:
      return 1;
    default:
      return 0;
  }
}
