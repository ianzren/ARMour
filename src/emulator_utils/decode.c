#include "decode.h"
#include "../binary_utils/bitmanipulation.h"
#include "state.h"
#include "instruction.h"
#include <stdint.h>
#include <stdio.h>

//The function decode takes in a 32 bit int instruction as an arguement and uses it to set the relevant values in the DECODED_INSTR struct.

//The DECODED INSTR can then be passed into execute as a single argument.

DECODED_INSTR decode(INSTRUCTION instr, MACHINE_STATE state) {
  // Condition CPSR register
 // int32_t instr = getWord(pc);
 // state.registers[16] = getNibble(instr, 7);
 struct decodedInstr decoded;
 INSTR_TYPE type = findType(instr);
 decoded.type = type;
 decoded.condition = getNibble(instr, 7);
  switch (type) {
    case MULTIPLY:
      decoded.A = getBit(instr, 21);
      decoded.S = getBit(instr, 20); 
      decoded.rd = getNibble(instr, 4);
      decoded.rm = getNibble(instr, 0);
      decoded.rs = getNibble(instr, 2);
      decoded.rn = getNibble(instr, 3);
      break;
    case BRANCH:
      decoded.offset = clearBits(instr, 8, 24);
      break;
    case PROCESSING:
      decoded.opcode = getNibble(instr >> 1, 5);
      decoded.I = getBit(instr, 25);
      decoded.S = getBit(instr, 20);
      decoded.rn = getNibble(instr, 4);
      decoded.rd = getNibble(instr, 3);
      //Here we calculate the value of operand2
      if (decoded.I) {
        // rotate bits 0-7 by bits 8-11
        decoded.op2 = rotateRight(getByte(instr, 0), 2 * getNibble(instr, 2));
      } else {
          int shiftAmount;
          int shiftType = getBits(instr, 2, 5);
           
          decoded.rm = state.registers[getNibble(instr, 0)];

          if (getBit(instr, 4)) {
            // gets the last bit of the register specified.
//            shiftAmount = getBit(state.registers[getNibble(instr, 2)], 31);
              shiftAmount = getByte(state.registers[getNibble(instr, 2)], 0);
          } else {
            shiftAmount = getBits(instr, 5, 7);
          }
          uint32_t discarded;
          switch (shiftType) {
            case 0:
              //lsl
              discarded = (decoded.rm >> (32 - shiftAmount));
              decoded.shiftCarryOut = discarded % 2;
              decoded.op2 = decoded.rm << shiftAmount;
              break;
            case 1:
              //lsr
              discarded = decoded.rm << (32 - shiftAmount);
//              decoded.shiftCarryOut = discarded % 2;
                  decoded.shiftCarryOut = getBit(discarded, 31);
              decoded.op2 = decoded.rm >> shiftAmount;
              break;
            case 2:
              //asr
              discarded = decoded.rm << (32 - shiftAmount);
//              decoded.shiftCarryOut = discarded % 2;
                  decoded.shiftCarryOut = getBit(discarded, 31);
              decoded.op2 = arithmeticRight(decoded.rm, shiftAmount);
              break;
            case 3:
              //ror;
              discarded = decoded.rm << (32 - shiftAmount);
//              decoded.shiftCarryOut = discarded % 2;
                  decoded.shiftCarryOut = getBit(discarded, 31);
              decoded.op2 = rotateRight(decoded.rm, shiftAmount);
              break;
          }
      }
      break;
    case TRANSFER:
      decoded.I = getBit(instr, 25);
      decoded.P = getBit(instr, 24);
      decoded.U = getBit(instr, 23);
      decoded.L = getBit(instr, 20);
      decoded.rn = getNibble(instr, 4);
      decoded.rd = getNibble(instr, 3);
      if (decoded.I) {
          int shiftAmount;
          int shiftType = getBits(instr, 2, 5);
          decoded.rm = state.registers[getNibble(instr, 0)];
          if (getBit(instr, 4)) {
            shiftAmount = getBit(state.registers[getNibble(instr, 2)], 31);
          } else {
            shiftAmount = getBits(instr, 5, 7);
          }
          switch (shiftType) {
            case 0:
              decoded.offset = decoded.rm << shiftAmount;
              break;
            case 1:
              decoded.offset = decoded.rm >> shiftAmount;
              break;
            case 2:
              decoded.offset = arithmeticRight(decoded.rm, shiftAmount);
              break;
            case 3:
              decoded.offset = rotateRight(decoded.rm, shiftAmount);
              break;
          }
      } else {
          decoded.offset = getBits(instr, 12, 0);
      }
      break;
    default:
      break;
  }
  return decoded;
}
