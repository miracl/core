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

package NUMS256W

// Base Bits= 28
var Modulus = [...]Chunk{0xFFFFF43, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xF}
var ROI = [...]Chunk{0xFFFFF42, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xF}
var R2modp = [...]Chunk{0x0, 0x8900000, 0x8B, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const MConst Chunk = 0xBD

const CURVE_Cof_I int = 1

var CURVE_Cof = [...]Chunk{0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
const CURVE_B_I int = 152961

var CURVE_B = [...]Chunk{0x25581, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
var CURVE_Order = [...]Chunk{0x751A825, 0xAB20294, 0x65C6020, 0x8275EA2, 0xFFFE43C, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xF}
var CURVE_Gx = [...]Chunk{0x21AACB1, 0x52EE1EB, 0x4C73ABC, 0x9B0903D, 0xB098357, 0xA04F42C, 0x1297A95, 0x5AAADB6, 0xC9ED6B6, 0xB}
var CURVE_Gy = [...]Chunk{0x184DE9F, 0xB5B9CB2, 0x10FBB80, 0xC3D1153, 0x35C955, 0xF77E04E, 0x673448B, 0x3399B6A, 0x8FC0F1, 0xD}
var CURVE_HTPC= [...]Chunk {0x52B84D6,0xFC6F759,0xE89E05,0x92C6204,0x6AE4640,0x70CADDC,0x5B22ED0,0x5411E3B,0xF7F44E4,0xC}
