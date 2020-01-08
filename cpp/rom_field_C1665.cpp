#include "arch.h"
#include "fp_C1665.h"

namespace C1665 {

/* Curve C1665 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B168_29;

// Base Bits= 29
const BIG Modulus= {0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFF};
const BIG R2modp= {0x190000,0x0,0x0,0x0,0x0,0x0};
const BIG ROI= {0x1FFFFFFA,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFF};
const chunk MConst= 0x5;
#endif

#if CHUNK==64

using namespace B168_60;

// Base Bits= 60
const BIG Modulus= {0xFFFFFFFFFFFFFFBL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFL};
const BIG R2modp= {0x190000000L,0x0L,0x0L};
const BIG ROI= {0xFFFFFFFFFFFFFFAL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFL};
const chunk MConst= 0x5L;
#endif

}
