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
use crate::nums512w::big::NLEN;

// Base Bits= 60
// nums512 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFFFFFFFFFDC7,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFF,
];
pub const ROI: [Chunk; NLEN] = [
    0xFFFFFFFFFFFFDC6,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFF,
];
pub const R2MODP: [Chunk; NLEN] = [0x100000000000000, 0x4F0B, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0x239;

// nums512w Curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_B_I: isize = 121243;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [0x1D99B, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xE153F390433555D,
    0x568B36607CD243C,
    0x258ED97D0BDC63B,
    0xA4FB94E7831B4FC,
    0xFFFFFFFFFFF5B3C,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0xC8287958CABAE57,
    0x5D60137D6F5DE2D,
    0x94286255615831D,
    0xA151076B359E937,
    0xC25306D9F95021,
    0x3BB501F6854506E,
    0x2A03D3B5298CAD8,
    0x141D0A93DA2B700,
    0x3AC03447,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x3A08760383527A6,
    0x2B5C1E4CFD0FE92,
    0x1A840B25A5602CF,
    0x15DA8B0EEDE9C12,
    0x60C7BD14F14A284,
    0xDEABBCBB8C8F4B2,
    0xC63EBB1004B97DB,
    0x29AD56B3CE0EEED,
    0x943A54CA,
];
pub const CURVE_HTPC: [Chunk; NLEN] = [0xFFFFFFFFFFFFEE3,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0x7FFFFFFF];

