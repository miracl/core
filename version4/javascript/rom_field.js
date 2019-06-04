/*
    Licensed to the Apache Software Foundation (ASF) under one
    or more contributor license agreements.  See the NOTICE file
    distributed with this work for additional information
    regarding copyright ownership.  The ASF licenses this file
    to you under the Apache License, Version 2.0 (the
    "License"); you may not use this file except in compliance
    with the License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing,
    software distributed under the License is distributed on an
    "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
    KIND, either express or implied.  See the License for the
    specific language governing permissions and limitations
    under the License.
*/

/* Fixed Data in ROM - Field and Curve parameters */

var ROM_FIELD_25519,
    ROM_FIELD_256PM,
    ROM_FIELD_384PM,
    ROM_FIELD_512PM,
    ROM_FIELD_ANSSI,
    ROM_FIELD_BLS383,
    ROM_FIELD_BLS24,
    ROM_FIELD_BLS48,
	ROM_FIELD_BLS48_581,
    ROM_FIELD_BLS381,
    ROM_FIELD_BLS461,
	ROM_FIELD_BN462,
    ROM_FIELD_FP256BN,
    ROM_FIELD_FP512BN,
    ROM_FIELD_BN254,
    ROM_FIELD_BN254CX,
    ROM_FIELD_BRAINPOOL,
    ROM_FIELD_C41417,
    ROM_FIELD_GOLDILOCKS,
    ROM_FIELD_HIFIVE,
    ROM_FIELD_NIST256,
    ROM_FIELD_NIST384,
    ROM_FIELD_NIST521,
    ROM_FIELD_SECP256K1,
	ROM_FIELD_SM2;

ROM_FIELD_25519 = function() {
    "use strict";

    var ROM_FIELD_25519 = {

        // 25519 Curve Modulus
        Modulus: [0xFFFFED, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x7FFF],
        R2modp: [0xA40000, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0],
        MConst: 0x13,

    };
    return ROM_FIELD_25519;
};

ROM_FIELD_256PM = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_256PM = {

        // NUMS256 Curve Modulus
        // Base Bits= 24
        Modulus: [0xFFFF43, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFF],
        R2modp: [0x890000, 0x8B, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0],
        MConst: 0xBD,

    };
    return ROM_FIELD_256PM;
};

ROM_FIELD_384PM = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_384PM = {

        // NUMS384 Curve Modulus
        // Base Bits= 23
        Modulus: [0x7FFEC3, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0xFFFF],
        R2modp: [0x224000, 0xC4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0],
        MConst: 0x13D,

    };
    return ROM_FIELD_384PM;
};

ROM_FIELD_512PM = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_512PM = {

        // NUMS512 Curve Modulus
        // Base Bits= 23
        Modulus: [0x7FFDC7, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x3F],
        R2modp: [0x0, 0x58800, 0x4F, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0],
        MConst: 0x239,

    };
    return ROM_FIELD_512PM;
};

ROM_FIELD_ANSSI = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_ANSSI = {

        // ANSSI modulus
        // Base Bits= 24
        Modulus: [0x6E9C03, 0xF353D8, 0x6DE8FC, 0xABC8CA, 0x61ADBC, 0x435B39, 0xE8CE42, 0x10126D, 0x3AD58F, 0x178C0B, 0xF1FD],
        R2modp: [0xACECE3, 0x924166, 0xB10FCE, 0x6CFBB6, 0x87EC2, 0x3DE43D, 0xD2CF67, 0xA67DDE, 0xAD30F2, 0xBCAAE, 0xDF98],
        MConst: 0x4E1155,

    };
    return ROM_FIELD_ANSSI;
};

ROM_FIELD_BLS383 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_BLS383 = {

        // BLS383 Modulus
        // Base Bits= 23
        Modulus: [0x2AB0AB,0x3AC90B,0x2F68DC,0x532429,0x43F298,0x1E8F51,0x5A5849,0x18DC00,0x2511AC,0x59E6CB,0x2B518,0x549425,0x5C41FE,0x340DB5,0x2ADBAD,0x2B4AB2,0x5565],
        R2modp: [0x250A44,0x68F66F,0xE3C74,0x791772,0x3525E3,0xE1E15,0x356616,0x54F624,0x508069,0x272663,0x4A4CB0,0x359293,0x5B6573,0x9F27F,0x5EA3B4,0x60FD2D,0x5167],
        MConst: 0x3435FD,
        Fra: [0x11DAC1,0x2E5A66,0x614B,0x733B9F,0x13480F,0x19146D,0x395436,0x2B3A25,0x1A8682,0x247F74,0x3931B3,0x5A9788,0x7C2C11,0x67173,0x1FDA2F,0x6ADF81,0x22AC],
        Frb: [0x18D5EA,0xC6EA5,0x2F0791,0x5FE88A,0x30AA88,0x57AE4,0x210413,0x6DA1DB,0xA8B29,0x356757,0x498365,0x79FC9C,0x6015EC,0x2D9C41,0xB017E,0x406B31,0x32B8],


    };

    return ROM_FIELD_BLS383;
};


ROM_FIELD_BLS24 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_BLS24 = {

        // BLS24 Modulus
        // Base Bits= 23

        Modulus: [0x6152B,0x2CE94,0x6BE113,0x416986,0x2FFE2E,0x36D4C8,0x47172F,0x1945B7,0x5F068A,0xE6441,0x110186,0x4F0F9,0x33568E,0x4A0F2E,0x306EA0,0x173BF2,0x6E803F,0x735D8,0x3316EA,0x3C01E,0x555C0],
        R2modp: [0x22D6FA,0x7AA299,0x4C307E,0x68E711,0x7DA4AE,0x383CC3,0x12048C,0x11B7D,0x3CA412,0x2CE421,0x4932AC,0x27A306,0x340B6A,0x666E,0x3F6575,0x2F823C,0xA0DE6,0x137EC5,0x37D4BC,0x48A54E,0x4C28B],
        MConst: 0x15FE7D,
        Fra: [0x796F1D,0x4E9577,0x6EB572,0x68637F,0x41FF8B,0x46E8D3,0x7A7898,0x7C72A4,0x248407,0x6E79D9,0x56499E,0x4EB47F,0x27CBD6,0x33C662,0x4E9746,0xC2798,0x397549,0x4A5B1B,0x5C90B6,0x3DCA73,0x4BBC8],
        Frb: [0xCA60E,0x34391C,0x7D2BA0,0x590606,0x6DFEA2,0x6FEBF4,0x4C9E96,0x1CD312,0x3A8282,0x1FEA68,0x3AB7E7,0x363C79,0xB8AB7,0x1648CC,0x61D75A,0xB1459,0x350AF6,0x3CDABD,0x568633,0x45F5AA,0x99F7],

    };

    return ROM_FIELD_BLS24;
};


ROM_FIELD_BLS48_581 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_BLS48_581 = {
// Base Bits= 23
		Modulus: [0x65912B,0x2A9CCA,0x30FB70,0x525245,0x7DF721,0x414AF1,0x4C5C3B,0x33D2A8,0x5AF683,0x1E8E6D,0x34C0E5,0x1566CE,0x2F6E08,0x115C87,0x551129,0x78F9CB,0x3BE6C0,0x426376,0x3E3448,0x2B742,0x7012A0,0x1C63A8,0x4C4E09,0x79A3B7,0xF73F,0x25],
		R2modp: [0x4E2506,0x1FAD8C,0x618BE7,0x23E9BF,0x6A8AD9,0x3529C9,0xB64D9,0x1C7A8E,0x759816,0x2AE889,0x3B1F74,0x53D536,0x7CFF54,0x389EE8,0x4F41F1,0x2C5D2D,0x48D19D,0x6C88A7,0x7B224E,0x3B0DB8,0x3BDCF8,0x83ABF,0x5B548E,0x50A9D7,0x362F43,0x10],
		MConst: 0x1D5A7D,
		Fra: [0x2AEB25,0x6D9FC8,0x7E58BA,0x3B0569,0x341942,0x3D9C9B,0x15C1C5,0x7D3EF9,0x208AC0,0x6EF6A2,0x106240,0x1F0E46,0x684849,0x3030FC,0xC1F8A,0x317A4B,0x4AFD6B,0xE81C7,0xF9E16,0x6823CC,0x5F6067,0x7CE7CA,0x69D735,0x280A4F,0x7D30DB,0x1],
		Frb: [0x2AEB25,0x6D9FC8,0x7E58BA,0x3B0569,0x341942,0x3D9C9B,0x15C1C5,0x7D3EF9,0x208AC0,0x6EF6A2,0x106240,0x1F0E46,0x684849,0x3030FC,0xC1F8A,0x317A4B,0x4AFD6B,0xE81C7,0xF9E16,0x6823CC,0x5F6067,0x7CE7CA,0x69D735,0x280A4F,0x7D30DB,0x1],
    };

    return ROM_FIELD_BLS48_581;
};

ROM_FIELD_BLS48 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_BLS48 = {

        // BLS48 Modulus
        // Base Bits= 23

        Modulus: [0x76AC0B,0x4C1FF9,0x67BBDB,0x5330EF,0x167009,0x450805,0x61C350,0x609BD4,0x76B2E,0x40410D,0x169054,0x353E01,0x141301,0x66F371,0x3B355A,0x6D4A85,0x36F405,0x28840A,0x454AB3,0x2B6433,0x29047A,0xB646E,0xBFF3F,0x68BEC2,0xF],
        R2modp: [0x5F42C2,0x596E88,0x2ED8FA,0x15C970,0x2518B4,0x2A75E7,0x62CE53,0x431C50,0x3CF507,0x620E44,0xD6FCD,0x21A7D,0x1FDA3F,0x6A099,0x53487,0x53EEBF,0x54E2D0,0x48437D,0x2233D8,0x63296F,0x21EE21,0x611417,0x619D35,0x13A61A,0xB],
        MConst: 0x5A805D,
        Fra: [0x25BF89,0x79FB26,0x56F988,0x399A14,0x507EA3,0x77995,0x3EE83A,0x52ECA9,0x3E3474,0x5F1E13,0x2E7CB0,0x255F3D,0x3AE7F8,0x2E4EF6,0x3BDE94,0x7B05A,0x13C83C,0x7BF664,0x1FF27F,0x6FE082,0x3B36CE,0x138113,0x6E2002,0x4C5C03,0x2],
        Frb: [0x25BF89,0x79FB26,0x56F988,0x399A14,0x507EA3,0x77995,0x3EE83A,0x52ECA9,0x3E3474,0x5F1E13,0x2E7CB0,0x255F3D,0x3AE7F8,0x2E4EF6,0x3BDE94,0x7B05A,0x13C83C,0x7BF664,0x1FF27F,0x6FE082,0x3B36CE,0x138113,0x6E2002,0x4C5C03,0x2],
    };

    return ROM_FIELD_BLS48;
};


ROM_FIELD_BN462 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_BN462 = {

// Base Bits= 23
		Modulus: [0x138013,0x601080,0x9006,0x0,0x7F6400,0xDFED0,0x3FD9BF,0x7FFFFB,0xFFFFF,0x39004,0x4A423D,0x6C0,0x0,0x37F940,0x3DADF6,0x7B7F86,0x7FFFFF,0x47FFF,0x580480,0x202401,0x2],
		R2modp: [0x8B8D,0x7265C,0x56003A,0xE1112,0x275149,0x50BFF0,0x1E3E2E,0x2DEDF3,0x43D666,0x644751,0x2A7239,0x1DCA94,0x5F2FF8,0x41CFF0,0x41494F,0x34B04A,0x47932F,0x5E4BBC,0x50BC6,0x30F0FB,0x1],
		MConst: 0x1BB5E5,
		Fra: [0x575D1A,0x7C7FE4,0x6F7B92,0x44B2CE,0xFC7D,0x27F423,0x517476,0x3C6C86,0x3310CC,0xAC42E,0x3551F0,0x3C9282,0x7E11C,0x207535,0x2164BB,0x8B938,0x35A8CC,0x179FB,0x5232ED,0xB7FA1,0x0],
		Frb: [0x5FF85B,0x15C12A,0x2D8881,0x432BDF,0x90238,0x2C6144,0x653D2F,0x786807,0x27A84F,0x362943,0x2C76EB,0x6C72,0x685F7B,0x794003,0x6C9D24,0x5B4192,0x2008E6,0x21D510,0x2503A,0x1E1BFC,0x2],
	};

    return ROM_FIELD_BN462;
};

ROM_FIELD_BLS381 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_BLS381 = {

        // BLS381 Modulus
        // Base Bits= 23

        Modulus: [0x7FAAAB,0x7FFFFF,0x7EE7FB,0xA9FFF,0x3FFFEB,0x4483D5,0x3DAC3D,0x186950,0x12BF67,0x9E70A,0x11DD2E,0x5D66BB,0x7B6434,0x496374,0x5FF9A6,0x8F51C,0x1A01],
        R2modp: [0x40C6E6,0xE1A28,0x3D1C6C,0x6D2448,0x1BB111,0x4EAFA8,0x229C8C,0x4CEE55,0x46D2AD,0x7BA87C,0x708835,0x2413D1,0x6702E3,0x390116,0xD9E3F,0x4BD65C,0x9A3],
        MConst: 0x7CFFFD,
        Fra: [0x235FB8,0x6BDB24,0x76341D,0x1F3C09,0x6A53D6,0x389ECF,0x612EAE,0x1221EB,0x5F4F7B,0x7A797A,0x3F580F,0x6068F8,0x6B4202,0x784637,0x2EC199,0x69DF81,0x1904],
        Frb: [0x5C4AF3,0x1424DB,0x8B3DE,0x6B63F6,0x55AC14,0xBE505,0x5C7D8F,0x64764,0x336FEC,0xF6D8F,0x52851E,0x7CFDC2,0x102231,0x511D3D,0x31380C,0x1F159B,0xFC],

    };

    return ROM_FIELD_BLS381;
};


ROM_FIELD_BLS461 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_BLS461 = {

        // BLS461 Modulus
        // Base Bits= 23
        Modulus: [0x2AAAAB, 0x155, 0x2AAAB0, 0x2AAA55, 0x55, 0x80004, 0x555FC0, 0x135548, 0x1CC00F, 0x3FF4B8, 0x2D0AA3, 0x58A424, 0x2CCA47, 0x465B17, 0x6F5BC7, 0xA49AF, 0x55D694, 0x34AAB4, 0x155535, 0x2AAAAA, 0x1],
        R2modp: [0x621498, 0x3B585F, 0x41688, 0x6F780D, 0x17C239, 0x158D8A, 0x491A92, 0x737DF1, 0x22A06, 0x460263, 0x275FF2, 0x5496C3, 0x6D4AD2, 0x3A7B46, 0x3A6323, 0x1723B1, 0x76204B, 0x66FD26, 0x4E743E, 0x1BE66E, 0x0],
        MConst: 0x7FFFFD,
        Fra: [0x12A3A, 0x2F7F37, 0x3DC4, 0x52CCE2, 0x1C6308, 0xB7F14, 0x4381D4, 0x52D328, 0x58D45F, 0x359C90, 0x1DC2CC, 0x616582, 0x7C61EB, 0x6B11C5, 0x64341C, 0x421B30, 0x4DFEFA, 0x3CABC4, 0x12DFDA, 0x172028, 0x1],
        Frb: [0x298071, 0x50821E, 0x2A6CEB, 0x57DD73, 0x639D4C, 0x7C80EF, 0x11DDEB, 0x408220, 0x43EBAF, 0xA5827, 0xF47D7, 0x773EA2, 0x30685B, 0x5B4951, 0xB27AA, 0x482E7F, 0x7D799, 0x77FEF0, 0x2755A, 0x138A82, 0x0],

    };

    return ROM_FIELD_BLS461;
};


ROM_FIELD_FP256BN = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_FP256BN = {

        // FP256BN Modulus
        // Base Bits= 24
        Modulus: [0xD33013, 0x2DDBAE, 0x82D329, 0x12980A, 0xDC65FB, 0xA49F0C, 0x5EEE71, 0x46E5F2, 0xFCF0CD, 0xFFFFFF, 0xFFFF],
        R2modp: [0x2F4801, 0xF779D1, 0x3E7F6E, 0xB42A3A, 0xC919C9, 0xC26C08, 0x1BB715, 0xCA2ED6, 0x54293E, 0xE578E, 0x78EA],
        MConst: 0x37E5E5,
        Fra: [0x943106, 0x328AF, 0x8F7476, 0x1E3AB2, 0xA17151, 0x67CF39, 0x8DDB08, 0x2D1A6E, 0x786F35, 0x7662CA, 0x3D61],
        Frb: [0x3EFF0D, 0x2AB2FF, 0xF35EB3, 0xF45D57, 0x3AF4A9, 0x3CCFD3, 0xD11369, 0x19CB83, 0x848198, 0x899D35, 0xC29E],
    };

    return ROM_FIELD_FP256BN;
};

ROM_FIELD_FP512BN = function() {
    "use strict";

    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_FP512BN = {

        // FP512BN Modulus
        // Base Bits= 23
        Modulus: [0x2DEF33, 0x501245, 0x1ED3AC, 0x7A6323, 0x255CE5, 0x7C322D, 0x2AC8DB, 0x4632EF, 0x18B8E4, 0x3D597D, 0x451B3C, 0x77A2A, 0x3C111B, 0x78177C, 0x32D4C1, 0x5D0EC, 0x7F01C6, 0x7FF3D8, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x3F],
        R2modp: [0x23E65D, 0x575A37, 0x411CD0, 0x295FB3, 0x640669, 0x375C69, 0x92395, 0x738492, 0x780D6D, 0x1BCD9D, 0x417CAA, 0x2DC6FB, 0x7EACFB, 0x327043, 0x7F2FC7, 0xF268C, 0x73D733, 0x2147C9, 0x2ACCD3, 0x32EAF8, 0x3B2C1E, 0xD46A2, 0x30],
        MConst: 0x4C5C05,
        Fra: [0x373AB2, 0x2F63E9, 0x47D258, 0x101576, 0x1514F6, 0x503C2E, 0x34EF61, 0x4FB040, 0x2CBBB5, 0x553D0A, 0x63A7E2, 0x10341C, 0x48CF2E, 0x3564D7, 0x25BDE4, 0x50C529, 0x468B4E, 0x2D518F, 0x6DE46, 0x7C84AD, 0x1CF5BB, 0x5EE355, 0x7],
        Frb: [0x76B481, 0x20AE5B, 0x570154, 0x6A4DAC, 0x1047EF, 0x2BF5FF, 0x75D97A, 0x7682AE, 0x6BFD2E, 0x681C72, 0x617359, 0x77460D, 0x7341EC, 0x42B2A4, 0xD16DD, 0x350BC3, 0x387677, 0x52A249, 0x7921B9, 0x37B52, 0x630A44, 0x211CAA, 0x38],

    };

    return ROM_FIELD_FP512BN;
};


ROM_FIELD_BN254 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_BN254 = {

        // BN254 Modulus
        // Base Bits= 24
        Modulus: [0x13, 0x0, 0x13A700, 0x0, 0x210000, 0x861, 0x800000, 0xBA344D, 0x1, 0x648240, 0x2523],
        R2modp: [0x2F2AA7, 0x537047, 0xF8F174, 0xC3E364, 0xAB8C1C, 0x3C2035, 0x69549, 0x379287, 0x3BE629, 0x75617A, 0x1F47],
        MConst: 0x9435E5,
        Fra: [0x2A6DE9, 0xE6C06F, 0xC2E17D, 0x4D3F77, 0x97492, 0x953F85, 0x50A846, 0xB6499B, 0x2E7C8C, 0x761921, 0x1B37],
        Frb: [0xD5922A, 0x193F90, 0x50C582, 0xB2C088, 0x178B6D, 0x6AC8DC, 0x2F57B9, 0x3EAB2, 0xD18375, 0xEE691E, 0x9EB],

    };

    return ROM_FIELD_BN254;
};

ROM_FIELD_BN254CX = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_BN254CX = {

        // BN254CX Modulus
        // Base Bits= 24
        Modulus: [0x1B55B3, 0x23EF5C, 0xE1BE66, 0x18093E, 0x3FD6EE, 0x66D324, 0x647A63, 0xB0BDDF, 0x702A0D, 0x8, 0x2400],
        R2modp: [0x8EE63D, 0x721FDE, 0xCC0891, 0x10C28B, 0xD4F5A, 0x4C18FB, 0x9036FA, 0x3F845F, 0xA507E4, 0x78EB29, 0x1587],
        MConst: 0x789E85,
        Fra: [0xC80EA3, 0x83355, 0x215BD9, 0xF173F8, 0x677326, 0x189868, 0x8AACA7, 0xAFE18B, 0x3A0164, 0x82FA6, 0x1359],
        Frb: [0x534710, 0x1BBC06, 0xC0628D, 0x269546, 0xD863C7, 0x4E3ABB, 0xD9CDBC, 0xDC53, 0x3628A9, 0xF7D062, 0x10A6],
    };

    return ROM_FIELD_BN254CX;
};

ROM_FIELD_BRAINPOOL = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_BRAINPOOL = {

        // Brainpool modulus
        // Base Bits= 24
        Modulus: [0x6E5377, 0x481D1F, 0x282013, 0xD52620, 0x3BF623, 0x8D726E, 0x909D83, 0x3E660A, 0xEEA9BC, 0x57DBA1, 0xA9FB],
        R2modp: [0x35B819, 0xB03428, 0xECAF0F, 0x3854A4, 0x4A0ED5, 0x2421EA, 0xAA562C, 0xF9C45, 0xDDAE58, 0x4350FD, 0x52B8],
        MConst: 0xFD89B9,

    };
    return ROM_FIELD_BRAINPOOL;
};

ROM_FIELD_C41417 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */


    var ROM_FIELD_C41417 = {

        // C41417 modulus
        // Base Bits= 2
        Modulus: [0x3FFFEF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFFF, 0x3FFFF],
        R2modp: [0x12100, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0],
        MConst: 0x11,
    };
    return ROM_FIELD_C41417;
};

ROM_FIELD_GOLDILOCKS = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_GOLDILOCKS = {

        // GOLDILOCKS modulus
        // Base Bits= 23
        Modulus: [0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7DFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FF],
        R2modp: [0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC0000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0],
        MConst: 0x1,
    };
    return ROM_FIELD_GOLDILOCKS;
};

ROM_FIELD_HIFIVE = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_HIFIVE = {

        // HIFIVE modulus
        // Base Bits= 23
        Modulus: [0x7FFFFD, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x3FFF],
        R2modp: [0x240000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0],
        MConst: 0x3,
    };
    return ROM_FIELD_HIFIVE;
};

ROM_FIELD_NIST256 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_NIST256 = {

        // NIST256 Modulus
        // Base Bits= 24
        Modulus: [0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x1, 0xFFFF00, 0xFFFF],
        R2modp: [0x30000, 0x0, 0x0, 0xFFFF00, 0xFBFFFF, 0xFFFFFF, 0xFFFFFE, 0xFFFFFF, 0xFDFFFF, 0xFFFFFF, 0x4],
        MConst: 0x1,

    };
    return ROM_FIELD_NIST256;
};


ROM_FIELD_SM2 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_SM2 = {

        // SM2 Modulus
        // Base Bits= 24
			Modulus: [0xFFFFFF,0xFFFFFF,0xFFFF,0x0,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFEFF,0xFFFF],
			R2modp: [0x30000,0x0,0x2,0xFFFF00,0x2FFFF,0x0,0x1,0x100,0x20000,0x0,0x4],
			MConst: 0x1,

    };
    return ROM_FIELD_SM2;
};


ROM_FIELD_NIST384 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_NIST384 = {

        // NIST384 modulus
        // Base Bits= 23
        Modulus: [0x7FFFFF, 0x1FF, 0x0, 0x0, 0x7FFFF0, 0x7FDFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0xFFFF],
        R2modp: [0x4000, 0x0, 0x7FFFFE, 0x1FF, 0x80000, 0x0, 0x0, 0x7FC000, 0x3FFFFF, 0x0, 0x200, 0x20000, 0x0, 0x0, 0x0, 0x0, 0x0],
        MConst: 0x1,

    };
    return ROM_FIELD_NIST384;
};

ROM_FIELD_NIST521 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_NIST521 = {

        // NIST521 modulus
        // Base Bits= 23
        Modulus: [0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFFFF, 0x7FFF],
        R2modp: [0x10000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0],
        MConst: 0x1,
    };
    return ROM_FIELD_NIST521;
};


ROM_FIELD_SECP256K1 = function() {
    "use strict";
    /* Fixed Data in ROM - Field and Curve parameters */

    var ROM_FIELD_SECP256K1 = {

        // SECP256K1 modulus
        // Base Bits= 24
        Modulus: [0xFFFC2F,0xFFFEFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFF],
        R2modp: [0xA10000,0xE90,0x7A2,0x100,0x0,0x0,0x0,0x0,0x0,0x0,0x0],
        MConst: 0x253531,
    };
    return ROM_FIELD_SECP256K1;
};
