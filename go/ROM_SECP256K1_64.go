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

// Base Bits= 56

var Modulus = [...]Chunk{0xFFFFFEFFFFFC2F, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFF}
var ROI = [...]Chunk{0xFFFFFEFFFFFC2E, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFF}
var R2modp = [...]Chunk{0xA1000000000000, 0x7A2000E90, 0x1, 0x0, 0x0}
var SQRTm3= [...]Chunk {0x8D27AE1CD5F852,0x6D15DA14ECD47D,0xC2A797962CC61F,0x3507F1DF233770,0xA2D2BA9}
const MConst Chunk = 0x38091DD2253531

// Base Bits= 56


const CURVE_Cof_I int = 1
var CURVE_Cof = [...]Chunk{0x1, 0x0, 0x0, 0x0, 0x0}
const CURVE_B_I int = 7
var CURVE_B = [...]Chunk{0x7, 0x0, 0x0, 0x0, 0x0}
var CURVE_Order = [...]Chunk{0xD25E8CD0364141, 0xDCE6AF48A03BBF, 0xFFFFFFFFFEBAAE, 0xFFFFFFFFFFFFFF, 0xFFFFFFFF}
var CURVE_Gx = [...]Chunk{0xF2815B16F81798, 0xFCDB2DCE28D959, 0x95CE870B07029B, 0xF9DCBBAC55A062, 0x79BE667E}
var CURVE_Gy = [...]Chunk{0x47D08FFB10D4B8, 0xB448A68554199C, 0xFC0E1108A8FD17, 0x26A3C4655DA4FB, 0x483ADA77}
