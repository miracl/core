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

// Base Bits= 60
pub const MODULUS:[Chunk;NLEN]=[0xFFFFFFFFFFFFFFB,0xFFFFFFFFFFFFFFF,0x3FFFFFFFFFFF];
pub const R2MODP:[Chunk;NLEN]=[0x190000000,0x0,0x0];
pub const ROI:[Chunk;NLEN]=[0xFFFFFFFFFFFFFFA,0xFFFFFFFFFFFFFFF,0x3FFFFFFFFFFF];
pub const MCONST:Chunk=0x5;

//*** rom curve parameters *****
// Base Bits= 60

pub const CURVE_COF_I:isize = 4;
pub const CURVE_COF:[Chunk;NLEN]=[0x4,0x0,0x0];
pub const CURVE_B_I:isize = 5766;
pub const CURVE_B:[Chunk;NLEN]=[0x1686,0x0,0x0];
pub const CURVE_ORDER:[Chunk;NLEN]=[0x80FF0A99DBA8B27,0xFFFFFFFFFD5EF01,0xFFFFFFFFFFF];
pub const CURVE_GX:[Chunk;NLEN]=[0x671B9DBF9D52398,0x9A0618EE0F666C,0x14C94DA505B8];
pub const CURVE_GY:[Chunk;NLEN]=[0xC7087B244920345,0x13BFC7739D62DC2,0x29414549BC0A];
pub const CURVE_HTPC:[Chunk;NLEN]=[0x1,0x0,0x0];
