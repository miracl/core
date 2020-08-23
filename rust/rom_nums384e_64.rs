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
use crate::nums384e::big::NLEN;

// Base Bits= 56
// nums384 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFFFFFFFFEC3,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFF,
];
pub const ROI: [Chunk; NLEN] = [
    0xFFFFFFFFFFFEC2,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFF,
];
pub const R2MODP: [Chunk; NLEN] = [0x188890000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0x13D;

// nums384e Curve
pub const CURVE_COF_I: isize = 4;
pub const CURVE_B_I: isize = -11556;
pub const CURVE_COF: [Chunk; NLEN] = [0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [
    0xFFFFFFFFFFD19F,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFF,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xB9DCC4E6A3897D,
    0x555AAB35C87920,
    0x1CB46BE1CF61E4,
    0xFFFFFFFFE2471A,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0x3FFFFFFFFFFF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0xD540E46C206BDE,
    0x92B16545941350,
    0xA8F33163406FF2,
    0xE5BE4C005763FF,
    0xE55DB5B30BF446,
    0x266CC0B6A2129A,
    0x61B111FB45A9,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x8D03E1F0729392,
    0xB0F946EC48DC9D,
    0xF7F645964B0072,
    0xF1425F56830F98,
    0xB10DD716AD8274,
    0xEEB08738B1A423,
    0x82983E67B9A6,
];
pub const CURVE_HTPC:[Chunk;NLEN]=[0x1,0x0,0x0,0x0,0x0,0x0,0x0];
