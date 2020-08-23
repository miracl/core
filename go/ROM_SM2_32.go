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

/* Fixed Data in ROM - Field and Curve parameters */

package SM2

// Base Bits= 28
var Modulus= [...]Chunk {0xFFFFFFF,0xFFFFFFF,0xFF,0xFFFF000,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF}
var ROI= [...]Chunk {0xFFFFFFE,0xFFFFFFF,0xFF,0xFFFF000,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF}
var R2modp= [...]Chunk {0x40000,0x300000,0xE000000,0xFFFFFFF,0x2,0x30,0x100,0x1000,0x60000,0x0}
const MConst Chunk=0x1

//*** rom curve parameters *****
// Base Bits= 28

const CURVE_Cof_I int= 1
var CURVE_Cof= [...]Chunk {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
const CURVE_B_I int= 0
var CURVE_B= [...]Chunk {0xD940E93,0xBCBD414,0xB8F92DD,0x89F515A,0x9A7F397,0x4BCF650,0x44D5A9E,0x9D9F5E3,0x8E9FA9E,0x2}
var CURVE_Order= [...]Chunk {0x9D54123,0xBBF4093,0x6052B53,0xDF6B21C,0xFFF7203,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF}
var CURVE_Gx= [...]Chunk {0x34C74C7,0x5A45893,0x60BE171,0xBBFF26,0x9948FE3,0x466A39C,0x95F9904,0x1F19811,0x2C4AE2C,0x3}
var CURVE_Gy= [...]Chunk {0x139F0A0,0xDF32E52,0xA474002,0x877CC62,0x153D0A9,0xE36B692,0xC59BDCE,0xF4F6779,0xC3736A2,0xB}
var CURVE_HTPC= [...]Chunk {0xFFFFFFF,0xFFFFFFF,0x55555FF,0xFFFF555,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xAAAAAA9,0xA}
