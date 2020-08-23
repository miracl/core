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

package SECP160R1

// Base Bits= 56
var Modulus= [...]Chunk {0xFFFFFF7FFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF}
var R2modp= [...]Chunk {0x1000000010000,0x400000,0x0}
var ROI= [...]Chunk {0xFFFFFF7FFFFFFE,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF}
const MConst Chunk=0xFFFFFF80000001

const CURVE_Cof_I int= 1
var CURVE_Cof= [...]Chunk {0x1,0x0,0x0}
const CURVE_B_I int= 0
var CURVE_B= [...]Chunk {0xD4D4ADC565FA45,0x7A8B65ACF89F81,0x1C97BEFC54BD}
var CURVE_Order= [...]Chunk {0x27AED3CA752257,0x1F4C8F9,0x1000000000000}
var CURVE_Gx= [...]Chunk {0xC38BB913CBFC82,0x73284664698968,0x4A96B5688EF5}
var CURVE_Gy= [...]Chunk {0x2351377AC5FB32,0x947D59DCC91204,0x23A628553168}
var CURVE_HTPC= [...]Chunk {0xDAFAF3A25BC0DA,0x6106FACE6A7305,0x1E50BD02CD5A}
