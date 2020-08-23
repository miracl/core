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

use crate::arch::Chunk;
use crate::nums256w::big::NLEN;

// Base Bits= 28
// nums256 modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFF43, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF,
    0xFFFFFFF, 0xF,
];
pub const ROI: [Chunk; NLEN] = [
    0xFFFFF42, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF,
    0xFFFFFFF, 0xF,
];
pub const R2MODP: [Chunk; NLEN] = [0x0, 0x8900000, 0x8B, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0xBD;

// nums256w curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_B_I: isize = 152961;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [0x25581, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0x751A825, 0xAB20294, 0x65C6020, 0x8275EA2, 0xFFFE43C, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF,
    0xFFFFFFF, 0xF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x21AACB1, 0x52EE1EB, 0x4C73ABC, 0x9B0903D, 0xB098357, 0xA04F42C, 0x1297A95, 0x5AAADB6,
    0xC9ED6B6, 0xB,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x184DE9F, 0xB5B9CB2, 0x10FBB80, 0xC3D1153, 0x35C955, 0xF77E04E, 0x673448B, 0x3399B6A,
    0x8FC0F1, 0xD,
];
pub const CURVE_HTPC:[Chunk;NLEN]=[0x52B84D6,0xFC6F759,0xE89E05,0x92C6204,0x6AE4640,0x70CADDC,0x5B22ED0,0x5411E3B,0xF7F44E4,0xC];
