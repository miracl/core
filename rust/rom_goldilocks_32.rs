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
use crate::goldilocks::big::NLEN;

// Base Bits= 29
// Goldilocks modulus
pub const MODULUS: [Chunk; NLEN] = [
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FDFFFFF,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFF,
];
pub const ROI: [Chunk; NLEN] = [
    0x1FFFFFFE, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FDFFFFF,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFF,
];
pub const R2MODP: [Chunk; NLEN] = [
    0x0, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
];
pub const MCONST: Chunk = 0x1;

// Goldilocks curve
pub const CURVE_COF_I: isize = 4;
pub const CURVE_B_I: isize = -39081;
pub const CURVE_COF: [Chunk; NLEN] = [
    0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
];
pub const CURVE_B: [Chunk; NLEN] = [
    0x1FFF6756, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FDFFFFF,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFF,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xB5844F3, 0x1BC61495, 0x1163D548, 0x1984E51B, 0x3690216, 0xDA4D76B, 0xFA7113B, 0x1FEF9944,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x7FF,
];
//pub const CURVE_GX: [Chunk; NLEN] = [
//    0x15555555, 0xAAAAAAA, 0x15555555, 0xAAAAAAA, 0x15555555, 0xAAAAAAA, 0x15555555, 0x152AAAAA,
//    0xAAAAAAA, 0x15555555, 0xAAAAAAA, 0x15555555, 0xAAAAAAA, 0x15555555, 0xAAAAAAA, 0x1555,
//];
//pub const CURVE_GY: [Chunk; NLEN] = [
//    0xA9386ED, 0x1757DE6F, 0x13681AF6, 0x19657DA3, 0x3098BBB, 0x12C19D15, 0x12E03595, 0xE515B18,
//    0x17B7E36D, 0x1AC426E, 0xDBB5E8, 0x10D8560, 0x159D6205, 0xB8246D9, 0x17A58D2B, 0x15C0,
//];
pub const CURVE_GX:[Chunk;NLEN]=[0x70CC05E,0x1135415E,0x24E389,0x1701C316,0x6511433,0xD7B955B,0x11904AB8,0x4947A74,0x7EA6DE3,0x23878BB,0x785195C,0x57E6DB5,0x1D15A62,0x1686F691,0x5C319AF,0x9E3];
pub const CURVE_GY:[Chunk;NLEN]=[0x1230FA14,0x43CADF,0x15F22B66,0x1A26589D,0x39C4FDB,0x1F8E733E,0xB5CEB4F,0x3C0B418,0x87789C,0x1B651CC2,0x12FA9CDC,0xD938EC4,0x7620375,0x1B5E1244,0x1D19C5BA,0xD27];
pub const CURVE_HTPC:[Chunk;NLEN]=[0x1FFFFFFE,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FDFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFF];
