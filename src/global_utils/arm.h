#include "../emulator_utils/execute.h"

#ifndef ARM11_02_SRC_GLOBAL_UTILS_ARM_H_
#define ARM11_02_SRC_GLOBAL_UTILS_ARM_H_

int pipelineCycle(MACHINE_STATE *state, INSTRUCTION *fetched, DECODED_INSTR *decoded, int *toDecode, int *toExecute);
MACHINE_STATE *initialiseState();

#endif //ARM11_02_SRC_GLOBAL_UTILS_ARM_H_
