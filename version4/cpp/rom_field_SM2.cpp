#include "arch.h"
#include "fp_SM2.h"

namespace SM2 {

/* Curve SM2 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B256_28;

// Base Bits= 28
const BIG Modulus= {0xFFFFFFF,0xFFFFFFF,0xFF,0xFFFF000,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF};
const BIG R2modp= {0x40000,0x300000,0xE000000,0xFFFFFFF,0x2,0x30,0x100,0x1000,0x60000,0x0};
const chunk MConst= 0x1;
#endif

#if CHUNK==64

using namespace B256_56;

// Base Bits= 56
const BIG Modulus= {0xFFFFFFFFFFFFFFL,0xFFFF00000000FFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFEL};
const BIG R2modp= {0x3000000040000L,0xFFFFFFFE000000L,0x300000002L,0x10000000100L,0x60000L};
const chunk MConst= 0x1L;

#endif

}
