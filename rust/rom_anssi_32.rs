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

use crate::anssi::big::NLEN;
use crate::arch::Chunk;

// Base Bits= 28
// anssi Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0x86E9C03, 0xFCF353D, 0x8CA6DE8, 0xADBCABC, 0x35B3961, 0xE8CE424, 0xF10126D, 0xB3AD58,
    0x1FD178C, 0xF,
];
pub const ROI: [Chunk; NLEN] = [
    0x86E9C02, 0xFCF353D, 0x8CA6DE8, 0xADBCABC, 0x35B3961, 0xE8CE424, 0xF10126D, 0xB3AD58,
    0x1FD178C, 0xF,
];
pub const R2MODP: [Chunk; NLEN] = [
    0x288CC9C, 0x18D2374, 0x646BD2B, 0x4929E67, 0xD6F7F2D, 0x220E6C1, 0xABCE02E, 0x751B1FD,
    0x7401B78, 0xE,
];
pub const MCONST: Chunk = 0x64E1155;

// anssi Curve

pub const CURVE_COF_I: isize = 1;
pub const CURVE_B_I: isize = 0;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [
    0xB7BB73F, 0x75ED967, 0x1A18030, 0xC9AE4B, 0xFDFEC, 0x754A44C, 0xD4ABA, 0x5428A93, 0xE353FCA,
    0xE,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0x6D655E1, 0xFDD459C, 0x2BF941F, 0x67E140D, 0x35B53DC, 0xE8CE424, 0xF10126D, 0xB3AD58,
    0x1FD178C, 0xF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x98F5CFF, 0xC97A2DD, 0x8B70164, 0xD2DCAF9, 0x3958C27, 0x4749D42, 0xB31183D, 0x56C139E,
    0x6B3D4C3, 0xB,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x4062CFB, 0x115A155, 0x4C9E183, 0xC307E8E, 0xF8C2701, 0xF0F3ECE, 0x11F9271, 0xC8B2049,
    0x142E0F7, 0x6,
];
pub const CURVE_HTPC:[Chunk;NLEN]=[0x1A725D0,0xA8983E2,0x77B349E,0x66D2BB4,0x676B54C,0x119A095,0xCAC6DC7,0xFD18A3A,0x2906D4D,0x7];
