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

package C25519

// Base Bits= 56
var Modulus = [...]Chunk{0xFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0x7FFFFFFF}
var ROI= [...]Chunk {0xEE1B274A0EA0B0,0x1806AD2FE478C4,0x993DFBD7A72F43,0x4FC1DF0B2B4D00,0x2B832480}
var R2modp = [...]Chunk{0xA4000000000000, 0x5, 0x0, 0x0, 0x0}

const MConst Chunk = 0x13

const CURVE_Cof_I int = 8

var CURVE_Cof = [...]Chunk{0x8, 0x0, 0x0, 0x0, 0x0}
const CURVE_B_I int = 0

var CURVE_B = [...]Chunk{0x0, 0x0, 0x0, 0x0, 0x0}
var CURVE_Order = [...]Chunk{0x12631A5CF5D3ED, 0xF9DEA2F79CD658, 0x14DE, 0x0, 0x10000000}
var CURVE_Gx = [...]Chunk{0x9, 0x0, 0x0, 0x0, 0x0}
var CURVE_Gy = [...]Chunk{0x0, 0x0, 0x0, 0x0, 0x0}
var CURVE_HTPC= [...]Chunk {0x770D93A507504F,0x8C035697F23C62,0x4C9EFDEBD397A1,0x27E0EF8595A680,0x55C19240}
