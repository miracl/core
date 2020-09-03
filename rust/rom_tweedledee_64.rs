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

use crate::tweedledee::big::NLEN;
use crate::arch::Chunk;

// Base  bits= 56
pub const MODULUS:[Chunk;NLEN]=[0x2CAFD400000001,0xA127696286C984,0x38A,0x0,0x40000000];
pub const R2MODP:[Chunk;NLEN]=[0x21CA8949BBE5DF,0x717E3D9E719200,0xB8A93694FCEE5,0xE6FCB03AA0A27E,0x2013F4C0];
pub const ROI:[Chunk;NLEN]=[0xBD3234869D57EC,0x7DD1B8084FBF1C,0x645A9872930A28,0xDC03C0BF1DBCB,0x113EFC51];
pub const SQRTM3:[Chunk;NLEN]=[0xFCD5A6291F49FA,0x7AA6D867D6B902,0xBFF2E63FFCD014,0x3C0934B10F5B16,0x2D8CDA74];
pub const MCONST:Chunk=0x2CAFD3FFFFFFFF;

//*** rom curve parameters *****
pub const CURVE_COF_I:isize = 1;
pub const CURVE_COF:[Chunk;NLEN]=[0x1,0x0,0x0,0x0,0x0];
pub const CURVE_B_I:isize = 5;
pub const CURVE_B:[Chunk;NLEN]=[0x5,0x0,0x0,0x0,0x0];
pub const CURVE_ORDER:[Chunk;NLEN]=[0x4064E200000001,0xA1276C3F59B9A1,0x38A,0x0,0x40000000];
pub const CURVE_GX:[Chunk;NLEN]=[0x2CAFD400000000,0xA127696286C984,0x38A,0x0,0x40000000];
pub const CURVE_GY:[Chunk;NLEN]=[0x2,0x0,0x0,0x0,0x0];
pub const CURVE_HTPC:[Chunk;NLEN]=[0x861CBDAD69C848,0x754672EF68733,0xE5BC1A0E9704AC,0x2A64972082C6A2,0x3AC188DE];
