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
use crate::c25519::big::NLEN;

// Base Bits= 56
// Curve25519 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFFFFFFFFFED,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0x7FFFFFFF,
];
pub const ROI:[Chunk;NLEN]=[0xEE1B274A0EA0B0,0x1806AD2FE478C4,0x993DFBD7A72F43,0x4FC1DF0B2B4D00,0x2B832480];
pub const R2MODP: [Chunk; NLEN] = [0xA4000000000000, 0x5, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0x13;

// c25519 Curve
pub const CURVE_COF_I: isize = 8;
pub const CURVE_B_I: isize = 0;
pub const CURVE_COF: [Chunk; NLEN] = [0x8, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_ORDER: [Chunk; NLEN] =
    [0x12631A5CF5D3ED, 0xF9DEA2F79CD658, 0x14DE, 0x0, 0x10000000];
pub const CURVE_GX: [Chunk; NLEN] = [0x9, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_GY: [Chunk; NLEN] = [0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_HTPC:[Chunk;NLEN]=[0x770D93A507504F,0x8C035697F23C62,0x4C9EFDEBD397A1,0x27E0EF8595A680,0x55C19240];
