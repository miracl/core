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

package TWEEDLEDUM

// Base  bits= 56
var Modulus= [...]Chunk {0x4064E200000001,0xA1276C3F59B9A1,0x38A,0x0,0x40000000}
var R2modp= [...]Chunk {0xFC85D749BBE0CD,0x64ED6614FCA69B,0x9925C8F6AC3D6D,0x28E19094C65991,0xA0EDD7B}
var ROI= [...]Chunk {0xDE9F31B88837CE,0xE8F3F3EA99D653,0xEAAF8C2D57FF46,0x890EE2FCF624F2,0x2AE45117}
var SQRTm3= [...]Chunk {0xF0D7AB7DE6C70,0xAAE034F7256C96,0xD7220F84CB1286,0x942D06D6C8286D,0x15EF7D4A}
const MConst Chunk=0x4064E1FFFFFFFF

//*** rom curve parameters *****
const CURVE_Cof_I int= 1
var CURVE_Cof= [...]Chunk {0x1,0x0,0x0,0x0,0x0}
const CURVE_B_I int= 5
var CURVE_B= [...]Chunk {0x5,0x0,0x0,0x0,0x0}
var CURVE_Order= [...]Chunk {0x2CAFD400000001,0xA127696286C984,0x38A,0x0,0x40000000}
var CURVE_Gx= [...]Chunk {0x4064E200000000,0xA1276C3F59B9A1,0x38A,0x0,0x40000000}
var CURVE_Gy= [...]Chunk {0x2,0x0,0x0,0x0,0x0}
var CURVE_HTPC= [...]Chunk {0xD6755341381795,0xAEEBD03CFBC366,0x8B6CAABAA53E10,0x37020BF4EF14D8,0x3AB46CF3}

