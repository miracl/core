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

package SECP256K1

// Base Bits= 28

var Modulus = [...]Chunk{0xFFFFC2F, 0xFFFFFEF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xF}
var ROI = [...]Chunk{0xFFFFC2E, 0xFFFFFEF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xF}
var R2modp = [...]Chunk{0x0, 0xA100000, 0x2000E90, 0x7A, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0}
var SQRTm3= [...]Chunk {0xCD5F852,0x8D27AE1,0x4ECD47D,0x6D15DA1,0x62CC61F,0xC2A7979,0xF233770,0x3507F1D,0xA2D2BA9,0x0}
const MConst Chunk = 0x2253531

// Base Bits= 28

const CURVE_Cof_I int = 1
var CURVE_Cof = [...]Chunk{0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
const CURVE_B_I int = 7

var CURVE_B = [...]Chunk{0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
var CURVE_Order = [...]Chunk{0x364141, 0xD25E8CD, 0x8A03BBF, 0xDCE6AF4, 0xFFEBAAE, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xF}
var CURVE_Gx = [...]Chunk{0x6F81798, 0xF2815B1, 0xE28D959, 0xFCDB2DC, 0xB07029B, 0x95CE870, 0xC55A062, 0xF9DCBBA, 0x9BE667E, 0x7}
var CURVE_Gy = [...]Chunk{0xB10D4B8, 0x47D08FF, 0x554199C, 0xB448A68, 0x8A8FD17, 0xFC0E110, 0x55DA4FB, 0x26A3C46, 0x83ADA77, 0x4}
