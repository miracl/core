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
#include "arch.h"
#include "ecp_BLS48286.h"

namespace BLS48286 {
/* Curve BLS48286 - Pairing friendly BLS curve */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B288_29;

// Base Bits= 29

const int CURVE_Cof_I= 62958;
const BIG CURVE_Cof= {0xF5EE,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const int CURVE_B_I= 10;
const BIG CURVE_B= {0xA,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG CURVE_Order= {0x1D612C81,0x1E208D97,0x1CA5C07F,0x11E51045,0x1E4FE229,0x13154A7D,0x127B79AC,0x1602A6BF,0x86BC9E,0x0};
const BIG CURVE_Gx= {0x95D59E0,0x44BF518,0x1BEE8577,0x1B2E8EB9,0xE84D19D,0xA23A0D2,0x103C1301,0x111F68ED,0x13D120DB,0x34B921};
const BIG CURVE_Gy= {0x10ABEB43,0x177B4FCB,0x38F8BB,0x6F9CD6B,0x1B0AC1F2,0x1846F729,0x1E5E6A8C,0x5AF112F,0xA1E656E,0x101C720};
const BIG CURVE_HTPC= {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

const BIG CURVE_Bnx= {0xF5EF,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG CURVE_Pxaaa= {0x56106B5,0x20B1A4C,0x113303FA,0x187639D9,0x1D9F51AC,0xA96D657,0x132BADEA,0x1A544F60,0x531C71,0x11567EA};
const BIG CURVE_Pxaab= {0x96D1A55,0xF668DEC,0xE2476A7,0x4EE30AA,0x1EF56B94,0x5192C97,0x121CF932,0x14B1A4A1,0x196B5C85,0xC6DDCC};
const BIG CURVE_Pxaba= {0x7D7A35E,0x408D1D3,0x1162980E,0x1F5E9D19,0x12AF41C9,0xCD2835A,0x17875370,0x62F5C57,0x15DE0A7C,0x541496};
const BIG CURVE_Pxabb= {0x2F023E8,0x14742906,0xCF4B428,0x3236B9E,0x1B62D89F,0x192E7AEF,0x1E4BA079,0x19B17763,0x155FAC94,0x1015F06};
const BIG CURVE_Pxbaa= {0xF14CB9B,0x5CC6ADC,0x18955362,0x142FAC5F,0x11B96A21,0x18F7B6AA,0x96591F1,0xC5096A4,0x9A1E3F7,0xBA939B};
const BIG CURVE_Pxbab= {0xB60DD54,0x1FB59436,0x1C68910E,0x10A65726,0x6BE9C0B,0x1C6AA6BF,0x14DD8358,0x1E68D885,0x85E9D59,0x880F52};
const BIG CURVE_Pxbba= {0x19D776DA,0x1052D855,0x844D7E8,0xCAE4EC2,0x4ADABAA,0x120A2C5F,0x192AB537,0x3F9DEC1,0xE261FE4,0x7B037E};
const BIG CURVE_Pxbbb= {0x8B50A8E,0x19FF11C1,0x18273AD7,0xEAA9DE4,0x1EC9A57,0x1670C91D,0x300009C,0xACD8A2D,0x6920D19,0xF85987};
const BIG CURVE_Pyaaa= {0x7E988BE,0x1366AFEB,0x5E2F3B8,0x1FCBDF1D,0x114E8B31,0x1CD92A27,0x1A53FC0,0xAAF8FE7,0xDA962CD,0x10F87C6};
const BIG CURVE_Pyaab= {0xDF11B92,0x1BC2B368,0x1AF17821,0x6663003,0xDB40CB8,0x20094F8,0x1856186E,0x1528744B,0x1EC6FED7,0x700E0F};
const BIG CURVE_Pyaba= {0xBFDD06A,0x3E7D33D,0x13F96495,0x179B5611,0x12EB8E64,0xD7054C6,0xD7BFC95,0x145B3D76,0x18C3D6BE,0x9607FD};
const BIG CURVE_Pyabb= {0x188A47F4,0x19EEFE9B,0x41ACCA2,0x12BF346C,0x11F157D1,0x19429C28,0x14D4ACD2,0x743BB7B,0x55BDFFF,0xAB418F};
const BIG CURVE_Pybaa= {0x5E39E77,0x1B76DB9A,0x54E3132,0x190321FF,0x5119324,0x123E6F79,0x1064FB7D,0x1D69EBDE,0xAA01884,0x39C720};
const BIG CURVE_Pybab= {0x164975C6,0x194140A1,0xA2302C0,0x1C495B39,0x613723B,0xB4D86D7,0x1A6F8686,0x7238A06,0x6896E24,0x9B2C3B};
const BIG CURVE_Pybba= {0x1BF2D7C,0x1E99CD5E,0xEA89102,0xE27DE36,0x13837F1B,0xA2D04CE,0x23E06D8,0x18BDC695,0x175143C9,0xFD8541};
const BIG CURVE_Pybbb= {0x51130A6,0x154D068F,0x18954F6A,0x173CE106,0x7C687EE,0xD23B3B5,0xB58CC3B,0x1342EEF6,0xDC6AC9E,0xC9F9BC};


#endif

#if CHUNK==64

using namespace B288_60;

// Base Bits= 60

const int CURVE_Cof_I= 62958;
const BIG CURVE_Cof= {0xF5EEL,0x0L,0x0L,0x0L,0x0L};
const int CURVE_B_I= 10;
const BIG CURVE_B= {0xAL,0x0L,0x0L,0x0L,0x0L};
const BIG CURVE_Order= {0xFC411B2FD612C81L,0x98F28822F29701FL,0xB262A94FBE4FE22L,0x9EB01535FC9EDE6L,0x86BCL};
const BIG CURVE_Gx= {0xC897EA3095D59E0L,0xDD97475CEFBA15DL,0x544741A4E84D19L,0xDB88FB476C0F04CL,0x6972433D120L};
const BIG CURVE_Gy= {0xEEF69F970ABEB43L,0x237CE6B580E3E2EL,0x3308DEE53B0AC1FL,0x6E2D78897F979AAL,0x2038E40A1E65L};
const BIG CURVE_HTPC= {0x1L,0x0L,0x0L,0x0L,0x0L};

const BIG CURVE_Bnx= {0xF5EFL,0x0L,0x0L,0x0L,0x0L};
const BIG CURVE_Pxaaa= {0x8416349856106B5L,0xCC3B1CECC4CC0FEL,0xA952DACAFD9F51AL,0x71D2A27B04CAEB7L,0x22ACFD40531CL};
const BIG CURVE_Pxaab= {0xDECD1BD896D1A55L,0x427718553891DA9L,0xC8A32592FEF56B9L,0x85A58D250C873E4L,0x18DBB9996B5CL};
const BIG CURVE_Pxaba= {0x8811A3A67D7A35EL,0x9FAF4E8CC58A603L,0xC19A506B52AF41CL,0x7C317AE2BDE1D4DL,0xA8292D5DE0AL};
const BIG CURVE_Pxabb= {0x28E8520C2F023E8L,0xF191B5CF33D2D0AL,0xE725CF5DFB62D89L,0x94CD8BBB1F92E81L,0x202BE0D55FACL};
const BIG CURVE_Pxbaa= {0x8B98D5B8F14CB9BL,0x1A17D62FE2554D8L,0xC71EF6D551B96A2L,0xF76284B52259647L,0x17527369A1E3L};
const BIG CURVE_Pxbab= {0xBF6B286CB60DD54L,0xB8532B9371A2443L,0x638D54D7E6BE9C0L,0x59F346C42D3760DL,0x1101EA485E9DL};
const BIG CURVE_Pxbba= {0x20A5B0AB9D776DAL,0xA657276121135FAL,0xDE41458BE4ADABAL,0xE41FCEF60E4AAD4L,0xF606FCE261FL};
const BIG CURVE_Pxbbb= {0xF3FE23828B50A8EL,0x77554EF2609CEB5L,0x72CE1923A1EC9A5L,0x19566C5168C0002L,0x1F0B30E6920DL};
const BIG CURVE_Pyaaa= {0x26CD5FD67E988BEL,0x1FE5EF8E978BCEEL,0x39B2544F14E8B3L,0xCD557C7F38694FFL,0x21F0F8CDA962L};
const BIG CURVE_Pyaab= {0x778566D0DF11B92L,0x83331801EBC5E08L,0xB840129F0DB40CBL,0xD7A943A25E15861L,0xE01C1FEC6FEL};
const BIG CURVE_Pyaba= {0x47CFA67ABFDD06AL,0x4BCDAB08CFE5925L,0x55AE0A98D2EB8E6L,0xBEA2D9EBB35EFF2L,0x12C0FFB8C3D6L};
const BIG CURVE_Pyabb= {0xB3DDFD3788A47F4L,0x195F9A36106B328L,0x4B28538511F157DL,0xFF3A1DDBDD352B3L,0x156831E55BDFL};
const BIG CURVE_Pybaa= {0xB6EDB7345E39E77L,0x4C8190FF9538C4CL,0xF647CDEF2511932L,0x84EB4F5EF4193EDL,0x738E40AA018L};
const BIG CURVE_Pybab= {0x3282814364975C6L,0xBE24AD9CA88C0B0L,0x1969B0DAE613723L,0x24391C50369BE1AL,0x13658766896EL};
const BIG CURVE_Pybba= {0xBD339ABC1BF2D7CL,0xB713EF1B3AA2440L,0x6145A099D3837F1L,0xC9C5EE34A88F81BL,0x1FB0A8375143L};
const BIG CURVE_Pybbb= {0xAA9A0D1E51130A6L,0xEB9E708362553DAL,0xEDA47676A7C687EL,0x9E9A1777B2D6330L,0x193F378DC6ACL};

#endif

}
