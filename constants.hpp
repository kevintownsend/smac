#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <math.h>

#define NBITS2(n) ((n&2)?1:0)
#define NBITS4(n) ((n&(0xC))?(2+NBITS2(n>>2)):(NBITS2(n)))
#define NBITS8(n) ((n&0xF0)?(4+NBITS4(n>>4)):(NBITS4(n)))
#define NBITS16(n) ((n&0xFF00)?(8+NBITS8(n>>8)):(NBITS8(n)))
#define NBITS32(n) ((n&0xFFFF0000)?(16+NBITS16(n>>16)):(NBITS16(n)))
#define NBITS(n) (n==0?0:NBITS32(n)+1)

#define SUB_WIDTH 8
#define SUB_HEIGHT 512
#define CONSTANT_DELTAS 32
#define SPM_CODE_ENCODE_BITS 9
#define NBITS_SPM_CODE_ENCODE_BITS NBITS(SPM_CODE_ENCODE_BITS)

#endif
