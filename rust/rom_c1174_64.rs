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
use crate::c1174::big::NLEN;

// Base Bits= 56
// c1174 Modulus
pub const MODULUS:[Chunk;NLEN]=[0xFFFFFFFFFFFFF7,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFF];
pub const R2MODP:[Chunk;NLEN]=[0x0,0x144,0x0,0x0,0x0];
pub const ROI:[Chunk;NLEN]=[0xFFFFFFFFFFFFF6,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFF];
pub const MCONST:Chunk=0x9;

//*** rom curve parameters *****
// Base Bits= 56

pub const CURVE_COF_I:isize = 4;
pub const CURVE_COF:[Chunk;NLEN]=[0x4,0x0,0x0,0x0,0x0];
pub const CURVE_B_I:isize = -1174;
pub const CURVE_B:[Chunk;NLEN]=[0xFFFFFFFFFFFB61,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFF];
pub const CURVE_ORDER:[Chunk;NLEN]=[0x44D45FD166C971,0x65C4DFD3073489,0xFFFFFFFFFFF779,0xFFFFFFFFFFFFFF,0x1FFFFFF];
pub const CURVE_GX:[Chunk;NLEN]=[0x123F27BCE29EDA,0xD96A492ECD6516,0xE7C029A190C021,0xEA308C479343AE,0x37FBB0C];
pub const CURVE_GY:[Chunk;NLEN]=[0xCCB1BF9B46360E,0xDEE2AF3F976BA4,0x1169840E0C4FE2,0xD47FB7CC665684,0x6B72F82];
pub const CURVE_HTPC:[Chunk;NLEN]=[0xFFFFFFFFFFFFF6,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFF];
