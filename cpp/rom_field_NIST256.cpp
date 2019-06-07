#include "arch.h"
#include "fp_NIST256.h"

namespace NIST256 {

/* Curve NIST256 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B256_28;

// Base Bits= 28
const BIG Modulus= {0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFF,0x0,0x0,0x1000000,0x0,0xFFFFFFF,0xF};
const BIG R2modp= {0x50000,0x300000,0x0,0x0,0xFFFFFFA,0xFFFFFBF,0xFFFFEFF,0xFFFAFFF,0x2FFFF,0x0};
const chunk MConst= 0x1;
#endif

#if CHUNK==64

using namespace B256_56;

// Base Bits= 56
const BIG Modulus= {0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFL,0x0L,0x1000000L,0xFFFFFFFFL};
const BIG R2modp= {0x3000000050000L,0x0L,0xFFFFFBFFFFFFFAL,0xFFFAFFFFFFFEFFL,0x2FFFFL};
const chunk MConst= 0x1L;

#endif

}
