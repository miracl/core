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


/* AES Encryption */
package org.miracl.core;

public class AES {
    int Nk, Nr;
    int mode;
    private int[] fkey = new int[60];
    private int[] rkey = new int[60];
    public byte[] f = new byte[16];


    public static final int ECB = 0;
    public static final int CBC = 1;
    public static final int CFB1 = 2;
    public static final int CFB2 = 3;
    public static final int CFB4 = 5;
    public static final int OFB1 = 14;
    public static final int OFB2 = 15;
    public static final int OFB4 = 17;
    public static final int OFB8 = 21;
    public static final int OFB16 = 29;
    public static final int CTR1 = 30;
    public static final int CTR2 = 31;
    public static final int CTR4 = 33;
    public static final int CTR8 = 37;
    public static final int CTR16 = 45;

    private static final byte[] InCo = {(byte)0xB, (byte)0xD, (byte)0x9, (byte)0xE}; /* Inverse Coefficients */

    public static final int BS = 16; /* Block Size */

    private static final byte[] ptab = {
        (byte)1, (byte)3, (byte)5, (byte)15, (byte)17, (byte)51, (byte)85, (byte)255, (byte)26, (byte)46, (byte)114, (byte)150, (byte)161, (byte)248, (byte)19, (byte)53,
        (byte)95, (byte)225, (byte)56, (byte)72, (byte)216, (byte)115, (byte)149, (byte)164, (byte)247, (byte)2, (byte)6, (byte)10, (byte)30, (byte)34, (byte)102, (byte)170,
        (byte)229, (byte)52, (byte)92, (byte)228, (byte)55, (byte)89, (byte)235, (byte)38, (byte)106, (byte)190, (byte)217, (byte)112, (byte)144, (byte)171, (byte)230, (byte)49,
        (byte)83, (byte)245, (byte)4, (byte)12, (byte)20, (byte)60, (byte)68, (byte)204, (byte)79, (byte)209, (byte)104, (byte)184, (byte)211, (byte)110, (byte)178, (byte)205,
        (byte)76, (byte)212, (byte)103, (byte)169, (byte)224, (byte)59, (byte)77, (byte)215, (byte)98, (byte)166, (byte)241, (byte)8, (byte)24, (byte)40, (byte)120, (byte)136,
        (byte)131, (byte)158, (byte)185, (byte)208, (byte)107, (byte)189, (byte)220, (byte)127, (byte)129, (byte)152, (byte)179, (byte)206, (byte)73, (byte)219, (byte)118, (byte)154,
        (byte)181, (byte)196, (byte)87, (byte)249, (byte)16, (byte)48, (byte)80, (byte)240, (byte)11, (byte)29, (byte)39, (byte)105, (byte)187, (byte)214, (byte)97, (byte)163,
        (byte)254, (byte)25, (byte)43, (byte)125, (byte)135, (byte)146, (byte)173, (byte)236, (byte)47, (byte)113, (byte)147, (byte)174, (byte)233, (byte)32, (byte)96, (byte)160,
        (byte)251, (byte)22, (byte)58, (byte)78, (byte)210, (byte)109, (byte)183, (byte)194, (byte)93, (byte)231, (byte)50, (byte)86, (byte)250, (byte)21, (byte)63, (byte)65,
        (byte)195, (byte)94, (byte)226, (byte)61, (byte)71, (byte)201, (byte)64, (byte)192, (byte)91, (byte)237, (byte)44, (byte)116, (byte)156, (byte)191, (byte)218, (byte)117,
        (byte)159, (byte)186, (byte)213, (byte)100, (byte)172, (byte)239, (byte)42, (byte)126, (byte)130, (byte)157, (byte)188, (byte)223, (byte)122, (byte)142, (byte)137, (byte)128,
        (byte)155, (byte)182, (byte)193, (byte)88, (byte)232, (byte)35, (byte)101, (byte)175, (byte)234, (byte)37, (byte)111, (byte)177, (byte)200, (byte)67, (byte)197, (byte)84,
        (byte)252, (byte)31, (byte)33, (byte)99, (byte)165, (byte)244, (byte)7, (byte)9, (byte)27, (byte)45, (byte)119, (byte)153, (byte)176, (byte)203, (byte)70, (byte)202,
        (byte)69, (byte)207, (byte)74, (byte)222, (byte)121, (byte)139, (byte)134, (byte)145, (byte)168, (byte)227, (byte)62, (byte)66, (byte)198, (byte)81, (byte)243, (byte)14,
        (byte)18, (byte)54, (byte)90, (byte)238, (byte)41, (byte)123, (byte)141, (byte)140, (byte)143, (byte)138, (byte)133, (byte)148, (byte)167, (byte)242, (byte)13, (byte)23,
        (byte)57, (byte)75, (byte)221, (byte)124, (byte)132, (byte)151, (byte)162, (byte)253, (byte)28, (byte)36, (byte)108, (byte)180, (byte)199, (byte)82, (byte)246, (byte)1
    };

    private static final byte[] ltab = {
        (byte)0, (byte)255, (byte)25, (byte)1, (byte)50, (byte)2, (byte)26, (byte)198, (byte)75, (byte)199, (byte)27, (byte)104, (byte)51, (byte)238, (byte)223, (byte)3,
        (byte)100, (byte)4, (byte)224, (byte)14, (byte)52, (byte)141, (byte)129, (byte)239, (byte)76, (byte)113, (byte)8, (byte)200, (byte)248, (byte)105, (byte)28, (byte)193,
        (byte)125, (byte)194, (byte)29, (byte)181, (byte)249, (byte)185, (byte)39, (byte)106, (byte)77, (byte)228, (byte)166, (byte)114, (byte)154, (byte)201, (byte)9, (byte)120,
        (byte)101, (byte)47, (byte)138, (byte)5, (byte)33, (byte)15, (byte)225, (byte)36, (byte)18, (byte)240, (byte)130, (byte)69, (byte)53, (byte)147, (byte)218, (byte)142,
        (byte)150, (byte)143, (byte)219, (byte)189, (byte)54, (byte)208, (byte)206, (byte)148, (byte)19, (byte)92, (byte)210, (byte)241, (byte)64, (byte)70, (byte)131, (byte)56,
        (byte)102, (byte)221, (byte)253, (byte)48, (byte)191, (byte)6, (byte)139, (byte)98, (byte)179, (byte)37, (byte)226, (byte)152, (byte)34, (byte)136, (byte)145, (byte)16,
        (byte)126, (byte)110, (byte)72, (byte)195, (byte)163, (byte)182, (byte)30, (byte)66, (byte)58, (byte)107, (byte)40, (byte)84, (byte)250, (byte)133, (byte)61, (byte)186,
        (byte)43, (byte)121, (byte)10, (byte)21, (byte)155, (byte)159, (byte)94, (byte)202, (byte)78, (byte)212, (byte)172, (byte)229, (byte)243, (byte)115, (byte)167, (byte)87,
        (byte)175, (byte)88, (byte)168, (byte)80, (byte)244, (byte)234, (byte)214, (byte)116, (byte)79, (byte)174, (byte)233, (byte)213, (byte)231, (byte)230, (byte)173, (byte)232,
        (byte)44, (byte)215, (byte)117, (byte)122, (byte)235, (byte)22, (byte)11, (byte)245, (byte)89, (byte)203, (byte)95, (byte)176, (byte)156, (byte)169, (byte)81, (byte)160,
        (byte)127, (byte)12, (byte)246, (byte)111, (byte)23, (byte)196, (byte)73, (byte)236, (byte)216, (byte)67, (byte)31, (byte)45, (byte)164, (byte)118, (byte)123, (byte)183,
        (byte)204, (byte)187, (byte)62, (byte)90, (byte)251, (byte)96, (byte)177, (byte)134, (byte)59, (byte)82, (byte)161, (byte)108, (byte)170, (byte)85, (byte)41, (byte)157,
        (byte)151, (byte)178, (byte)135, (byte)144, (byte)97, (byte)190, (byte)220, (byte)252, (byte)188, (byte)149, (byte)207, (byte)205, (byte)55, (byte)63, (byte)91, (byte)209,
        (byte)83, (byte)57, (byte)132, (byte)60, (byte)65, (byte)162, (byte)109, (byte)71, (byte)20, (byte)42, (byte)158, (byte)93, (byte)86, (byte)242, (byte)211, (byte)171,
        (byte)68, (byte)17, (byte)146, (byte)217, (byte)35, (byte)32, (byte)46, (byte)137, (byte)180, (byte)124, (byte)184, (byte)38, (byte)119, (byte)153, (byte)227, (byte)165,
        (byte)103, (byte)74, (byte)237, (byte)222, (byte)197, (byte)49, (byte)254, (byte)24, (byte)13, (byte)99, (byte)140, (byte)128, (byte)192, (byte)247, (byte)112, (byte)7
    };

    private static final byte[] fbsub = {
        (byte)99, (byte)124, (byte)119, (byte)123, (byte)242, (byte)107, (byte)111, (byte)197, (byte)48, (byte)1, (byte)103, (byte)43, (byte)254, (byte)215, (byte)171, (byte)118,
        (byte)202, (byte)130, (byte)201, (byte)125, (byte)250, (byte)89, (byte)71, (byte)240, (byte)173, (byte)212, (byte)162, (byte)175, (byte)156, (byte)164, (byte)114, (byte)192,
        (byte)183, (byte)253, (byte)147, (byte)38, (byte)54, (byte)63, (byte)247, (byte)204, (byte)52, (byte)165, (byte)229, (byte)241, (byte)113, (byte)216, (byte)49, (byte)21,
        (byte)4, (byte)199, (byte)35, (byte)195, (byte)24, (byte)150, (byte)5, (byte)154, (byte)7, (byte)18, (byte)128, (byte)226, (byte)235, (byte)39, (byte)178, (byte)117,
        (byte)9, (byte)131, (byte)44, (byte)26, (byte)27, (byte)110, (byte)90, (byte)160, (byte)82, (byte)59, (byte)214, (byte)179, (byte)41, (byte)227, (byte)47, (byte)132,
        (byte)83, (byte)209, (byte)0, (byte)237, (byte)32, (byte)252, (byte)177, (byte)91, (byte)106, (byte)203, (byte)190, (byte)57, (byte)74, (byte)76, (byte)88, (byte)207,
        (byte)208, (byte)239, (byte)170, (byte)251, (byte)67, (byte)77, (byte)51, (byte)133, (byte)69, (byte)249, (byte)2, (byte)127, (byte)80, (byte)60, (byte)159, (byte)168,
        (byte)81, (byte)163, (byte)64, (byte)143, (byte)146, (byte)157, (byte)56, (byte)245, (byte)188, (byte)182, (byte)218, (byte)33, (byte)16, (byte)255, (byte)243, (byte)210,
        (byte)205, (byte)12, (byte)19, (byte)236, (byte)95, (byte)151, (byte)68, (byte)23, (byte)196, (byte)167, (byte)126, (byte)61, (byte)100, (byte)93, (byte)25, (byte)115,
        (byte)96, (byte)129, (byte)79, (byte)220, (byte)34, (byte)42, (byte)144, (byte)136, (byte)70, (byte)238, (byte)184, (byte)20, (byte)222, (byte)94, (byte)11, (byte)219,
        (byte)224, (byte)50, (byte)58, (byte)10, (byte)73, (byte)6, (byte)36, (byte)92, (byte)194, (byte)211, (byte)172, (byte)98, (byte)145, (byte)149, (byte)228, (byte)121,
        (byte)231, (byte)200, (byte)55, (byte)109, (byte)141, (byte)213, (byte)78, (byte)169, (byte)108, (byte)86, (byte)244, (byte)234, (byte)101, (byte)122, (byte)174, (byte)8,
        (byte)186, (byte)120, (byte)37, (byte)46, (byte)28, (byte)166, (byte)180, (byte)198, (byte)232, (byte)221, (byte)116, (byte)31, (byte)75, (byte)189, (byte)139, (byte)138,
        (byte)112, (byte)62, (byte)181, (byte)102, (byte)72, (byte)3, (byte)246, (byte)14, (byte)97, (byte)53, (byte)87, (byte)185, (byte)134, (byte)193, (byte)29, (byte)158,
        (byte)225, (byte)248, (byte)152, (byte)17, (byte)105, (byte)217, (byte)142, (byte)148, (byte)155, (byte)30, (byte)135, (byte)233, (byte)206, (byte)85, (byte)40, (byte)223,
        (byte)140, (byte)161, (byte)137, (byte)13, (byte)191, (byte)230, (byte)66, (byte)104, (byte)65, (byte)153, (byte)45, (byte)15, (byte)176, (byte)84, (byte)187, (byte)22
    };

    private static final byte[] rbsub = {
        (byte)82, (byte)9, (byte)106, (byte)213, (byte)48, (byte)54, (byte)165, (byte)56, (byte)191, (byte)64, (byte)163, (byte)158, (byte)129, (byte)243, (byte)215, (byte)251,
        (byte)124, (byte)227, (byte)57, (byte)130, (byte)155, (byte)47, (byte)255, (byte)135, (byte)52, (byte)142, (byte)67, (byte)68, (byte)196, (byte)222, (byte)233, (byte)203,
        (byte)84, (byte)123, (byte)148, (byte)50, (byte)166, (byte)194, (byte)35, (byte)61, (byte)238, (byte)76, (byte)149, (byte)11, (byte)66, (byte)250, (byte)195, (byte)78,
        (byte)8, (byte)46, (byte)161, (byte)102, (byte)40, (byte)217, (byte)36, (byte)178, (byte)118, (byte)91, (byte)162, (byte)73, (byte)109, (byte)139, (byte)209, (byte)37,
        (byte)114, (byte)248, (byte)246, (byte)100, (byte)134, (byte)104, (byte)152, (byte)22, (byte)212, (byte)164, (byte)92, (byte)204, (byte)93, (byte)101, (byte)182, (byte)146,
        (byte)108, (byte)112, (byte)72, (byte)80, (byte)253, (byte)237, (byte)185, (byte)218, (byte)94, (byte)21, (byte)70, (byte)87, (byte)167, (byte)141, (byte)157, (byte)132,
        (byte)144, (byte)216, (byte)171, (byte)0, (byte)140, (byte)188, (byte)211, (byte)10, (byte)247, (byte)228, (byte)88, (byte)5, (byte)184, (byte)179, (byte)69, (byte)6,
        (byte)208, (byte)44, (byte)30, (byte)143, (byte)202, (byte)63, (byte)15, (byte)2, (byte)193, (byte)175, (byte)189, (byte)3, (byte)1, (byte)19, (byte)138, (byte)107,
        (byte)58, (byte)145, (byte)17, (byte)65, (byte)79, (byte)103, (byte)220, (byte)234, (byte)151, (byte)242, (byte)207, (byte)206, (byte)240, (byte)180, (byte)230, (byte)115,
        (byte)150, (byte)172, (byte)116, (byte)34, (byte)231, (byte)173, (byte)53, (byte)133, (byte)226, (byte)249, (byte)55, (byte)232, (byte)28, (byte)117, (byte)223, (byte)110,
        (byte)71, (byte)241, (byte)26, (byte)113, (byte)29, (byte)41, (byte)197, (byte)137, (byte)111, (byte)183, (byte)98, (byte)14, (byte)170, (byte)24, (byte)190, (byte)27,
        (byte)252, (byte)86, (byte)62, (byte)75, (byte)198, (byte)210, (byte)121, (byte)32, (byte)154, (byte)219, (byte)192, (byte)254, (byte)120, (byte)205, (byte)90, (byte)244,
        (byte)31, (byte)221, (byte)168, (byte)51, (byte)136, (byte)7, (byte)199, (byte)49, (byte)177, (byte)18, (byte)16, (byte)89, (byte)39, (byte)128, (byte)236, (byte)95,
        (byte)96, (byte)81, (byte)127, (byte)169, (byte)25, (byte)181, (byte)74, (byte)13, (byte)45, (byte)229, (byte)122, (byte)159, (byte)147, (byte)201, (byte)156, (byte)239,
        (byte)160, (byte)224, (byte)59, (byte)77, (byte)174, (byte)42, (byte)245, (byte)176, (byte)200, (byte)235, (byte)187, (byte)60, (byte)131, (byte)83, (byte)153, (byte)97,
        (byte)23, (byte)43, (byte)4, (byte)126, (byte)186, (byte)119, (byte)214, (byte)38, (byte)225, (byte)105, (byte)20, (byte)99, (byte)85, (byte)33, (byte)12, (byte)125
    };

    private static final byte[] rco =
    {(byte)1, (byte)2, (byte)4, (byte)8, (byte)16, (byte)32, (byte)64, (byte)128, (byte)27, (byte)54, (byte)108, (byte)216, (byte)171, (byte)77, (byte)154, (byte)47};

    private static final int[] ftable = {
        0xa56363c6, 0x847c7cf8, 0x997777ee, 0x8d7b7bf6, 0xdf2f2ff, 0xbd6b6bd6,
        0xb16f6fde, 0x54c5c591, 0x50303060, 0x3010102, 0xa96767ce, 0x7d2b2b56,
        0x19fefee7, 0x62d7d7b5, 0xe6abab4d, 0x9a7676ec, 0x45caca8f, 0x9d82821f,
        0x40c9c989, 0x877d7dfa, 0x15fafaef, 0xeb5959b2, 0xc947478e, 0xbf0f0fb,
        0xecadad41, 0x67d4d4b3, 0xfda2a25f, 0xeaafaf45, 0xbf9c9c23, 0xf7a4a453,
        0x967272e4, 0x5bc0c09b, 0xc2b7b775, 0x1cfdfde1, 0xae93933d, 0x6a26264c,
        0x5a36366c, 0x413f3f7e, 0x2f7f7f5, 0x4fcccc83, 0x5c343468, 0xf4a5a551,
        0x34e5e5d1, 0x8f1f1f9, 0x937171e2, 0x73d8d8ab, 0x53313162, 0x3f15152a,
        0xc040408, 0x52c7c795, 0x65232346, 0x5ec3c39d, 0x28181830, 0xa1969637,
        0xf05050a, 0xb59a9a2f, 0x907070e, 0x36121224, 0x9b80801b, 0x3de2e2df,
        0x26ebebcd, 0x6927274e, 0xcdb2b27f, 0x9f7575ea, 0x1b090912, 0x9e83831d,
        0x742c2c58, 0x2e1a1a34, 0x2d1b1b36, 0xb26e6edc, 0xee5a5ab4, 0xfba0a05b,
        0xf65252a4, 0x4d3b3b76, 0x61d6d6b7, 0xceb3b37d, 0x7b292952, 0x3ee3e3dd,
        0x712f2f5e, 0x97848413, 0xf55353a6, 0x68d1d1b9, 0x0, 0x2cededc1,
        0x60202040, 0x1ffcfce3, 0xc8b1b179, 0xed5b5bb6, 0xbe6a6ad4, 0x46cbcb8d,
        0xd9bebe67, 0x4b393972, 0xde4a4a94, 0xd44c4c98, 0xe85858b0, 0x4acfcf85,
        0x6bd0d0bb, 0x2aefefc5, 0xe5aaaa4f, 0x16fbfbed, 0xc5434386, 0xd74d4d9a,
        0x55333366, 0x94858511, 0xcf45458a, 0x10f9f9e9, 0x6020204, 0x817f7ffe,
        0xf05050a0, 0x443c3c78, 0xba9f9f25, 0xe3a8a84b, 0xf35151a2, 0xfea3a35d,
        0xc0404080, 0x8a8f8f05, 0xad92923f, 0xbc9d9d21, 0x48383870, 0x4f5f5f1,
        0xdfbcbc63, 0xc1b6b677, 0x75dadaaf, 0x63212142, 0x30101020, 0x1affffe5,
        0xef3f3fd, 0x6dd2d2bf, 0x4ccdcd81, 0x140c0c18, 0x35131326, 0x2fececc3,
        0xe15f5fbe, 0xa2979735, 0xcc444488, 0x3917172e, 0x57c4c493, 0xf2a7a755,
        0x827e7efc, 0x473d3d7a, 0xac6464c8, 0xe75d5dba, 0x2b191932, 0x957373e6,
        0xa06060c0, 0x98818119, 0xd14f4f9e, 0x7fdcdca3, 0x66222244, 0x7e2a2a54,
        0xab90903b, 0x8388880b, 0xca46468c, 0x29eeeec7, 0xd3b8b86b, 0x3c141428,
        0x79dedea7, 0xe25e5ebc, 0x1d0b0b16, 0x76dbdbad, 0x3be0e0db, 0x56323264,
        0x4e3a3a74, 0x1e0a0a14, 0xdb494992, 0xa06060c, 0x6c242448, 0xe45c5cb8,
        0x5dc2c29f, 0x6ed3d3bd, 0xefacac43, 0xa66262c4, 0xa8919139, 0xa4959531,
        0x37e4e4d3, 0x8b7979f2, 0x32e7e7d5, 0x43c8c88b, 0x5937376e, 0xb76d6dda,
        0x8c8d8d01, 0x64d5d5b1, 0xd24e4e9c, 0xe0a9a949, 0xb46c6cd8, 0xfa5656ac,
        0x7f4f4f3, 0x25eaeacf, 0xaf6565ca, 0x8e7a7af4, 0xe9aeae47, 0x18080810,
        0xd5baba6f, 0x887878f0, 0x6f25254a, 0x722e2e5c, 0x241c1c38, 0xf1a6a657,
        0xc7b4b473, 0x51c6c697, 0x23e8e8cb, 0x7cdddda1, 0x9c7474e8, 0x211f1f3e,
        0xdd4b4b96, 0xdcbdbd61, 0x868b8b0d, 0x858a8a0f, 0x907070e0, 0x423e3e7c,
        0xc4b5b571, 0xaa6666cc, 0xd8484890, 0x5030306, 0x1f6f6f7, 0x120e0e1c,
        0xa36161c2, 0x5f35356a, 0xf95757ae, 0xd0b9b969, 0x91868617, 0x58c1c199,
        0x271d1d3a, 0xb99e9e27, 0x38e1e1d9, 0x13f8f8eb, 0xb398982b, 0x33111122,
        0xbb6969d2, 0x70d9d9a9, 0x898e8e07, 0xa7949433, 0xb69b9b2d, 0x221e1e3c,
        0x92878715, 0x20e9e9c9, 0x49cece87, 0xff5555aa, 0x78282850, 0x7adfdfa5,
        0x8f8c8c03, 0xf8a1a159, 0x80898909, 0x170d0d1a, 0xdabfbf65, 0x31e6e6d7,
        0xc6424284, 0xb86868d0, 0xc3414182, 0xb0999929, 0x772d2d5a, 0x110f0f1e,
        0xcbb0b07b, 0xfc5454a8, 0xd6bbbb6d, 0x3a16162c
    };

    private static final int[] rtable = {
        0x50a7f451, 0x5365417e, 0xc3a4171a, 0x965e273a, 0xcb6bab3b, 0xf1459d1f,
        0xab58faac, 0x9303e34b, 0x55fa3020, 0xf66d76ad, 0x9176cc88, 0x254c02f5,
        0xfcd7e54f, 0xd7cb2ac5, 0x80443526, 0x8fa362b5, 0x495ab1de, 0x671bba25,
        0x980eea45, 0xe1c0fe5d, 0x2752fc3, 0x12f04c81, 0xa397468d, 0xc6f9d36b,
        0xe75f8f03, 0x959c9215, 0xeb7a6dbf, 0xda595295, 0x2d83bed4, 0xd3217458,
        0x2969e049, 0x44c8c98e, 0x6a89c275, 0x78798ef4, 0x6b3e5899, 0xdd71b927,
        0xb64fe1be, 0x17ad88f0, 0x66ac20c9, 0xb43ace7d, 0x184adf63, 0x82311ae5,
        0x60335197, 0x457f5362, 0xe07764b1, 0x84ae6bbb, 0x1ca081fe, 0x942b08f9,
        0x58684870, 0x19fd458f, 0x876cde94, 0xb7f87b52, 0x23d373ab, 0xe2024b72,
        0x578f1fe3, 0x2aab5566, 0x728ebb2, 0x3c2b52f, 0x9a7bc586, 0xa50837d3,
        0xf2872830, 0xb2a5bf23, 0xba6a0302, 0x5c8216ed, 0x2b1ccf8a, 0x92b479a7,
        0xf0f207f3, 0xa1e2694e, 0xcdf4da65, 0xd5be0506, 0x1f6234d1, 0x8afea6c4,
        0x9d532e34, 0xa055f3a2, 0x32e18a05, 0x75ebf6a4, 0x39ec830b, 0xaaef6040,
        0x69f715e, 0x51106ebd, 0xf98a213e, 0x3d06dd96, 0xae053edd, 0x46bde64d,
        0xb58d5491, 0x55dc471, 0x6fd40604, 0xff155060, 0x24fb9819, 0x97e9bdd6,
        0xcc434089, 0x779ed967, 0xbd42e8b0, 0x888b8907, 0x385b19e7, 0xdbeec879,
        0x470a7ca1, 0xe90f427c, 0xc91e84f8, 0x0, 0x83868009, 0x48ed2b32,
        0xac70111e, 0x4e725a6c, 0xfbff0efd, 0x5638850f, 0x1ed5ae3d, 0x27392d36,
        0x64d90f0a, 0x21a65c68, 0xd1545b9b, 0x3a2e3624, 0xb1670a0c, 0xfe75793,
        0xd296eeb4, 0x9e919b1b, 0x4fc5c080, 0xa220dc61, 0x694b775a, 0x161a121c,
        0xaba93e2, 0xe52aa0c0, 0x43e0223c, 0x1d171b12, 0xb0d090e, 0xadc78bf2,
        0xb9a8b62d, 0xc8a91e14, 0x8519f157, 0x4c0775af, 0xbbdd99ee, 0xfd607fa3,
        0x9f2601f7, 0xbcf5725c, 0xc53b6644, 0x347efb5b, 0x7629438b, 0xdcc623cb,
        0x68fcedb6, 0x63f1e4b8, 0xcadc31d7, 0x10856342, 0x40229713, 0x2011c684,
        0x7d244a85, 0xf83dbbd2, 0x1132f9ae, 0x6da129c7, 0x4b2f9e1d, 0xf330b2dc,
        0xec52860d, 0xd0e3c177, 0x6c16b32b, 0x99b970a9, 0xfa489411, 0x2264e947,
        0xc48cfca8, 0x1a3ff0a0, 0xd82c7d56, 0xef903322, 0xc74e4987, 0xc1d138d9,
        0xfea2ca8c, 0x360bd498, 0xcf81f5a6, 0x28de7aa5, 0x268eb7da, 0xa4bfad3f,
        0xe49d3a2c, 0xd927850, 0x9bcc5f6a, 0x62467e54, 0xc2138df6, 0xe8b8d890,
        0x5ef7392e, 0xf5afc382, 0xbe805d9f, 0x7c93d069, 0xa92dd56f, 0xb31225cf,
        0x3b99acc8, 0xa77d1810, 0x6e639ce8, 0x7bbb3bdb, 0x97826cd, 0xf418596e,
        0x1b79aec, 0xa89a4f83, 0x656e95e6, 0x7ee6ffaa, 0x8cfbc21, 0xe6e815ef,
        0xd99be7ba, 0xce366f4a, 0xd4099fea, 0xd67cb029, 0xafb2a431, 0x31233f2a,
        0x3094a5c6, 0xc066a235, 0x37bc4e74, 0xa6ca82fc, 0xb0d090e0, 0x15d8a733,
        0x4a9804f1, 0xf7daec41, 0xe50cd7f, 0x2ff69117, 0x8dd64d76, 0x4db0ef43,
        0x544daacc, 0xdf0496e4, 0xe3b5d19e, 0x1b886a4c, 0xb81f2cc1, 0x7f516546,
        0x4ea5e9d, 0x5d358c01, 0x737487fa, 0x2e410bfb, 0x5a1d67b3, 0x52d2db92,
        0x335610e9, 0x1347d66d, 0x8c61d79a, 0x7a0ca137, 0x8e14f859, 0x893c13eb,
        0xee27a9ce, 0x35c961b7, 0xede51ce1, 0x3cb1477a, 0x59dfd29c, 0x3f73f255,
        0x79ce1418, 0xbf37c773, 0xeacdf753, 0x5baafd5f, 0x146f3ddf, 0x86db4478,
        0x81f3afca, 0x3ec468b9, 0x2c342438, 0x5f40a3c2, 0x72c31d16, 0xc25e2bc,
        0x8b493c28, 0x41950dff, 0x7101a839, 0xdeb30c08, 0x9ce4b4d8, 0x90c15664,
        0x6184cb7b, 0x70b632d5, 0x745c6c48, 0x4257b8d0
    };


    /* Rotates 32-bit word left by 1, 2 or 3 byte  */

    private static int ROTL8(int x) {
        return (((x) << 8) | ((x) >>> 24));
    }

    private static int ROTL16(int x) {
        return (((x) << 16) | ((x) >>> 16));
    }

    private static int ROTL24(int x) {
        return (((x) << 24) | ((x) >>> 8));
    }

    private static int pack(byte[] b) {
        /* pack bytes into a 32-bit Word */
        return ((((int)b[3]) & 0xff) << 24) | (((int)b[2] & 0xff) << 16) | (((int)b[1] & 0xff) << 8) | ((int)b[0] & 0xff);
    }

    private static byte[] unpack(int a) {
        /* unpack bytes from a word */
        byte [] b = new byte[4];
        b[0] = (byte)(a);
        b[1] = (byte)(a >>> 8);
        b[2] = (byte)(a >>> 16);
        b[3] = (byte)(a >>> 24);
        return b;
    }

    private static byte bmul(byte x, byte y) {
        /* x.y= AntiLog(Log(x) + Log(y)) */

        int ix = ((int)x) & 0xff;
        int iy = ((int)y) & 0xff;
        int lx = ((int)ltab[ix]) & 0xff;
        int ly = ((int)ltab[iy]) & 0xff;
        if (x != 0 && y != 0) return ptab[(lx + ly) % 255];
        else return (byte)0;
    }

    //  if (x && y)

    private static int SubByte(int a) {
        byte [] b = unpack(a);
        b[0] = fbsub[(int)b[0] & 0xff];
        b[1] = fbsub[(int)b[1] & 0xff];
        b[2] = fbsub[(int)b[2] & 0xff];
        b[3] = fbsub[(int)b[3] & 0xff];
        return pack(b);
    }

    private static byte product(int x, int y) {
        /* dot product of two 4-byte arrays */
        byte [] xb;//=new byte[4];
        byte [] yb;//=new byte[4];
        xb = unpack(x);
        yb = unpack(y);

        return (byte)(bmul(xb[0], yb[0]) ^ bmul(xb[1], yb[1]) ^ bmul(xb[2], yb[2]) ^ bmul(xb[3], yb[3]));
    }

    private static int InvMixCol(int x) {
        /* matrix Multiplication */
        int y, m;
        byte [] b = new byte[4];

        m = pack(InCo);
        b[3] = product(m, x);
        m = ROTL24(m);
        b[2] = product(m, x);
        m = ROTL24(m);
        b[1] = product(m, x);
        m = ROTL24(m);
        b[0] = product(m, x);
        y = pack(b);
        return y;
    }

    private static void increment(byte [] f) {
        int i;
        for (i = 0; i < 16; i++) {
            f[i]++;
            if (f[i] != 0) break;
        }
    }

    /* reset cipher */
    public void reset(int m, byte[] iv) {
        /* reset mode, or reset iv */
        mode = m;
        for (int i = 0; i < 16; i++)
            f[i] = 0;
        if (mode != ECB && iv != null)
            for (int i = 0; i < 16; i++)
                f[i] = iv[i];
    }

    public byte[] getreg() {
        byte [] ir = new byte[16];
        for (int i = 0; i < 16; i++) ir[i] = f[i];
        return ir;
    }

    /* Initialise cipher */
    public boolean init(int m, int nk, byte[] key, byte[] iv) {
        /* Key Scheduler. Create expanded encryption key */
        int i, j, k, N, nr;
        int [] CipherKey = new int[8];
        byte [] b = new byte[4];
        nk /= 4;

        if (nk != 4 && nk != 6 && nk != 8) return false;

        nr = 6 + nk;

        Nk = nk; Nr = nr;

        reset(m, iv);
        N = 4 * (nr + 1);

        for (i = j = 0; i < nk; i++, j += 4) {
            for (k = 0; k < 4; k++) b[k] = key[j + k];
            CipherKey[i] = pack(b);
        }
        for (i = 0; i < nk; i++) fkey[i] = CipherKey[i];

        for (j = nk, k = 0; j < N; j += nk, k++) {
            fkey[j] = fkey[j - nk] ^ SubByte(ROTL24(fkey[j - 1])) ^ ((int)rco[k]) & 0xff;
            if (nk <= 6) {
                for (i = 1; i < nk && (i + j) < N; i++)
                    fkey[i + j] = fkey[i + j - nk] ^ fkey[i + j - 1];
            } else {
                for (i = 1; i < 4 && (i + j) < N; i++) {
                    fkey[i + j] = fkey[i + j - nk] ^ fkey[i + j - 1];
                }
                if ((j + 4) < N) {
                    fkey[j + 4] = fkey[j + 4 - nk] ^ SubByte(fkey[j + 3]);
                }
                for (i = 5; i < nk && (i + j) < N; i++) {
                    fkey[i + j] = fkey[i + j - nk] ^ fkey[i + j - 1];
                }
            }
        }

        /* now for the expanded decrypt key in reverse order */

        for (j = 0; j < 4; j++) rkey[j + N - 4] = fkey[j];
        for (i = 4; i < N - 4; i += 4) {
            k = N - 4 - i;
            for (j = 0; j < 4; j++) rkey[k + j] = InvMixCol(fkey[i + j]);
        }
        for (j = N - 4; j < N; j++) rkey[j - N + 4] = fkey[j];
        return true;
    }

    /* Encrypt a single block */
    public void ecb_encrypt(byte[] buff) {
        int i, j, k;
        int t;
        byte [] b = new byte[4];
        int [] p = new int[4];
        int [] q = new int[4];

        for (i = j = 0; i < 4; i++, j += 4) {
            for (k = 0; k < 4; k++) b[k] = buff[j + k];
            p[i] = pack(b);
            p[i] ^= fkey[i];
        }

        k = 4;

        /* State alternates between p and q */
        for (i = 1; i < Nr; i++) {
            q[0] = fkey[k] ^ ftable[p[0] & 0xff] ^
                   ROTL8(ftable[(p[1] >>> 8) & 0xff]) ^
                   ROTL16(ftable[(p[2] >>> 16) & 0xff]) ^
                   ROTL24(ftable[(p[3] >>> 24) & 0xff]);
            q[1] = fkey[k + 1] ^ ftable[p[1] & 0xff] ^
                   ROTL8(ftable[(p[2] >>> 8) & 0xff]) ^
                   ROTL16(ftable[(p[3] >>> 16) & 0xff]) ^
                   ROTL24(ftable[(p[0] >>> 24) & 0xff]);
            q[2] = fkey[k + 2] ^ ftable[p[2] & 0xff] ^
                   ROTL8(ftable[(p[3] >>> 8) & 0xff]) ^
                   ROTL16(ftable[(p[0] >>> 16) & 0xff]) ^
                   ROTL24(ftable[(p[1] >>> 24) & 0xff]);
            q[3] = fkey[k + 3] ^ ftable[p[3] & 0xff] ^
                   ROTL8(ftable[(p[0] >>> 8) & 0xff]) ^
                   ROTL16(ftable[(p[1] >>> 16) & 0xff]) ^
                   ROTL24(ftable[(p[2] >>> 24) & 0xff]);

            k += 4;
            for (j = 0; j < 4; j++) {
                t = p[j]; p[j] = q[j]; q[j] = t;
            }
        }

        /* Last Round */

        q[0] = fkey[k] ^ ((int)fbsub[p[0] & 0xff] & 0xff) ^
               ROTL8((int)fbsub[(p[1] >>> 8) & 0xff] & 0xff) ^
               ROTL16((int)fbsub[(p[2] >>> 16) & 0xff] & 0xff) ^
               ROTL24((int)fbsub[(p[3] >>> 24) & 0xff] & 0xff);

        q[1] = fkey[k + 1] ^ ((int)fbsub[p[1] & 0xff] & 0xff) ^
               ROTL8((int)fbsub[(p[2] >>> 8) & 0xff] & 0xff) ^
               ROTL16((int)fbsub[(p[3] >>> 16) & 0xff] & 0xff) ^
               ROTL24((int)fbsub[(p[0] >>> 24) & 0xff] & 0xff);

        q[2] = fkey[k + 2] ^ ((int)fbsub[p[2] & 0xff] & 0xff) ^
               ROTL8((int)fbsub[(p[3] >>> 8) & 0xff] & 0xff) ^
               ROTL16((int)fbsub[(p[0] >>> 16) & 0xff] & 0xff) ^
               ROTL24((int)fbsub[(p[1] >>> 24) & 0xff] & 0xff);

        q[3] = fkey[k + 3] ^ ((int)fbsub[(p[3]) & 0xff] & 0xff) ^
               ROTL8((int)fbsub[(p[0] >>> 8) & 0xff] & 0xff) ^
               ROTL16((int)fbsub[(p[1] >>> 16) & 0xff] & 0xff) ^
               ROTL24((int)fbsub[(p[2] >>> 24) & 0xff] & 0xff);

        for (i = j = 0; i < 4; i++, j += 4) {
            b = unpack(q[i]);
            for (k = 0; k < 4; k++) buff[j + k] = b[k];
        }
    }

    /* Decrypt a single block */
    public void ecb_decrypt(byte[] buff) {
        int i, j, k;
        int t;
        byte [] b = new byte[4];
        int [] p = new int[4];
        int [] q = new int[4];

        for (i = j = 0; i < 4; i++, j += 4) {
            for (k = 0; k < 4; k++) b[k] = buff[j + k];
            p[i] = pack(b);
            p[i] ^= rkey[i];
        }

        k = 4;

        /* State alternates between p and q */
        for (i = 1; i < Nr; i++) {
            q[0] = rkey[k] ^ rtable[p[0] & 0xff] ^
                   ROTL8(rtable[(p[3] >>> 8) & 0xff]) ^
                   ROTL16(rtable[(p[2] >>> 16) & 0xff]) ^
                   ROTL24(rtable[(p[1] >>> 24) & 0xff]);
            q[1] = rkey[k + 1] ^ rtable[p[1] & 0xff] ^
                   ROTL8(rtable[(p[0] >>> 8) & 0xff]) ^
                   ROTL16(rtable[(p[3] >>> 16) & 0xff]) ^
                   ROTL24(rtable[(p[2] >>> 24) & 0xff]);
            q[2] = rkey[k + 2] ^ rtable[p[2] & 0xff] ^
                   ROTL8(rtable[(p[1] >>> 8) & 0xff]) ^
                   ROTL16(rtable[(p[0] >>> 16) & 0xff]) ^
                   ROTL24(rtable[(p[3] >>> 24) & 0xff]);
            q[3] = rkey[k + 3] ^ rtable[p[3] & 0xff] ^
                   ROTL8(rtable[(p[2] >>> 8) & 0xff]) ^
                   ROTL16(rtable[(p[1] >>> 16) & 0xff]) ^
                   ROTL24(rtable[(p[0] >>> 24) & 0xff]);

            k += 4;
            for (j = 0; j < 4; j++) {
                t = p[j]; p[j] = q[j]; q[j] = t;
            }
        }

        /* Last Round */

        q[0] = rkey[k] ^ ((int)rbsub[p[0] & 0xff] & 0xff) ^
               ROTL8((int)rbsub[(p[3] >>> 8) & 0xff] & 0xff) ^
               ROTL16((int)rbsub[(p[2] >>> 16) & 0xff] & 0xff) ^
               ROTL24((int)rbsub[(p[1] >>> 24) & 0xff] & 0xff);
        q[1] = rkey[k + 1] ^ ((int)rbsub[p[1] & 0xff] & 0xff) ^
               ROTL8((int)rbsub[(p[0] >>> 8) & 0xff] & 0xff) ^
               ROTL16((int)rbsub[(p[3] >>> 16) & 0xff] & 0xff) ^
               ROTL24((int)rbsub[(p[2] >>> 24) & 0xff] & 0xff);
        q[2] = rkey[k + 2] ^ ((int)rbsub[p[2] & 0xff] & 0xff) ^
               ROTL8((int)rbsub[(p[1] >>> 8) & 0xff] & 0xff) ^
               ROTL16((int)rbsub[(p[0] >>> 16) & 0xff] & 0xff) ^
               ROTL24((int)rbsub[(p[3] >>> 24) & 0xff] & 0xff);
        q[3] = rkey[k + 3] ^ ((int)rbsub[p[3] & 0xff] & 0xff) ^
               ROTL8((int)rbsub[(p[2] >>> 8) & 0xff] & 0xff) ^
               ROTL16((int)rbsub[(p[1] >>> 16) & 0xff] & 0xff) ^
               ROTL24((int)rbsub[(p[0] >>> 24) & 0xff] & 0xff);

        for (i = j = 0; i < 4; i++, j += 4) {
            b = unpack(q[i]);
            for (k = 0; k < 4; k++) buff[j + k] = b[k];
        }

    }

    /* Encrypt using selected mode of operation */
    public int encrypt(byte[] buff) {
        int j, bytes;
        byte[] st = new byte[16];
        int fell_off;

// Supported Modes of Operation

        fell_off = 0;
        switch (mode) {
        case ECB:
            ecb_encrypt(buff);
            return 0;
        case CBC:
            for (j = 0; j < 16; j++) buff[j] ^= f[j];
            ecb_encrypt(buff);
            for (j = 0; j < 16; j++) f[j] = buff[j];
            return 0;

        case CFB1:
        case CFB2:
        case CFB4:
            bytes = mode - CFB1 + 1;
            for (j = 0; j < bytes; j++) fell_off = (fell_off << 8) | f[j];
            for (j = 0; j < 16; j++) st[j] = f[j];
            for (j = bytes; j < 16; j++) f[j - bytes] = f[j];
            ecb_encrypt(st);
            for (j = 0; j < bytes; j++) {
                buff[j] ^= st[j];
                f[16 - bytes + j] = buff[j];
            }
            return fell_off;

        case OFB1:
        case OFB2:
        case OFB4:
        case OFB8:
        case OFB16:

            bytes = mode - OFB1 + 1;
            ecb_encrypt(f);
            for (j = 0; j < bytes; j++) buff[j] ^= f[j];
            return 0;

        case CTR1:
        case CTR2:
        case CTR4:
        case CTR8:
        case CTR16:

            bytes = mode - CTR1 + 1;
            for (j = 0; j < 16; j++) st[j] = f[j];
            ecb_encrypt(st);
            for (j = 0; j < bytes; j++) buff[j] ^= st[j];
            increment(f);

        default:
            return 0;
        }
    }

    /* Decrypt using selected mode of operation */
    public int decrypt(byte[] buff) {
        int j, bytes;
        byte[] st = new byte[16];
        int fell_off;

        // Supported modes of operation
        fell_off = 0;
        switch (mode) {
        case ECB:
            ecb_decrypt(buff);
            return 0;
        case CBC:
            for (j = 0; j < 16; j++) {
                st[j] = f[j];
                f[j] = buff[j];
            }
            ecb_decrypt(buff);
            for (j = 0; j < 16; j++) {
                buff[j] ^= st[j];
                st[j] = 0;
            }
            return 0;
        case CFB1:
        case CFB2:
        case CFB4:
            bytes = mode - CFB1 + 1;
            for (j = 0; j < bytes; j++) fell_off = (fell_off << 8) | f[j];
            for (j = 0; j < 16; j++) st[j] = f[j];
            for (j = bytes; j < 16; j++) f[j - bytes] = f[j];
            ecb_encrypt(st);
            for (j = 0; j < bytes; j++) {
                f[16 - bytes + j] = buff[j];
                buff[j] ^= st[j];
            }
            return fell_off;
        case OFB1:
        case OFB2:
        case OFB4:
        case OFB8:
        case OFB16:
            bytes = mode - OFB1 + 1;
            ecb_encrypt(f);
            for (j = 0; j < bytes; j++) buff[j] ^= f[j];
            return 0;

        case CTR1:
        case CTR2:
        case CTR4:
        case CTR8:
        case CTR16:

            bytes = mode - CTR1 + 1;
            for (j = 0; j < 16; j++) st[j] = f[j];
            ecb_encrypt(st);
            for (j = 0; j < bytes; j++) buff[j] ^= st[j];
            increment(f);

        default:
            return 0;
        }
    }

    /* Clean up and delete left-overs */
    public void end() {
        // clean up
        int i;
        for (i = 0; i < 4 * (Nr + 1); i++)
            fkey[i] = rkey[i] = 0;
        for (i = 0; i < 16; i++)
            f[i] = 0;
    }

    /* AES encryption/decryption. Encrypt byte array M using key K and returns ciphertext */
    public static byte[] CBC_IV0_ENCRYPT(byte[] K, byte[] M) {
        /* AES CBC encryption, with Null IV and key K */
        /* Input is from an octet string M, output is to an octet string C */
        /* Input is padded as necessary to make up a full final block */
        AES a = new AES();
        boolean fin;
        int i, j, ipt, opt;
        byte[] buff = new byte[16];
        int clen = 16 + (M.length / 16) * 16;

        byte[] C = new byte[clen];
        int padlen;

        a.init(AES.CBC, K.length, K, null);

        ipt = opt = 0;
        fin = false;
        for (;;) {
            for (i = 0; i < 16; i++) {
                if (ipt < M.length) buff[i] = M[ipt++];
                else {fin = true; break;}
            }
            if (fin) break;
            a.encrypt(buff);
            for (i = 0; i < 16; i++)
                C[opt++] = buff[i];
        }

        /* last block, filled up to i-th index */

        padlen = 16 - i;
        for (j = i; j < 16; j++) buff[j] = (byte)padlen;

        a.encrypt(buff);

        for (i = 0; i < 16; i++)
            C[opt++] = buff[i];
        a.end();
        return C;
    }

    /* returns plaintext if all consistent, else returns null string */
    public static byte[] CBC_IV0_DECRYPT(byte[] K, byte[] C) {
        /* padding is removed */
        AES a = new AES();
        int i, ipt, opt, ch;
        byte[] buff = new byte[16];
        byte[] MM = new byte[C.length];
        boolean fin, bad;
        int padlen;
        ipt = opt = 0;

        a.init(AES.CBC, K.length, K, null);

        if (C.length == 0) return new byte[0];
        ch = C[ipt++];

        fin = false;

        for (;;) {
            for (i = 0; i < 16; i++) {
                buff[i] = (byte)ch;
                if (ipt >= C.length) {fin = true; break;}
                else ch = C[ipt++];
            }
            a.decrypt(buff);
            if (fin) break;
            for (i = 0; i < 16; i++)
                MM[opt++] = buff[i];
        }

        a.end();
        bad = false;
        padlen = buff[15];
        if (i != 15 || padlen < 1 || padlen > 16) bad = true;
        if (padlen >= 2 && padlen <= 16)
            for (i = 16 - padlen; i < 16; i++) if (buff[i] != padlen) bad = true;

        if (!bad) for (i = 0; i < 16 - padlen; i++)
                MM[opt++] = buff[i];

        if (bad) return new byte[0];

        byte[] M = new byte[opt];
        for (i = 0; i < opt; i++) M[i] = MM[i];

        return M;
    }


/*
    public static void main(String[] args) {
        int i;

        byte[] key = new byte[32];
        byte[] block = new byte[16];
        byte[] iv = new byte[16];

        for (i = 0; i < 32; i++) key[i] = 0;
        key[0] = 1;
        for (i = 0; i < 16; i++) iv[i] = (byte)i;
        for (i = 0; i < 16; i++) block[i] = (byte)i;

        AES a = new AES();

        a.init(CTR16, 32, key, iv);
        System.out.println("Plain= ");
        for (i = 0; i < 16; i++)  System.out.format("%02X ", block[i] & 0xff);
        System.out.println("");

        a.encrypt(block);

        System.out.println("Encrypt= ");
        for (i = 0; i < 16; i++)  System.out.format("%02X ", block[i] & 0xff);
        System.out.println("");

        a.reset(CTR16, iv);
        a.decrypt(block);

        System.out.println("Decrypt= ");
        for (i = 0; i < 16; i++)  System.out.format("%02X ", block[i] & 0xff);
        System.out.println("");

        a.end();

    }*/
}
