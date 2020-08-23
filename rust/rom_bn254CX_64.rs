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
use crate::bn254CX::big::NLEN;

// Base Bits= 56
// bn254CX Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0x6623EF5C1B55B3,
    0xD6EE18093EE1BE,
    0x647A6366D3243F,
    0x8702A0DB0BDDF,
    0x24000000,
];
pub const ROI: [Chunk; NLEN] = [
    0x6623EF5C1B55B2,
    0xD6EE18093EE1BE,
    0x647A6366D3243F,
    0x8702A0DB0BDDF,
    0x24000000,
];
pub const R2MODP: [Chunk; NLEN] = [
    0x466A0618A0800A,
    0x2B3A22543056A3,
    0x148515B09C6600,
    0xEC9EA5606BDF50,
    0x1C992E66,
];
pub const MCONST: Chunk = 0x4E205BF9789E85;
pub const SQRTM3:[Chunk;NLEN]=[0xA4FDC033D49C84,0x1825342A30355A,0x646112E8AAA2C4,0x8702A0D20BDDF,0x24000000];
pub const FRA: [Chunk; NLEN] = [
    0xD9083355C80EA3,
    0x7326F173F8215B,
    0x8AACA718986867,
    0xA63A0164AFE18B,
    0x1359082F,
];
pub const FRB: [Chunk; NLEN] = [
    0x8D1BBC06534710,
    0x63C7269546C062,
    0xD9CDBC4E3ABBD8,
    0x623628A900DC53,
    0x10A6F7D0,
];

// bn254CX Curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_B_I: isize = 2;
pub const CURVE_B: [Chunk; NLEN] = [0x2, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0x11C0A636EB1F6D,
    0xD6EE0CC906CEBE,
    0x647A6366D2C43F,
    0x8702A0DB0BDDF,
    0x24000000,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x6623EF5C1B55B2,
    0xD6EE18093EE1BE,
    0x647A6366D3243F,
    0x8702A0DB0BDDF,
    0x24000000,
];
pub const CURVE_GY: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_HTPC:[Chunk;NLEN]=[0x1,0x0,0x0,0x0,0x0];
pub const CURVE_BNX: [Chunk; NLEN] = [0x3C012B1, 0x40, 0x0, 0x0, 0x0];
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0];
pub const CRU: [Chunk; NLEN] = [
    0xE0931794235C97,
    0xDF6471EF875631,
    0xCA83F1440BD,
    0x480000,
    0x0,
];

pub const CURVE_PXA: [Chunk; NLEN] = [
    0x851CEEE4D2EC74,
    0x85BFA03E2726C0,
    0xF5C34BBB907C,
    0x7053B256358B25,
    0x19682D2C,
];
pub const CURVE_PXB: [Chunk; NLEN] = [
    0xA58E8B2E29CFE1,
    0x97B0C209C30F47,
    0x37A8E99743F81B,
    0x3E19F64AA011C9,
    0x1466B9EC,
];
pub const CURVE_PYA: [Chunk; NLEN] = [
    0xFBFCEBCF0BE09F,
    0xB33D847EC1B30C,
    0x157DAEE2096361,
    0x72332B8DD81E22,
    0xA79EDD9,
];
pub const CURVE_PYB: [Chunk; NLEN] = [
    0x904B228898EE9D,
    0x4EA569D2EDEBED,
    0x512D8D3461C286,
    0xECC4C09035C6E4,
    0x6160C39,
];

pub const CURVE_W: [[Chunk; NLEN]; 2] = [
    [0x546349162FEB83, 0xB40381200, 0x6000, 0x0, 0x0],
    [0x7802561, 0x80, 0x0, 0x0, 0x0],
];
pub const CURVE_SB: [[[Chunk; NLEN]; 2]; 2] = [
    [
        [0x5463491DB010E4, 0xB40381280, 0x6000, 0x0, 0x0],
        [0x7802561, 0x80, 0x0, 0x0, 0x0],
    ],
    [
        [0x7802561, 0x80, 0x0, 0x0, 0x0],
        [
            0xBD5D5D20BB33EA,
            0xD6EE0188CEBCBD,
            0x647A6366D2643F,
            0x8702A0DB0BDDF,
            0x24000000,
        ],
    ],
];
pub const CURVE_WB: [[Chunk; NLEN]; 4] = [
    [0x1C2118567A84B0, 0x3C012B040, 0x2000, 0x0, 0x0],
    [
        0xCDF995BE220475,
        0x94EDA8CA7F9A36,
        0x8702A0DC07E,
        0x300000,
        0x0,
    ],
    [
        0x66FCCAE0F10B93,
        0x4A76D4653FCD3B,
        0x4381506E03F,
        0x180000,
        0x0,
    ],
    [0x1C21185DFAAA11, 0x3C012B0C0, 0x2000, 0x0, 0x0],
];
pub const CURVE_BB: [[[Chunk; NLEN]; 4]; 4] = [
    [
        [
            0x11C0A6332B0CBD,
            0xD6EE0CC906CE7E,
            0x647A6366D2C43F,
            0x8702A0DB0BDDF,
            0x24000000,
        ],
        [
            0x11C0A6332B0CBC,
            0xD6EE0CC906CE7E,
            0x647A6366D2C43F,
            0x8702A0DB0BDDF,
            0x24000000,
        ],
        [
            0x11C0A6332B0CBC,
            0xD6EE0CC906CE7E,
            0x647A6366D2C43F,
            0x8702A0DB0BDDF,
            0x24000000,
        ],
        [0x7802562, 0x80, 0x0, 0x0, 0x0],
    ],
    [
        [0x7802561, 0x80, 0x0, 0x0, 0x0],
        [
            0x11C0A6332B0CBC,
            0xD6EE0CC906CE7E,
            0x647A6366D2C43F,
            0x8702A0DB0BDDF,
            0x24000000,
        ],
        [
            0x11C0A6332B0CBD,
            0xD6EE0CC906CE7E,
            0x647A6366D2C43F,
            0x8702A0DB0BDDF,
            0x24000000,
        ],
        [
            0x11C0A6332B0CBC,
            0xD6EE0CC906CE7E,
            0x647A6366D2C43F,
            0x8702A0DB0BDDF,
            0x24000000,
        ],
    ],
    [
        [0x7802562, 0x80, 0x0, 0x0, 0x0],
        [0x7802561, 0x80, 0x0, 0x0, 0x0],
        [0x7802561, 0x80, 0x0, 0x0, 0x0],
        [0x7802561, 0x80, 0x0, 0x0, 0x0],
    ],
    [
        [0x3C012B2, 0x40, 0x0, 0x0, 0x0],
        [0xF004AC2, 0x100, 0x0, 0x0, 0x0],
        [
            0x11C0A62F6AFA0A,
            0xD6EE0CC906CE3E,
            0x647A6366D2C43F,
            0x8702A0DB0BDDF,
            0x24000000,
        ],
        [0x3C012B2, 0x40, 0x0, 0x0, 0x0],
    ],
];

pub const USE_GLV: bool = true;
pub const USE_GS_G2: bool = true;
pub const USE_GS_GT: bool = true;
pub const GT_STRONG: bool = true;
