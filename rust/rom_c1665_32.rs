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
use crate::c1665::big::NLEN;

// Base Bits= 29
// c1174 Modulus
pub const MODULUS:[Chunk;NLEN]=[0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFF];
pub const R2MODP:[Chunk;NLEN]=[0x190000,0x0,0x0,0x0,0x0,0x0];
pub const ROI:[Chunk;NLEN]=[0x1FFFFFFA,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFF];
pub const MCONST:Chunk=0x5;

//*** rom curve parameters *****
// Base Bits= 29

pub const CURVE_COF_I:isize = 4;
pub const CURVE_COF:[Chunk;NLEN]=[0x4,0x0,0x0,0x0,0x0,0x0];
pub const CURVE_B_I:isize = 5766;
pub const CURVE_B:[Chunk;NLEN]=[0x1686,0x0,0x0,0x0,0x0,0x0];
pub const CURVE_ORDER:[Chunk;NLEN]=[0x1DBA8B27,0x7F854C,0x1F57BC06,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF];
pub const CURVE_GX:[Chunk;NLEN]=[0x19D52398,0x138DCEDF,0x183D99B1,0x1340C31D,0x1A505B80,0xA64A6];
pub const CURVE_GY:[Chunk;NLEN]=[0x4920345,0x3843D92,0x758B70B,0x77F8EE7,0x149BC0A1,0x14A0A2];
pub const CURVE_HTPC:[Chunk;NLEN]=[0x1,0x0,0x0,0x0,0x0,0x0];
