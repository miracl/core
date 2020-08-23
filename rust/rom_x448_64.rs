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
use crate::x448::big::NLEN;

// Base Bits= 58
// Goldilocks modulus
pub const MODULUS: [Chunk; NLEN] = [
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FBFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFF,
];
pub const ROI: [Chunk; NLEN] = [
    0x3FFFFFFFFFFFFFE,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FBFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFF,
];
pub const R2MODP: [Chunk; NLEN] = [0x200000000, 0x0, 0x0, 0x0, 0x3000000, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0x1;

// x448 curve
pub const CURVE_COF_I:isize = 4;
pub const CURVE_COF:[Chunk;NLEN]=[0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0];
pub const CURVE_B_I:isize = 0;
pub const CURVE_B:[Chunk;NLEN]=[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0];
pub const CURVE_ORDER:[Chunk;NLEN]=[0x378C292AB5844F3,0x3309CA37163D548,0x1B49AED63690216,0x3FDF3288FA7113B,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0xFFFFFFFFFF];
pub const CURVE_GX:[Chunk;NLEN]=[0x5,0x0,0x0,0x0,0x0,0x0,0x0,0x0];
pub const CURVE_GY:[Chunk;NLEN]=[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0];
pub const CURVE_HTPC:[Chunk;NLEN]=[0x3FFFFFFFFFFFFFE,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FBFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFF];
