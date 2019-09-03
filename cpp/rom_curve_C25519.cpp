#include "arch.h"
#include "ecp_C25519.h"

namespace C25519 {

/* Curve 25519 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B256_29;

const int CURVE_Cof_I=8;
const BIG CURVE_Cof= {0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const int CURVE_A=486662;
const BIG CURVE_Order= {0x1CF5D3ED,0x9318D2,0x1DE73596,0x1DF3BD45,0x14D,0x0,0x0,0x0,0x100000};
const BIG CURVE_Gx= {0x9};
#endif

#if CHUNK==64

using namespace B256_56;

const int CURVE_Cof_I=8;
const BIG CURVE_Cof= {0x8L,0x0L,0x0L,0x0L,0x0L};
const int CURVE_A=486662;
const BIG CURVE_Order= {0x12631A5CF5D3ED,0xF9DEA2F79CD658,0x14DE,0x0,0x10000000};
const BIG CURVE_Gx= {0x9};
#endif

}
