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
#include "fp_TWEEDLEDEE.h"

namespace TWEEDLEDEE {

/* TWEEDLEDEE Curve */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B256_29;

// Base  bits= 29
const BIG Modulus= {0x1,0x1657EA0,0x18A1B261,0x15424ED2,0x38,0x0,0x0,0x0,0x400000};
const BIG R2modp= {0x3FF4,0xE8E1080,0xA9C2909,0xC128269,0x89F8301,0x19607541,0x1F4C0E6F,0x3443009,0x22B644};
const BIG ROI= {0x69D57EC,0x5E991A4,0x213EFC7,0x10FBA370,0x72930A2,0x5B22D4C,0x102FC76F,0xA21B807,0x113EFC};
const BIG SQRTm3= {0x91F49FA,0x17E6AD31,0x19F5AE40,0x8F54DB0,0x3FFCD01,0xB5FF973,0x12C43D6C,0xE878126,0x2D8CDA};
const chunk MConst= 0x1FFFFFFF;
#endif

#if CHUNK==64

using namespace B256_56;

// Base  bits= 56
const BIG Modulus= {0x2CAFD400000001L,0xA127696286C984L,0x38AL,0x0L,0x40000000L};
const BIG R2modp= {0x21CA8949BBE5DFL,0x717E3D9E719200L,0xB8A93694FCEE5L,0xE6FCB03AA0A27EL,0x2013F4C0L};
const BIG ROI= {0xBD3234869D57ECL,0x7DD1B8084FBF1CL,0x645A9872930A28L,0xDC03C0BF1DBCBL,0x113EFC51L};
const BIG SQRTm3= {0xFCD5A6291F49FAL,0x7AA6D867D6B902L,0xBFF2E63FFCD014L,0x3C0934B10F5B16L,0x2D8CDA74L};
const chunk MConst= 0x2CAFD3FFFFFFFFL;

#endif

}
