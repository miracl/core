#include "arch.h"
#include "fp_SECP256K1.h"

namespace SECP256K1 {

/* Curve SECP256K1 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B256_28;

// Base Bits= 28
const BIG Modulus= {0xFFFFC2F,0xFFFFFEF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF};
const BIG R2modp= {0x0,0xA100000,0x2000E90,0x7A,0x1,0x0,0x0,0x0,0x0,0x0};
const chunk MConst= 0x2253531;

// Could set as Generalised Mersenne - but its slower!!
//const chunk MConst=977;
//const int BTset=32;

#endif

#if CHUNK==64

using namespace B256_56;

// Base Bits= 56
const BIG Modulus= {0xFFFFFEFFFFFC2FL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
const BIG R2modp= {0xA1000000000000L,0x7A2000E90L,0x1L,0x0L,0x0L};
const chunk MConst= 0x38091DD2253531L;

//const chunk MConst=977;
//const int BTset=32;


#endif

}
