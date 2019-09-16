#include "arch.h"
#include "fp_HIFIVE.h"

namespace HIFIVE {

/* Curve HIFIVE */


#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B336_29;

// Base Bits= 29
const BIG Modulus= {0x1FFFFFFD,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFF};
const BIG R2modp= {0x9000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const chunk MConst= 0x3;
#endif

#if CHUNK==64

using namespace B336_60;

// Base Bits= 60
const BIG Modulus= {0xFFFFFFFFFFFFFFDL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFL};
const BIG R2modp= {0x9000000000000L,0x0L,0x0L,0x0L,0x0L,0x0L};
const chunk MConst= 0x3L;
#endif

}