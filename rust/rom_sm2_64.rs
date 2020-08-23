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

use crate::arch::Chunk;
use crate::sm2::big::NLEN;

// Base Bits= 56
pub const MODULUS:[Chunk;NLEN]=[0xFFFFFFFFFFFFFF,0xFFFF00000000FF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFE];
pub const ROI:[Chunk;NLEN]=[0xFFFFFFFFFFFFFE,0xFFFF00000000FF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFE];
pub const R2MODP:[Chunk;NLEN]=[0x3000000040000,0xFFFFFFFE000000,0x300000002,0x10000000100,0x60000];
pub const MCONST:Chunk=0x1;

//*** rom curve parameters *****
// Base Bits= 56

pub const CURVE_COF_I:isize = 1;
pub const CURVE_COF:[Chunk;NLEN]=[0x1,0x0,0x0,0x0,0x0];
pub const CURVE_B_I:isize = 0;
pub const CURVE_B:[Chunk;NLEN]=[0xBCBD414D940E93,0x89F515AB8F92DD,0x4BCF6509A7F397,0x9D9F5E344D5A9E,0x28E9FA9E];
pub const CURVE_ORDER:[Chunk;NLEN]=[0xBBF40939D54123,0xDF6B21C6052B53,0xFFFFFFFFFF7203,0xFFFFFFFFFFFFFF,0xFFFFFFFE];
pub const CURVE_GX:[Chunk;NLEN]=[0x5A4589334C74C7,0xBBFF2660BE171,0x466A39C9948FE3,0x1F1981195F9904,0x32C4AE2C];
pub const CURVE_GY:[Chunk;NLEN]=[0xDF32E52139F0A0,0x877CC62A474002,0xE36B692153D0A9,0xF4F6779C59BDCE,0xBC3736A2];
pub const CURVE_HTPC:[Chunk;NLEN]=[0xFFFFFFFFFFFFFF,0xFFFF55555555FF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xAAAAAAA9];
