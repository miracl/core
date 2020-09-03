/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "arch.h"
#include "ecp_TWEEDLEDUM.h"


namespace TWEEDLEDUM {
/* TWEEDLEDUM Curve */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B256_29;

//*** rom curve parameters *****
const int CURVE_Cof_I= 1;
const BIG CURVE_Cof= {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const int CURVE_B_I= 5;
const BIG CURVE_B= {0x5,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG CURVE_Order= {0x1,0x1657EA0,0x18A1B261,0x15424ED2,0x38,0x0,0x0,0x0,0x400000};
const BIG CURVE_Gx= {0x0,0xA032710,0xFD66E68,0x15424ED8,0x38,0x0,0x0,0x0,0x400000};
const BIG CURVE_Gy= {0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG CURVE_HTPC= {0x1381795,0x16B3AA9A,0xF3EF0D9,0x15DD7A0,0xBAA53E1,0xC45B655,0xFD3BC53,0x1E66E041,0x3AB46C};
#endif

#if CHUNK==64

using namespace B256_56;

//*** rom curve parameters *****
const int CURVE_Cof_I= 1;
const BIG CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
const int CURVE_B_I= 5;
const BIG CURVE_B= {0x5L,0x0L,0x0L,0x0L,0x0L};
const BIG CURVE_Order= {0x2CAFD400000001L,0xA127696286C984L,0x38AL,0x0L,0x40000000L};
const BIG CURVE_Gx= {0x4064E200000000L,0xA1276C3F59B9A1L,0x38AL,0x0L,0x40000000L};
const BIG CURVE_Gy= {0x2L,0x0L,0x0L,0x0L,0x0L};
const BIG CURVE_HTPC= {0xD6755341381795L,0xAEEBD03CFBC366L,0x8B6CAABAA53E10L,0x37020BF4EF14D8L,0x3AB46CF3L};
#endif

}
