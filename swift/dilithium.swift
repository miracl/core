/*
 * Copyright (c) 2012-2023 MIRACL UK Ltd.
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

//
// dilithium.swift
//
// Created by Michael Scott on 26/03/2023
//

/* Dilithium API */

public struct DILITHIUM {

//q= 8380417
    static public let DL_LGN:Int = 8
    static public let DL_DEGREE:Int = (1 << DL_LGN)
    static public let DL_PRIME:Int32 = 0x7fe001
    static public let DL_D:Int = 13
    static public let DL_TD:Int = (23-DL_D)

    static public let DL_ONE:UInt32 = 0x3FFE00    // R mod Q
    static public let DL_COMBO:UInt32 = 0xA3FA    // ONE*inv mod Q
    static public let DL_R2MODP:Int32 = 0x2419FF // R^2 mod Q
    static public let DL_ND:Int64 = 0xFC7FDFFF   // 1/(R-Q) mod R

    static public let DL_MAXLG:Int = 19;
    static public let DL_MAXK:Int = 8;     // could reduce these if not using highest security
    static public let DL_MAXL:Int = 7;
    static public let DL_YBYTES:Int = (((DL_MAXLG+1)*DL_DEGREE)/8)

    static public let SK_SIZE_2:Int = (32*3+DL_DEGREE*(4*13+4*3+4*3)/8)
    static public let PK_SIZE_2:Int = ((4*DL_DEGREE*DL_TD)/8+32)
    static public let SIG_SIZE_2:Int = ((DL_DEGREE*4*(17+1))/8+80+4+32)

    static public let SK_SIZE_3:Int = (32*3+DL_DEGREE*(6*13+5*4+6*4)/8)
    static public let PK_SIZE_3:Int = ((6*DL_DEGREE*DL_TD)/8+32)
    static public let SIG_SIZE_3:Int = ((DL_DEGREE*5*(19+1))/8+55+6+32)

    static public let SK_SIZE_5:Int = (32*3+DL_DEGREE*(8*13+7*3+8*3)/8)
    static public let PK_SIZE_5:Int = ((8*DL_DEGREE*DL_TD)/8+32)
    static public let SIG_SIZE_5:Int = ((DL_DEGREE*7*(19+1))/8+75+8+32)

// parameters for each security level
// tau,gamma1,gamma2,K,L,eta,lg(2*eta+1),omega
	static public let PARAMS_2:[Int] = [39,17,88,4,4,2,3,80]
	static public let PARAMS_3:[Int] = [49,19,32,6,5,4,4,55]
	static public let PARAMS_5:[Int] = [60,19,32,8,7,2,3,75]

	static private let roots:[Int32] =  [0x3ffe00,0x64f7,0x581103,0x77f504,0x39e44,0x740119,0x728129,0x71e24,0x1bde2b,0x23e92b,0x7a64ae,0x5ff480,0x2f9a75,0x53db0a,0x2f7a49,0x28e527,0x299658,0xfa070,0x6f65a5,0x36b788,0x777d91,0x6ecaa1,0x27f968,0x5fb37c,0x5f8dd7,0x44fae8,0x6a84f8,0x4ddc99,0x1ad035,0x7f9423,0x3d3201,0x445c5,0x294a67,0x17620,0x2ef4cd,0x35dec5,0x668504,0x49102d,0x5927d5,0x3bbeaf,0x44f586,0x516e7d,0x368a96,0x541e42,0x360400,0x7b4a4e,0x23d69c,0x77a55e,0x65f23e,0x66cad7,0x357e1e,0x458f5a,0x35843f,0x5f3618,0x67745d,0x38738c,0xc63a8,0x81b9a,0xe8f76,0x3b3853,0x3b8534,0x58dc31,0x1f9d54,0x552f2e,0x43e6e6,0x688c82,0x47c1d0,0x51781a,0x69b65e,0x3509ee,0x2135c7,0x67afbc,0x6caf76,0x1d9772,0x419073,0x709cf7,0x4f3281,0x4fb2af,0x4870e1,0x1efca,0x3410f2,0x70de86,0x20c638,0x296e9f,0x5297a4,0x47844c,0x799a6e,0x5a140a,0x75a283,0x6d2114,0x7f863c,0x6be9f8,0x7a0bde,0x1495d4,0x1c4563,0x6a0c63,0x4cdbea,0x40af0,0x7c417,0x2f4588,0xad00,0x6f16bf,0xdcd44,0x3c675a,0x470bcb,0x7fbe7f,0x193948,0x4e49c1,0x24756c,0x7ca7e0,0xb98a1,0x6bc809,0x2e46c,0x49a809,0x3036c2,0x639ff7,0x5b1c94,0x7d2ae1,0x141305,0x147792,0x139e25,0x67b0e1,0x737945,0x69e803,0x51cea3,0x44a79d,0x488058,0x3a97d9,0x1fea93,0x33ff5a,0x2358d4,0x3a41f8,0x4cdf73,0x223dfb,0x5a8ba0,0x498423,0x412f5,0x252587,0x6d04f1,0x359b5d,0x4a28a1,0x4682fd,0x6d9b57,0x4f25df,0xdbe5e,0x1c5e1a,0xde0e6,0xc7f5a,0x78f83,0x67428b,0x7f3705,0x77e6fd,0x75e022,0x503af7,0x1f0084,0x30ef86,0x49997e,0x77dcd7,0x742593,0x4901c3,0x53919,0x4610c,0x5aad42,0x3eb01b,0x3472e7,0x4ce03c,0x1a7cc7,0x31924,0x2b5ee5,0x291199,0x585a3b,0x134d71,0x3de11c,0x130984,0x25f051,0x185a46,0x466519,0x1314be,0x283891,0x49bb91,0x52308a,0x1c853f,0x1d0b4b,0x6fd6a7,0x6b88bf,0x12e11b,0x4d3e3f,0x6a0d30,0x78fde5,0x1406c7,0x327283,0x61ed6f,0x6c5954,0x1d4099,0x590579,0x6ae5ae,0x16e405,0xbdbe7,0x221de8,0x33f8cf,0x779935,0x54aa0d,0x665ff9,0x63b158,0x58711c,0x470c13,0x910d8,0x463e20,0x612659,0x251d8b,0x2573b7,0x7d5c90,0x1ddd98,0x336898,0x2d4bb,0x6d73a8,0x4f4cbf,0x27c1c,0x18aa08,0x2dfd71,0xc5ca5,0x19379a,0x478168,0x646c3e,0x51813d,0x35c539,0x3b0115,0x41dc0,0x21c4f7,0x70fbf5,0x1a35e7,0x7340e,0x795d46,0x1a4cd0,0x645caf,0x1d2668,0x666e99,0x6f0634,0x7be5db,0x455fdc,0x530765,0x5dc1b0,0x7973de,0x5cfd0a,0x2cc93,0x70f806,0x189c2a,0x49c5aa,0x776a51,0x3bcf2c,0x7f234f,0x6b16e0,0x3c15ca,0x155e68,0x72f6b7,0x1e29ce];
	static private let iroots:[Int32] = [0x3ffe00,0x7f7b0a,0x7eafd,0x27cefe,0x78c1dd,0xd5ed8,0xbdee8,0x7c41bd,0x56fada,0x5065b8,0x2c04f7,0x50458c,0x1feb81,0x57b53,0x5bf6d6,0x6401d6,0x7b9a3c,0x42ae00,0x4bde,0x650fcc,0x320368,0x155b09,0x3ae519,0x20522a,0x202c85,0x57e699,0x111560,0x86270,0x492879,0x107a5c,0x703f91,0x5649a9,0x2ab0d3,0x6042ad,0x2703d0,0x445acd,0x44a7ae,0x71508b,0x77c467,0x737c59,0x476c75,0x186ba4,0x20a9e9,0x4a5bc2,0x3a50a7,0x4a61e3,0x19152a,0x19edc3,0x83aa3,0x5c0965,0x495b3,0x49dc01,0x2bc1bf,0x49556b,0x2e7184,0x3aea7b,0x442152,0x26b82c,0x36cfd4,0x195afd,0x4a013c,0x50eb34,0x7e69e1,0x56959a,0x454828,0x375fa9,0x3b3864,0x2e115e,0x15f7fe,0xc66bc,0x182f20,0x6c41dc,0x6b686f,0x6bccfc,0x2b520,0x24c36d,0x1c400a,0x4fa93f,0x3637f8,0x7cfb95,0x1417f8,0x744760,0x33821,0x5b6a95,0x319640,0x66a6b9,0x2182,0x38d436,0x4378a7,0x7212bd,0x10c942,0x7f3301,0x509a79,0x781bea,0x7bd511,0x330417,0x15d39e,0x639a9e,0x6b4a2d,0x5d423,0x13f609,0x59c5,0x12beed,0xa3d7e,0x25cbf7,0x64593,0x385bb5,0x2d485d,0x567162,0x5f19c9,0xf017b,0x4bcf0f,0x7df037,0x376f20,0x302d52,0x30ad80,0xf430a,0x3e4f8e,0x62488f,0x13308b,0x183045,0x5eaa3a,0x4ad613,0x1629a3,0x2e67e7,0x381e31,0x17537f,0x3bf91b,0x61b633,0xce94a,0x6a8199,0x43ca37,0x14c921,0xbcb2,0x4410d5,0x875b0,0x361a57,0x6743d7,0xee7fb,0x7d136e,0x22e2f7,0x66c23,0x221e51,0x2cd89c,0x3a8025,0x3fa26,0x10d9cd,0x197168,0x62b999,0x1b8352,0x659331,0x682bb,0x78abf3,0x65aa1a,0xee40c,0x5e1b0a,0x7bc241,0x44deec,0x4a1ac8,0x2e5ec4,0x1b73c3,0x385e99,0x66a867,0x73835c,0x51e290,0x6735f9,0x7d63e5,0x309342,0x126c59,0x7d0b46,0x4c7769,0x620269,0x28371,0x5a6c4a,0x5ac276,0x1eb9a8,0x39a1e1,0x76cf29,0x38d3ee,0x276ee5,0x1c2ea9,0x198008,0x2b35f4,0x846cc,0x4be732,0x5dc219,0x74041a,0x68fbfc,0x14fa53,0x26da88,0x629f68,0x1386ad,0x1df292,0x4d6d7e,0x6bd93a,0x6e21c,0x15d2d1,0x32a1c2,0x6cfee6,0x145742,0x10095a,0x62d4b6,0x635ac2,0x2daf77,0x362470,0x57a770,0x6ccb43,0x397ae8,0x6785bb,0x59efb0,0x6cd67d,0x41fee5,0x6c9290,0x2785c6,0x56ce68,0x54811c,0x7cc6dd,0x65633a,0x32ffc5,0x4b6d1a,0x412fe6,0x2532bf,0x7b7ef5,0x7aa6e8,0x36de3e,0xbba6e,0x8032a,0x364683,0x4ef07b,0x60df7d,0x2fa50a,0x9ffdf,0x7f904,0xa8fc,0x189d76,0x78507e,0x7360a7,0x71ff1b,0x6381e7,0x7221a3,0x30ba22,0x1244aa,0x395d04,0x35b760,0x4a44a4,0x12db10,0x5aba7a,0x7bcd0c,0x365bde,0x255461,0x5da206,0x33008e,0x459e09,0x5c872d,0x4be0a7,0x5ff56e];

    /* Montgomery stuff */

    static func redc(_ T: UInt64) -> Int32 {
        let m = UInt32(T) * UInt32(DL_ND)
        return Int32((UInt64(m) * UInt64(DL_PRIME) + T) >> 32)
    }

    static func nres(_ x: UInt32) -> Int32 {
        return redc(UInt64(x) * UInt64(DL_R2MODP))
    }

    static func modmul(_ a:  UInt32, _ b: UInt32) -> Int32 {
	    return redc(UInt64(a)*UInt64(b))
    }

// make all elements +ve
    static func poly_pos(_ p: inout [Int32]) {
        for i in 0..<DL_DEGREE {
            p[i]+=(p[i]>>31)&DL_PRIME; 
        }
    }

// NTT code
 
// Important!
// DL_nres(x); DL_ntt(x)
// DL_nres(y); DL_ntt(y)
// z=x*y
// DL_intt(z);
// DL_redc(z);
 
// is equivalent to (note that DL_nres() and DL_redc() cancel out)
 
// DL_ntt(x);
// DL_nres(y); DL_ntt(y);
// z=x*y
// DL_intt(z)
 
// is equivalent to
 
// DL_ntt(x)
// DL_ntt(y)
// z=x*y
// DL_intt(z)
// DL_nres(z)
 
// In all cases z ends up in normal (non-Montgomery) form!
// So the conversion to Montgomery form can be "pushed" through the calculation.
 
// Here DL_intt(z) <- DL_intt(z);DL_nres(z);
// Combining is more efficient
// note that DL_ntt() and DL_intt() are not mutually inverse
 
/* NTT code */
/* Cooley-Tukey NTT */
/* Excess of 2 allowed on input - coefficients must be < 2*PRIME */

    static func ntt(_ x: inout [Int32]) {
	    var len = DL_DEGREE/2
	    let q = DL_PRIME

	    // Make positive
	    poly_pos(&x)
	    var m = 1
	    while m < DL_DEGREE {
		    var start = 0
		    for i in 0..<m {
			    let S = roots[m+i]
			    for j in start..<start+len {
				    let V = modmul(UInt32(x[j+len]), UInt32(S))
				    x[j+len] = x[j] + 2 * q - V
				    x[j] = x[j] + V
			    }
			    start += 2 * len
		    }
		    len /= 2
		    m *= 2
	    }
    }

// Gentleman-Sande INTT
// Excess of 2 allowed on input - coefficients must be < 2*PRIME
// Output fully reduced
    static public let NTTL:Int = 1
 
    static func intt(_ x:inout [Int32]) {
	    var t = 1
	    let q  = DL_PRIME
	    var m = DL_DEGREE/2
	    var n = DL_LGN
	    while m >= 1 {
		    let lim = NTTL >> n
		    n -= 1
		    var k = 0
		    for i in 0..<m {
			    let S = iroots[m+i]
			    for j in k..<k+t {
                    var U:Int32
                    var V:Int32
				    if NTTL > 1 {
					    if m < NTTL && j < k+lim {
						    U = modmul(UInt32(x[j]), DL_ONE)
						    V = modmul(UInt32(x[j+t]), DL_ONE)
					    } else {
						    U = x[j]
						    V = x[j+t]
					    }
				    } else {
					    U = x[j]
					    V = x[j+t]
				    }
				    x[j] = U + V
				    let W = U + Int32(DL_DEGREE/NTTL)*q - V
				    x[j+t] = modmul(UInt32(W), UInt32(S))
			    }
			    k += 2 * t
		    }
		    t *= 2
		    m /= 2
	    }

	    for j in 0..<DL_DEGREE {
		    //fully reduce, DL_nres combined with 1/DEGREE
		    x[j] = modmul(UInt32(x[j]), DL_COMBO)
		    x[j] -= q
		    x[j] += (x[j] >> 31)&q
	    }
    }

    static func nres_it(_ p:inout [Int32]) {
        for i in 0..<DL_DEGREE {
            p[i] = nres(UInt32(p[i]))
        }
    }

    static func redc_it(_ p:inout [Int32]) {
        for i in 0..<DL_DEGREE {
            p[i] = redc(UInt64(p[i]))
        }
    }
// copy polynomial
    static func poly_copy(_ p1:inout [Int32],_ p2: [Int32] ) {
        for i in 0..<DL_DEGREE {
            p1[i] = p2[i]
        }
    }

// copy from small polynomial
    static func poly_scopy(_ p1:inout [Int32],_ p2: [Int8]) {
        for i in 0..<DL_DEGREE {
            p1[i] = Int32(p2[i])
        }
    }

// copy from medium polynomial
    static func poly_mcopy(_ p1:inout [Int32],_ p2: [Int16]) {
        for i in 0..<DL_DEGREE {
            p1[i] = Int32(p2[i])
        }
    }

    static func poly_zero(_ p1:inout [Int32]) {
        for i in 0..<DL_DEGREE {
            p1[i] = 0
        }
    }

    static func poly_negate(_ p1:inout [Int32],_ p2: [Int32]) {
        for i in 0..<DL_DEGREE {
            p1[i] = DL_PRIME-p2[i]
        }
    }

    static func poly_mul(_ p1:inout [Int32],_ p2: [Int32],_ p3: [Int32]) {
        for i in 0..<DL_DEGREE {
            p1[i] = modmul(UInt32(p2[i]), UInt32(p3[i]))
        }
    }

    static func poly_add(_ p1:inout [Int32],_ p2: [Int32],_ p3: [Int32]) {
        for i in 0..<DL_DEGREE {
            p1[i] = (p2[i] + p3[i])
        }
    }

    static func poly_sub(_ p1:inout [Int32],_ p2: [Int32],_ p3: [Int32]) {
        for i in 0..<DL_DEGREE {
            p1[i] = (p2[i] + DL_PRIME - p3[i])
        }
    }

/* reduces inputs that are already < 2q */
    static func poly_soft_reduce(_ poly:inout [Int32]) {
        for i in 0..<DL_DEGREE {
            let e = poly[i] - DL_PRIME
            poly[i] = e + ((e >> 31)&DL_PRIME)
        }
    }

/* fully reduces modulo q */
    static func poly_hard_reduce(_ poly:inout [Int32]) {
        for i in 0..<DL_DEGREE {
            var e = modmul(UInt32(poly[i]), DL_ONE); // reduces to < 2q
            e = e - DL_PRIME
            poly[i] = e + ((e >> 31)&DL_PRIME) // finishes it off
        }
    }

//Generate A[i][j] from rho
    static func ExpandAij(_ rho: [UInt8],_ Aij:inout [Int32],_ i: Int, _ j: Int) {
        var sh=SHA3(SHA3.SHAKE128)
	    var buff=[UInt8](repeating: 0,count: 4*DL_DEGREE) // should be plenty (?)
 
	    for m in 0..<32 {
		    sh.process(rho[m])
	    }
	    sh.process(UInt8(j&0xff))
	    sh.process(UInt8(i&0xff))
	    sh.shake(&buff, 4*DL_DEGREE)
	    var m = 0
	    var n = 0
	    while m < DL_DEGREE {
		    let b0 = UInt32(buff[n])
		    n+=1
		    let b1 = UInt32(buff[n])
		    n+=1
		    let b2 = UInt32(buff[n])
		    n+=1
		    let cf = Int32(((b2 & 0x7f) << 16) + (b1 << 8) + b0)
		    if cf >= DL_PRIME {
			    continue
		    }
		    Aij[m] = cf
		    m+=1
	    }
    }

// array t has ab active bits per word
// extract bytes from array of words
// if max!=0 then -max<=t[i]<=+max
    static func nextbyte32(_ ab: Int,_ max: Int, _ t: [Int32],_ position:inout [Int]) -> UInt8 {
	    var ptr = position[0] // index in array
	    var bts = position[1] // bit index in word
	    var left = ab - bts   // number of bits left in this word
	    var i = 0
	    let k = ptr % 256
	    var w = t[k]

	    if max != 0 {
		    w = Int32(max) - w
	    }
	    var r = w >> bts
	    while left < 8 {
		    i+=1
		    w = t[k+i]
		    if max != 0 {
			    w = Int32(max) - w
		    }
		    r |= w << left
		    left += ab
	    }
	    bts += 8
	    while bts >= ab {
		    bts -= ab
		    ptr+=1
	    }
	    position[0] = ptr
	    position[1] = bts
 
	    return UInt8(r & 0xff)
    }

// array t has ab active bits per word
// extract bytes from array of shorts
// if max!=0 then -max<=t[i]<=+max
    static func nextbyte16(_ ab: Int,_ max: Int, _ t: [Int16],_ position:inout [Int]) -> UInt8 {
	    var ptr = position[0] // index in array
	    var bts = position[1] // bit index in word
	    var left = ab - bts   // number of bits left in this word
	    var i = 0
	    let k = ptr % 256
	    var w = Int32(t[k])

	    if max != 0 {
		    w = Int32(max) - w
	    }
	    var r = w >> bts
	    while left < 8 {
		    i+=1
		    w = Int32(t[k+i])
		    if max != 0 {
			    w = Int32(max) - w
		    }
		    r |= w << left
		    left += ab
	    }
	    bts += 8
	    while bts >= ab {
		    bts -= ab
		    ptr+=1
	    }
	    position[0] = ptr
	    position[1] = bts
 
	    return UInt8(r & 0xff)
    }

// array t has ab active bits per word
// extract bytes from array of bytes
// if max!=0 then -max<=t[i]<=+max
    static func nextbyte8(_ ab: Int,_ max: Int, _ t: [Int8],_ position:inout [Int]) -> UInt8 {
	    var ptr = position[0] // index in array
	    var bts = position[1] // bit index in word
	    var left = ab - bts   // number of bits left in this word
	    var i = 0
	    let k = ptr % 256
	    var w = Int32(t[k])

	    if max != 0 {
		    w = Int32(max) - w
	    }
	    var r = w >> bts
	    while left < 8 {
		    i+=1
		    w = Int32(t[k+i])
		    if max != 0 {
			    w = Int32(max) - w
		    }
		    r |= w << left
		    left += ab
	    }
	    bts += 8
	    while bts >= ab {
		    bts -= ab
		    ptr+=1
	    }
	    position[0] = ptr
	    position[1] = bts
 
	    return UInt8(r & 0xff)
    }

// extract ab bits into word from dense byte stream
    static func nextword(_ ab: Int, _ max: Int,_ t: [UInt8], _ position:inout [Int]) -> Int32 {
	    var ptr = position[0] // index in array
	    var bts = position[1] // bit index in word
	    var r = Int32(t[ptr]>>bts)
	    let mask = Int32((1<<ab)-1)
	    var w:Int32
	    var i = 0
	    var gotbits = 8 - bts // bits left in current byte
	    while gotbits < ab {
		    i+=1
		    w = Int32(t[ptr+i])
		    r |= w<<gotbits
		    gotbits += 8
	    }
	    bts += ab
	    while bts >= 8 {
		    bts -= 8
		    ptr+=1
	    }
	    w = r&mask
	    if max != 0 {
		    w = Int32(max) - w
	    }
	    position[0] = ptr
	    position[1] = bts
	    return w
    }

//pack public key
    static func pack_pk(_ params: [Int],_ pk:inout [UInt8],_ rho: [UInt8],_ t1: [[Int16]]) -> Int {
        var pos=[Int](repeating: 0,count: 2)

	    let ck = params[3]
	    for i in 0..<32 {
		    pk[i] = rho[i]
	    }
	    var n = 32
	    for j in 0..<ck {
		    for _ in 0..<(DL_DEGREE*DL_TD)/8 {
			    pk[n] = nextbyte16(DL_TD, 0, t1[j], &pos)
			    n+=1
		    }
	    }
	    return n
    }

//unpack public key
    static func unpack_pk(_ params: [Int],_ rho:inout [UInt8],_ t1:inout [[Int16]],_ pk: [UInt8]) {
        var pos=[Int](repeating: 0,count: 2)
	    pos[0] = 32
	    let ck = params[3]
	    for i in 0..<32 {
		    rho[i] = pk[i]
	    }
	    for j in 0..<ck {
		    for i in 0..<DL_DEGREE {
			    t1[j][i] = Int16(nextword(DL_TD, 0, pk, &pos))
		    }
	    }
    }

// secret key of size 32*3+DEGREE*(K*D+L*LG2ETA1+K*LG2ETA1)/8
    static func pack_sk(_ params: [Int],_ sk:inout [UInt8],_ rho: [UInt8],_ bK: [UInt8],_ tr: [UInt8],_ s1: [[Int8]],_ s2: [[Int8]],_ t0: [[Int16]]) -> Int {
	    var n = 32
	    let ck = params[3]
	    let el = params[4]
	    let eta = params[5]
	    let lg2eta1 = params[6]
 
	    for i in 0..<32 {
		    sk[i] = rho[i]
	    }
	    for i in 0..<32 {
		    sk[n] = bK[i]
		    n+=1
	    }
	    for i in 0..<32 {
		    sk[n] = tr[i]
		    n+=1
	    }
 
        var pos=[Int](repeating: 0,count: 2)
 
	    for j in 0..<el {
		    for _ in 0..<(DL_DEGREE*lg2eta1)/8 {
			    sk[n] = nextbyte8(lg2eta1, eta, s1[j], &pos)
			    n+=1
		    }
	    }
	    pos[0] = 0; pos[1] = 0
	    for j in 0..<ck {
		    for _ in 0..<(DL_DEGREE*lg2eta1)/8 {
			    sk[n] = nextbyte8(lg2eta1, eta, s2[j], &pos)
			    n+=1
		    }
	    }
	    pos[0] = 0; pos[1] = 0
	    for j in 0..<ck {
		    for _ in 0..<(DL_DEGREE*DL_D)/8 {
			    sk[n] = nextbyte16(DL_D, (1 << (DL_D - 1)), t0[j], &pos)
			    n+=1
		    }
	    }
	    return n
    }

    static func unpack_sk(_ params: [Int],_ rho:inout [UInt8],_ bK:inout [UInt8],_ tr:inout [UInt8],_ s1:inout [[Int8]],_ s2:inout [[Int8]],_ t0:inout [[Int16]],_ sk: [UInt8]) {
	    var n = 32
	    let ck = params[3]
	    let el = params[4]
	    let eta = params[5]
	    let lg2eta1 = params[6]
 
	    for i in 0..<32 {
		    rho[i] = sk[i]
	    }
	    for i in 0..<32 {
		    bK[i] = sk[n]
		    n+=1
	    }
	    for i in 0..<32 {
		    tr[i] = sk[n]
		    n+=1
	    }
 
        var pos=[Int](repeating: 0,count: 2)
        pos[0] = n

	    for j in 0..<el {
		    for i in 0..<DL_DEGREE {
			    s1[j][i] = Int8(nextword(lg2eta1, eta, sk, &pos))
		    }
	    }

	    for j in 0..<ck {
		    for i in 0..<DL_DEGREE {
			    s2[j][i] = Int8(nextword(lg2eta1, eta, sk, &pos))
		    }
	    }
	    for j in 0..<ck {
		    for i in 0..<DL_DEGREE {
			    t0[j][i] = Int16(nextword(DL_D, (1 << (DL_D - 1)), sk, &pos))
            }
		}
	}

// pack signature - change z
    static func pack_sig(_ params: [Int],_ sig:inout [UInt8],_ z:inout  [[Int32]],_ ct: [UInt8],_ h: [UInt8]) -> Int {
	    let lg = params[1]
	    let gamma1 = 1 << lg
	    let ck = params[3]
	    let el = params[4]
	    let omega = params[7]
 
	    for i in 0..<32 {
		    sig[i] = ct[i]
	    }

	    var n = 32
        var pos=[Int](repeating: 0,count: 2)
	//pre-process z
	    for i in 0..<el {
		    for m in 0..<DL_DEGREE {
			    var t = z[i][m]
			    if t > DL_PRIME/2 {
				    t -= DL_PRIME
			    }
			    t = Int32(gamma1) - t
			    z[i][m] = t
		    }
	    }
	    for j in 0..<el {
		    for _ in 0..<(DL_DEGREE*(lg+1))/8 {
			    sig[n] = nextbyte32(lg+1, 0, z[j], &pos)
			    n+=1
		    }
	    }
	    for i in 0..<omega+ck {
		    sig[n] = h[i]
		    n+=1
	    }
	    return n
    }

    static func unpack_sig(_ params: [Int],_ z:inout [[Int32]],_ ct:inout [UInt8],_ h:inout [UInt8],_ sig: [UInt8]) {
	    let lg = params[1]
	    let gamma1 = 1 << lg
	    let ck = params[3]
	    let el = params[4]
	    let omega = params[7]

	    var m = 32 + (el*DL_DEGREE*(lg+1))/8

	    for i in 0..<32 {
		    ct[i] = sig[i]
	    }
 
        var pos=[Int](repeating: 0,count: 2)
        pos[0]=32
	    for j in 0..<el {
		    for i in 0..<DL_DEGREE {
			    var t = nextword(lg+1, 0, sig, &pos)
			    t = Int32(gamma1)-t
			    if t < 0 {
				    t += DL_PRIME
			    }
			    z[j][i] = t
		    }
	    }
	    for i in 0..<omega+ck {
		    h[i] = sig[m]
		    m+=1
	    }
    }

// rejection sampling in range -ETA to +ETA
    static func sample_Sn(_ params: [Int],_ rhod: [UInt8],_ s:inout [Int8],_ n: Int) {
	    let eta = Int8(params[5])
	    let lg2eta1 = params[6]
        var sh=SHA3(SHA3.SHAKE256)
	    var buff=[UInt8](repeating: 0,count: 272) 
 
	    for m in 0..<64 {
		    sh.process(rhod[m])
	    }
	    sh.process(UInt8(n & 0xff))
	    sh.process(UInt8((n >> 8) & 0xff))
	    sh.shake(&buff, 272)
 
        var pos=[Int](repeating: 0,count: 2)
	    for m in 0..<DL_DEGREE {
            repeat {
                s[m] = Int8(nextword(lg2eta1, 0, buff, &pos))
		    } while s[m] > 2*eta
		    s[m] = Int8(eta - s[m])
	    }
    }

// uniform random sampling
    static func sample_Y(_ params: [Int],_ k: Int,_ rhod: [UInt8],_ y:inout [[Int32]]) {
	    let lg = params[1]
	    let gamma1 = 1 << lg
	    let el = params[4]
	    var buff=[UInt8](repeating: 0,count: DL_YBYTES) 
 
	    for i in 0..<el {
		    var sh = SHA3(SHA3.SHAKE256)
		    for j in 0..<64 {
			    sh.process(rhod[j])
		    }
		    let ki = k + i
		    sh.process(UInt8(ki & 0xff))
		    sh.process(UInt8(ki >> 8))
		    sh.shake(&buff, DL_YBYTES)
 
            var pos=[Int](repeating: 0,count: 2)
		
		    for m in 0..<DL_DEGREE {
			    //take in LG+1 bits at a time
			    var w = nextword(lg+1, 0, buff, &pos)
			    w = Int32(gamma1) - w
			    let t = w >> 31
			    y[i][m] = w + (DL_PRIME & t)
		    }
	    }
    }

//CRH(rho,t1)
static func CRH1(_ params: [Int],_ H:inout [UInt8],_ rho: [UInt8],_ t1: [[Int16]]) {
        var pos=[Int](repeating: 0,count: 2)
	    let ck = params[3]
	    var sh = SHA3(SHA3.SHAKE256)
	    for i in 0..<32 {
		    sh.process(rho[i])
	    }
	    for j in 0..<ck {
		    for _ in 0..<(DL_DEGREE*DL_TD)/8 {
			    let nxt = nextbyte16(DL_TD, 0, t1[j], &pos)
			    sh.process(nxt)
		    }
	    }
	    sh.shake(&H, 32)
    }
//CRH(tr,M)
    static func CRH2(_ H:inout [UInt8],_ tr: [UInt8],_ mess: [UInt8],_ mlen: Int) {
	    var sh = SHA3(SHA3.SHAKE256)
	    for i in 0..<32 {
		    sh.process(tr[i])
	    }
	    for i in 0..<mlen {
		    sh.process(mess[i])
	    }
	    sh.shake(&H, 64)
    }
//CRH(K,mu)
static func CRH3(_ H:inout [UInt8],_ bK: [UInt8],_ mu: [UInt8]) {
	    var sh = SHA3(SHA3.SHAKE256)
	    for i in 0..<32 {
		    sh.process(bK[i])
	    }
	    for i in 0..<64 {
		    sh.process(mu[i])
	    }
	    sh.shake(&H, 64)
    }

//H(mu,w1)
    static func H4(_ params: [Int],_ CT:inout [UInt8],_ mu: [UInt8],_ w1: [[Int8]]) {
       var pos=[Int](repeating: 0,count: 2)
	    let ck = params[3]
	    let dv = params[2]
	    var w1b = 4
	    if dv == 88 {
		    w1b = 6
	    }
 
	    var sh = SHA3(SHA3.SHAKE256)
	    for i in 0..<64 {
		    sh.process(mu[i])
	    }
	    for j in 0..<ck {
		    for _ in 0..<(DL_DEGREE*w1b)/8 {
			    sh.process(nextbyte8(w1b, 0, w1[j], &pos))
		    }
	    }
	    sh.shake(&CT, 32)
    }

    static func SampleInBall(_ params: [Int],_ ct: [UInt8],_ c:inout [Int32]) {
	    let tau = params[0]
        var signs=[UInt8](repeating: 0,count: 8) 
        var buff=[UInt8](repeating: 0,count: 136) 
	    var sh = SHA3(SHA3.SHAKE256)
 
	    for i in 0..<32 {
		    sh.process(ct[i])
	    }
	    sh.shake(&buff, 136)
 
	    for i in 0..<8 {
		    signs[i] = buff[i]
	    }
 
	    var k = 8
	    var b = 0
	    poly_zero(&c)
	    var sn = Int(signs[0])
	    var n = 1
 
	    for i in DL_DEGREE - tau..<DL_DEGREE {
            var j:Int
            repeat {
                j=Int(buff[k])
                k+=1
            } while j>i
		    c[i] = c[j]
		    c[j] = 1 - 2*(Int32(sn&1))
		    sn >>= 1
		    b+=1
		    if b == 8 {
	    		sn = Int(signs[n])
		    	n+=1
			    b = 0
		    }
	    }
    }
    static func Power2Round(_ t: [Int32],_ t0:inout [Int16],_ t1:inout [Int16]) {
	    for m in 0..<DL_DEGREE {
		    var w = t[m]
		    let d = Int32(1 << DL_D)
		    let r = (w + d/2 - 1) >> DL_D
		    w -= r << DL_D
		    t1[m] = Int16(r)
		    t0[m] = Int16(w)
	    }
    }

// ALPHA = (Q-1)/16 - borrowed from dilithium ref implementation
    static func decompose_lo(_ params: [Int],_ a: Int32) -> Int32 {
	    var gamma2:Int32
	    let dv = params[2]

	    var a1 = (a + 127) >> 7
	    if dv == 32 {
		    a1 = (a1*1025 + (1 << 21)) >> 22
		    a1 &= 15
		    gamma2 = (DL_PRIME - 1) / 32
	    } else { //88
		    a1 = (a1*11275 + (1 << 23)) >> 24
		    a1 ^= ((43 - a1) >> 31) & a1
		    gamma2 = (DL_PRIME - 1) / 88
	    }
 
	    var a0 = a - a1 * 2 * Int32(gamma2)
	    a0 -= (((DL_PRIME-1)/2 - a0) >> 31) & DL_PRIME
	    a0 += (a0 >> 31) & DL_PRIME
 
	    return a0
    }

// ALPHA = (Q-1)/16
    static func decompose_hi(_ params: [Int],_ a: Int32) -> Int8 {
	    let dv = params[2]
	    var a1 = (a + 127) >> 7

	    if dv == 32 {
		    a1 = (a1*1025 + (1 << 21)) >> 22
		    a1 &= 15
	    } else {
		    a1 = (a1*11275 + (1 << 23)) >> 24
	    	a1 ^= ((43 - a1) >> 31) & a1
	    }
	    return Int8(a1)
    }

    static func lobits(_ params: [Int],_ r0:inout [Int32],_ r: [Int32]) {
	    for m in 0..<DL_DEGREE {
		    r0[m] = decompose_lo(params, r[m])
	    }
    }
 
    static func hibits(_ params: [Int],_ r1:inout [Int8],_ r: [Int32]) {
	    for m in 0..<DL_DEGREE {
		    r1[m] = decompose_hi(params, r[m])
	    }
    }

// before h initialised to zeros, hptr=0
// after new hptr returned and h[OMEGA+i]= hptr
    static func MakePartialHint(_ params: [Int],_ h:inout [UInt8],_ hptr: Int,_ z: [Int32],_ r: [Int32]) -> Int {
	    let omega = params[7]
        var ptr = hptr
	    for m in 0..<DL_DEGREE {
		    let a0 = decompose_hi(params, r[m])
		    var rz = r[m] + z[m]
		    rz -= DL_PRIME
		    rz = rz + ((rz >> 31) & DL_PRIME)
		    let a1 = decompose_hi(params, rz)
		    if a0 != a1 {
			    if ptr >= omega {
				    return omega + 1
			    }
			    h[ptr] = UInt8(m&0xff)
			    ptr+=1
		    }
	    }
	    return ptr
    }

    static func UsePartialHint(_ params: [Int],_ r:inout [Int8],_ h: [UInt8],_ hptr: Int,_ i: Int,_ w: [Int32]) -> Int {
	    let dv = Int8(params[2])
	    let omega = params[7]
	    let md = Int8(dv / 2)
        var ptr = hptr

	    for m in 0..<DL_DEGREE {
		    var a1 = decompose_hi(params, w[m])
		    if m == Int(h[ptr]) && ptr < Int(h[omega+i]) { 
			    ptr += 1
			    let a0 = decompose_lo(params, w[m])
			    if a0 <= DL_PRIME/2 {
				    a1+=1
				    if a1 >= md {
					    a1 -= md
				    }
			    } else {
				    a1-=1
				    if a1 < 0 {
					    a1 += md
				    }
			    }
		    }
		    r[m] = a1
	    }
	    return ptr
    }

    static func infinity_norm(_ w: [Int32]) -> Int32 {
	    var n:Int32 = 0
	    for m in 0..<DL_DEGREE {
		    var az = w[m]
		    if az > (DL_PRIME/2) {
			    az = DL_PRIME - az
		    }
		    if az > n {
			    n = az
		    }
	    }
	    return n
    }

// Dilithium API
// input entropy, output key pair
    static func keypair(_ params:[Int],_ tau:[UInt8],_ sk:inout [UInt8],_ pk:inout [UInt8]) {
        var buff=[UInt8](repeating: 0,count: 128) 
        var rho=[UInt8](repeating: 0,count: 32)
        var rhod=[UInt8](repeating: 0,count: 64)
        var bK=[UInt8](repeating: 0,count: 32)
        var tr=[UInt8](repeating: 0,count: 32)
        var Aij=[Int32](repeating: 0,count: DL_DEGREE)         
        var w=[Int32](repeating: 0,count: DL_DEGREE)         
        var r=[Int32](repeating: 0,count: DL_DEGREE)    
        
        let ck=params[3]
        let el=params[4]

        var s1 = [[Int8]](repeating: [Int8](repeating: 0,count: DL_DEGREE), count: el)
        var s2 = [[Int8]](repeating: [Int8](repeating: 0,count: DL_DEGREE), count: ck)
        var t0 = [[Int16]](repeating: [Int16](repeating: 0,count: DL_DEGREE), count: ck)
        var t1 = [[Int16]](repeating: [Int16](repeating: 0,count: DL_DEGREE), count: ck)

	    var sh = SHA3(SHA3.SHAKE256)

        for i in 0..<32 {
            sh.process(tau[i])
        }
        sh.shake(&buff,128)
        for i in 0..<32 {
            rho[i]=buff[i]
            bK[i]=buff[i+96]
        }
        for i in 0..<64 {
            rhod[i]=buff[32+i]
        }
        for i in 0..<el {
            sample_Sn(params,rhod,&s1[i],i);
        }
        for i in 0..<ck {
            sample_Sn(params,rhod,&s2[i],el+i)
            poly_zero(&r)
            for j in 0..<el {
                poly_scopy(&w,s1[j])
                ntt(&w)
                ExpandAij(rho,&Aij,i,j)
                poly_mul(&w,w,Aij)
                poly_add(&r,r,w)
            }
            poly_hard_reduce(&r)
            intt(&r)
            poly_scopy(&w,s2[i])
            poly_pos(&w)
            poly_add(&r,r,w)
            poly_soft_reduce(&r)
            Power2Round(r,&t0[i],&t1[i])
        }
        CRH1(params,&tr,rho,t1);
        _=pack_pk(params,&pk,rho,t1)
        _=pack_sk(params,&sk,rho,bK,tr,s1,s2,t0)
    }

    static func signature(_ params:[Int],_ sk: [UInt8],_ M: [UInt8],_ sig:inout [UInt8]) -> Int {
        var rho=[UInt8](repeating: 0,count: 32)
        var rhod=[UInt8](repeating: 0,count: 64)
        var bK=[UInt8](repeating: 0,count: 32)
        var tr=[UInt8](repeating: 0,count: 32)
        var ct=[UInt8](repeating: 0,count: 32)
        var mu=[UInt8](repeating: 0,count: 64)
        var hint=[UInt8](repeating: 0,count: 100)

        var c=[Int32](repeating: 0,count: DL_DEGREE)
        var w=[Int32](repeating: 0,count: DL_DEGREE)
        var r=[Int32](repeating: 0,count: DL_DEGREE)

	    let ck = params[3]
	    let el = params[4]
 
        var s1 = [[Int8]](repeating: [Int8](repeating: 0,count: DL_DEGREE), count: el)
        var s2 = [[Int8]](repeating: [Int8](repeating: 0,count: DL_DEGREE), count: ck)
        var t0 = [[Int16]](repeating: [Int16](repeating: 0,count: DL_DEGREE), count: ck)
        var y = [[Int32]](repeating: [Int32](repeating: 0,count: DL_DEGREE), count: el)
        var Ay = [[Int32]](repeating: [Int32](repeating: 0,count: DL_DEGREE), count: ck)
        var w1 = [[Int8]](repeating: [Int8](repeating: 0,count: DL_DEGREE), count: ck)

	    let tau = params[0]
	    let lg = params[1]
	    let gamma1 = Int32(1 << lg)
	    let dv = Int32(params[2])
	    let gamma2 = (DL_PRIME - 1) / dv
	    let eta = params[5]
	    let beta = Int32(tau * eta)
	    let omega = params[7]
	    unpack_sk(params, &rho, &bK, &tr, &s1, &s2, &t0, sk)

	//signature
	    CRH2(&mu, tr, M, M.count)
	    CRH3(&rhod, bK, mu)
        
        var k=0
        while true {
            let fk=k*el; k+=1
            sample_Y(params,fk,rhod,&y)
            for i in 0..<el {
                ntt(&y[i])
            }
            for i in 0..<ck {
                poly_zero(&r)
                for j in 0..<el {
                    poly_copy(&w,y[j])
                    ExpandAij(rho,&c,i,j)
                    poly_mul(&w,w,c)
                    poly_add(&r,r,w)
                }
                poly_hard_reduce(&r)
                intt(&r)
                poly_copy(&Ay[i],r)
                hibits(params,&w1[i],Ay[i])
            }
            H4(params,&ct,mu,w1)
            SampleInBall(params,ct,&c)

            var badone=false
            ntt(&c)
            for i in 0..<el {
                poly_scopy(&w,s1[i])
                ntt(&w)
                poly_mul(&w,w,c)
                nres_it(&w)

                poly_add(&y[i],y[i],w) // re-use y for z 
                redc_it(&y[i])
                intt(&y[i])
                poly_soft_reduce(&y[i])

                if infinity_norm(y[i]) >= gamma1-beta {
                    badone=true
                    break
                }
            }
            if badone {
                continue
            }
            var nh = 0
            for i in 0..<omega+ck {
                hint[i]=0
            }
            for i in 0..<ck {
                poly_scopy(&w,s2[i])
                ntt(&w)
                poly_mul(&w,w,c)
                intt(&w)
                poly_sub(&Ay[i],Ay[i],w)
                poly_soft_reduce(&Ay[i])
                lobits(params,&w,Ay[i])
                if infinity_norm(w) >= gamma2-beta {
                    badone = true
                    break
                }
                poly_mcopy(&w,t0[i])
                ntt(&w)
                poly_mul(&w,w,c)

                intt(&w)
                poly_negate(&r,w) // -ct0
                if infinity_norm(r) >= gamma2 {
                    badone = true
                    break
                }
                poly_sub(&Ay[i],Ay[i],r)
                poly_soft_reduce(&Ay[i])
                nh = MakePartialHint(params,&hint,nh,r,Ay[i])
                if nh > omega {
                    badone = true
                    break
                }
                hint[omega+i]=UInt8(nh)
            }
            if badone {
                continue
            }
            break
        }
        _ = pack_sig(params,&sig,&y,ct,hint)
        return k
    }

    static func verify(_ params:[Int],_ pk: [UInt8],_ M: [UInt8],_ sig: [UInt8]) -> Bool {
        var ct=[UInt8](repeating: 0,count: 32)
        var mu=[UInt8](repeating: 0,count: 64)
        var rho=[UInt8](repeating: 0,count: 32)
        var tr=[UInt8](repeating: 0,count: 32)
        var cct=[UInt8](repeating: 0,count: 32)
        var hint=[UInt8](repeating: 0,count: 100)

        var Aij=[Int32](repeating: 0,count: DL_DEGREE)   
        var c=[Int32](repeating: 0,count: DL_DEGREE)   
        var w=[Int32](repeating: 0,count: DL_DEGREE)   
        var r=[Int32](repeating: 0,count: DL_DEGREE)   

        let ck = params[3]
        let el = params[4]

        var z = [[Int32]](repeating: [Int32](repeating: 0,count: DL_DEGREE), count: el)
        var t1 = [[Int16]](repeating: [Int16](repeating: 0,count: DL_DEGREE), count: ck)
        var w1d = [[Int8]](repeating: [Int8](repeating: 0,count: DL_DEGREE), count: ck)

	    let tau = params[0]
	    let lg = params[1]
	    let gamma1 = Int32(1<<lg)
	    let eta = params[5]
	    let beta = Int32(tau*eta)
	    let omega = params[7]       
        
        unpack_pk(params,&rho,&t1,pk)
        unpack_sig(params,&z,&ct,&hint,sig)

        for i in 0..<el {
            if infinity_norm(z[i]) >= gamma1-beta {
                return false
            }
            ntt(&z[i])
        }
        CRH1(params,&tr,rho,t1)
        CRH2(&mu,tr,M,M.count)
        SampleInBall(params,ct,&c)
        ntt(&c)
        
        var hints = 0
        for i in 0..<ck {
            poly_zero(&r)
            for j in 0..<el {
                poly_copy(&w,z[j])
                ExpandAij(rho,&Aij,i,j)
                poly_mul(&w,w,Aij)
                poly_add(&r,r,w)
            }
            poly_hard_reduce(&r)
            for m in 0..<DL_DEGREE {
                w[m] = Int32(t1[i][m])<<DL_D
            }
            ntt(&w)
            poly_mul(&w,w,c)
            poly_sub(&r,r,w)
            intt(&r)
            hints=UsePartialHint(params,&w1d[i],hint,hints,i,r)
            if hints > omega {
                return false
            }
        }
        H4(params,&cct,mu,w1d)
        for i in 0..<32  {
            if ct[i] != cct[i] {
                return false
            }
        }
        return true
    }

    public static func keypair_2(_ tau: [UInt8],_ sk:inout [UInt8],_ pk:inout [UInt8]) {
        keypair(PARAMS_2,tau,&sk,&pk)
    }

    public static func signature_2(_ sk: [UInt8],_ M: [UInt8],_ sig:inout  [UInt8]) -> Int {
	    return signature(PARAMS_2, sk, M, &sig) 
    }
 
    public static func verify_2(_ pk: [UInt8],_ M: [UInt8],_ sig: [UInt8]) -> Bool {
	    return verify(PARAMS_2, pk, M, sig)
    }

    public static func keypair_3(_ tau: [UInt8],_ sk:inout [UInt8],_ pk:inout [UInt8]) {
        keypair(PARAMS_3,tau,&sk,&pk)
    }

    public static func signature_3(_ sk: [UInt8],_ M: [UInt8],_ sig:inout  [UInt8]) -> Int {
	    return signature(PARAMS_3, sk, M, &sig) 
    }
 
    public static func verify_3(_ pk: [UInt8],_ M: [UInt8],_ sig: [UInt8]) -> Bool {
	    return verify(PARAMS_3, pk, M, sig)
    }

    public static func keypair_5(_ tau: [UInt8],_ sk:inout [UInt8],_ pk:inout [UInt8]) {
        keypair(PARAMS_5,tau,&sk,&pk)
    }

    public static func signature_5(_ sk: [UInt8],_ M: [UInt8],_ sig:inout  [UInt8]) -> Int {
	    return signature(PARAMS_5, sk, M, &sig) 
    }
 
    public static func verify_5(_ pk: [UInt8],_ M: [UInt8],_ sig: [UInt8]) -> Bool {
	    return verify(PARAMS_5, pk, M, sig)
    }

}