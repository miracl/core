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

//
//  TestALL.swift
//
//  Created by Michael Scott on 02/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Test MPIN API */

import Foundation
import core // comment out for Xcode

import bn254
import bls12383
import bls24479
import bls48556

public func printBinary(_ array: [UInt8])
{
    for i in 0 ..< array.count
    {
        let h=String(format:"%02x",array[i])
        print("\(h)", terminator: "")
    }
    print(" ");
}

public func TestMPIN_bn254(_ rng: inout RAND)
{

    let EGS=bn254.MPIN.EGS
    let EFS=bn254.MPIN.EFS
    let G1S=2*EFS+1    // Group 1 Size
    let G2S=4*EFS+1;     // Group 2 Size

    var S=[UInt8](repeating: 0,count: EGS)
    var SST=[UInt8](repeating: 0,count: G2S)
    var TOKEN=[UInt8](repeating: 0,count: G1S)
    var SEC=[UInt8](repeating: 0,count: G1S)
    var xID=[UInt8](repeating: 0,count: G1S)
    var X=[UInt8](repeating: 0,count: EGS)
    var Y=[UInt8](repeating: 0,count: EGS)
    var HCID=[UInt8](repeating: 0,count: G1S)
    var HSID=[UInt8](repeating: 0,count: G1S)

    let dst = "BN254G1_XMD:SHA-256_SVDW_NU_MPIN"
    let DST=[UInt8](dst.utf8)

    print("\nTesting MPIN 2-factor authentication protocol on curve BN254\n")

// Trusted Authority (TA) set-up
    bn254.MPIN.RANDOM_GENERATE(&rng,&S)
    print("MPIN Master Secret s: 0x",terminator: "");  printBinary(S)

// Create Client Identity
    let IDstr = "testUser@miracl.com"
    let CLIENT_ID=[UInt8](IDstr.utf8)
    bn254.MPIN.ENCODE_TO_CURVE(DST,CLIENT_ID,&HCID)
    print("Client ID Hashed to Curve= "); printBinary(HCID)

// Client approaches Trusted Authority and is issued secret
    bn254.MPIN.GET_CLIENT_SECRET(&S,HCID,&TOKEN);
    print("Client Secret CS: 0x",terminator: ""); printBinary(TOKEN);
// TA sends Client secret to Client

// Server is issued secret by TA
    bn254.MPIN.GET_SERVER_SECRET(S,&SST);
//    print("Server Secret SS: 0x",terminator: "");  printBinary(SST);


// Client extracts PIN from secret to create Token
        var pin:Int32=1234
        print("Client extracts PIN= \(pin)")
        var rtn=bn254.MPIN.EXTRACT_PIN(HCID,pin,&TOKEN)
        if rtn != 0 {
            print("FAILURE: EXTRACT_PIN rtn: \(rtn)")
            return
        }
        print("Client Token TK: 0x",terminator: ""); printBinary(TOKEN);

// Exercise Secret Sharing
            var R=[UInt8](repeating: 0,count: 128)
            for i in 0..<128 {
                R[i]=rng.getByte()
            }
       // create 4 unique shares of TOKEN
            let Sh1=SHARE(1,3,TOKEN,R)  // indicate 3 shares required for recovery
            let Sh2=SHARE(2,3,TOKEN,R)
            //let Sh3=SHARE(3,3,TOKEN,R)
            let Sh4=SHARE(4,3,TOKEN,R)

            let Shares:[SHARE] = [Sh1,Sh2,Sh4]  // any 3 shares to recover TOKEN

            TOKEN=SHARE.recover(Shares)  // recover token


// MPin Protocol

// Client enters ID and PIN

    // ***** NOW ENTER PIN *******

        pin=1234

    // **************************


// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC

        var REALRNG : RAND? = rng
        rtn=bn254.MPIN.CLIENT_1(HCID,&REALRNG,&X,pin,TOKEN,&SEC,&xID)
        if rtn != 0 {
            print("FAILURE: CLIENT_1 rtn: \(rtn)")
            return
        }

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.m to points on the curve
                bn254.MPIN.ENCODE_TO_CURVE(DST,CLIENT_ID,&HSID)
                
// Server generates Random number Y and sends it to Client
                bn254.MPIN.RANDOM_GENERATE(&REALRNG!,&Y);

// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
        rtn=bn254.MPIN.CLIENT_2(X,Y,&SEC);
        if rtn != 0 {
            print("FAILURE: CLIENT_2 rtn: \(rtn)")
            return
        }

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST.
                rtn=bn254.MPIN.SERVER(HSID,Y,SST,xID,SEC);

                if rtn != 0 {
                    if (rtn == bn254.MPIN.BAD_PIN) {
                        print("Server says - Bad Pin. I don't know you. Feck off.\n");
                    } else {
                        print("FAILURE: SERVER rtn: \(rtn)")
                    }
                }
                else {print("Server says - PIN is good! You really are "+IDstr+"\n")}

    rng=REALRNG!
}

public func TestMPIN_bls12383(_ rng: inout RAND)
{

    let EGS=bls12383.MPIN.EGS
    let EFS=bls12383.MPIN.EFS
    let G1S=2*EFS+1    // Group 1 Size
    let G2S=4*EFS+1;     // Group 2 Size

    var S=[UInt8](repeating: 0,count: EGS)
    var SST=[UInt8](repeating: 0,count: G2S)
    var TOKEN=[UInt8](repeating: 0,count: G1S)
    var SEC=[UInt8](repeating: 0,count: G1S)
    var xID=[UInt8](repeating: 0,count: G1S)
    var X=[UInt8](repeating: 0,count: EGS)
    var Y=[UInt8](repeating: 0,count: EGS)
    var HCID=[UInt8](repeating: 0,count: G1S)
    var HSID=[UInt8](repeating: 0,count: G1S)

    let dst = "BLS12383G1_XMD:SHA-256_SVDW_NU_MPIN"
    let DST=[UInt8](dst.utf8)

    print("\nTesting MPIN 2-factor authentication protocol on curve bls12383\n")

// Trusted Authority (TA) set-up
    bls12383.MPIN.RANDOM_GENERATE(&rng,&S)
    print("MPIN Master Secret s: 0x",terminator: "");  printBinary(S)

// Create Client Identity
    let IDstr = "testUser@miracl.com"
    let CLIENT_ID=[UInt8](IDstr.utf8)
    bls12383.MPIN.ENCODE_TO_CURVE(DST,CLIENT_ID,&HCID)
    print("Client ID Hashed to Curve= "); printBinary(HCID)

// Client approaches Trusted Authority and is issued secret
    bls12383.MPIN.GET_CLIENT_SECRET(&S,HCID,&TOKEN);
    print("Client Secret CS: 0x",terminator: ""); printBinary(TOKEN);
// TA sends Client secret to Client

// Server is issued secret by TA
    bls12383.MPIN.GET_SERVER_SECRET(S,&SST);
//    print("Server Secret SS: 0x",terminator: "");  printBinary(SST);


// Client extracts PIN from secret to create Token
        var pin:Int32=1234
        print("Client extracts PIN= \(pin)")
        var rtn=bls12383.MPIN.EXTRACT_PIN(HCID,pin,&TOKEN)
        if rtn != 0 {
            print("FAILURE: EXTRACT_PIN rtn: \(rtn)")
            return
        }
        print("Client Token TK: 0x",terminator: ""); printBinary(TOKEN);

// MPin Protocol

// Client enters ID and PIN

    // ***** NOW ENTER PIN *******

        pin=1234

    // **************************


// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC

        var REALRNG : RAND? = rng
        rtn=bls12383.MPIN.CLIENT_1(HCID,&REALRNG,&X,pin,TOKEN,&SEC,&xID)
        if rtn != 0 {
            print("FAILURE: CLIENT_1 rtn: \(rtn)")
            return
        }

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.m to points on the curve
                bls12383.MPIN.ENCODE_TO_CURVE(DST,CLIENT_ID,&HSID)
                
// Server generates Random number Y and sends it to Client
                bls12383.MPIN.RANDOM_GENERATE(&REALRNG!,&Y);

// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
        rtn=bls12383.MPIN.CLIENT_2(X,Y,&SEC);
        if rtn != 0 {
            print("FAILURE: CLIENT_2 rtn: \(rtn)")
            return
        }

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST.
                rtn=bls12383.MPIN.SERVER(HSID,Y,SST,xID,SEC);

                if rtn != 0 {
                    if (rtn == bls12383.MPIN.BAD_PIN) {
                        print("Server says - Bad Pin. I don't know you. Feck off.\n");
                    } else {
                        print("FAILURE: SERVER rtn: \(rtn)")
                    }
                }
                else {print("Server says - PIN is good! You really are "+IDstr+"\n")}

    rng=REALRNG!
}


public func TestMPIN_bls24479(_ rng: inout RAND)
{

    let EGS=bls24479.MPIN192.EGS
    let EFS=bls24479.MPIN192.EFS
    let G1S=2*EFS+1    // Group 1 Size
    let G2S=8*EFS+1;     // Group 2 Size

    var S=[UInt8](repeating: 0,count: EGS)
    var SST=[UInt8](repeating: 0,count: G2S)
    var TOKEN=[UInt8](repeating: 0,count: G1S)
    var SEC=[UInt8](repeating: 0,count: G1S)
    var xID=[UInt8](repeating: 0,count: G1S)
    var X=[UInt8](repeating: 0,count: EGS)
    var Y=[UInt8](repeating: 0,count: EGS)
    var HCID=[UInt8](repeating: 0,count: G1S)
    var HSID=[UInt8](repeating: 0,count: G1S)

    let dst = "BLS24479G1_XMD:SHA-256_SVDW_NU_MPIN"
    let DST=[UInt8](dst.utf8)

    print("\nTesting MPIN 2-factor authentication protocol on curve bls24479\n")

// Trusted Authority (TA) set-up
    bls24479.MPIN192.RANDOM_GENERATE(&rng,&S)
    print("MPIN Master Secret s: 0x",terminator: "");  printBinary(S)

// Create Client Identity
    let IDstr = "testUser@miracl.com"
    let CLIENT_ID=[UInt8](IDstr.utf8)
    bls24479.MPIN192.ENCODE_TO_CURVE(DST,CLIENT_ID,&HCID)
    print("Client ID Hashed to Curve= "); printBinary(HCID)

// Client approaches Trusted Authority and is issued secret
    bls24479.MPIN192.GET_CLIENT_SECRET(&S,HCID,&TOKEN);
    print("Client Secret CS: 0x",terminator: ""); printBinary(TOKEN);
// TA sends Client secret to Client

// Server is issued secret by TA
    bls24479.MPIN192.GET_SERVER_SECRET(S,&SST);
//    print("Server Secret SS: 0x",terminator: "");  printBinary(SST);


// Client extracts PIN from secret to create Token
        var pin:Int32=1234
        print("Client extracts PIN= \(pin)")
        var rtn=bls24479.MPIN192.EXTRACT_PIN(HCID,pin,&TOKEN)
        if rtn != 0 {
            print("FAILURE: EXTRACT_PIN rtn: \(rtn)")
            return
        }
        print("Client Token TK: 0x",terminator: ""); printBinary(TOKEN);

// MPin Protocol

// Client enters ID and PIN

    // ***** NOW ENTER PIN *******

        pin=1234

    // **************************


// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC

        var REALRNG : RAND? = rng
        rtn=bls24479.MPIN192.CLIENT_1(HCID,&REALRNG,&X,pin,TOKEN,&SEC,&xID)
        if rtn != 0 {
            print("FAILURE: CLIENT_1 rtn: \(rtn)")
            return
        }

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.m to points on the curve
                bls24479.MPIN192.ENCODE_TO_CURVE(DST,CLIENT_ID,&HSID)
                
// Server generates Random number Y and sends it to Client
                bls24479.MPIN192.RANDOM_GENERATE(&REALRNG!,&Y);

// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
        rtn=bls24479.MPIN192.CLIENT_2(X,Y,&SEC);
        if rtn != 0 {
            print("FAILURE: CLIENT_2 rtn: \(rtn)")
            return
        }

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST.
                rtn=bls24479.MPIN192.SERVER(HSID,Y,SST,xID,SEC);

                if rtn != 0 {
                    if (rtn == bls24479.MPIN192.BAD_PIN) {
                        print("Server says - Bad Pin. I don't know you. Feck off.\n");
                    } else {
                        print("FAILURE: SERVER rtn: \(rtn)")
                    }
                }
                else {print("Server says - PIN is good! You really are "+IDstr+"\n")}

    rng=REALRNG!
}

public func TestMPIN_bls48556(_ rng: inout RAND)
{

    let EGS=bls48556.MPIN256.EGS
    let EFS=bls48556.MPIN256.EFS
    let G1S=2*EFS+1    // Group 1 Size
    let G2S=16*EFS+1;     // Group 2 Size

    var S=[UInt8](repeating: 0,count: EGS)
    var SST=[UInt8](repeating: 0,count: G2S)
    var TOKEN=[UInt8](repeating: 0,count: G1S)
    var SEC=[UInt8](repeating: 0,count: G1S)
    var xID=[UInt8](repeating: 0,count: G1S)
    var X=[UInt8](repeating: 0,count: EGS)
    var Y=[UInt8](repeating: 0,count: EGS)
    var HCID=[UInt8](repeating: 0,count: G1S)
    var HSID=[UInt8](repeating: 0,count: G1S)

    let dst = "BLS48556G1_XMD:SHA-256_SVDW_NU_MPIN"
    let DST=[UInt8](dst.utf8)

    print("\nTesting MPIN 2-factor authentication protocol on curve bls48556\n")

// Trusted Authority (TA) set-up
    bls48556.MPIN256.RANDOM_GENERATE(&rng,&S)
    print("MPIN Master Secret s: 0x",terminator: "");  printBinary(S)

// Create Client Identity
    let IDstr = "testUser@miracl.com"
    let CLIENT_ID=[UInt8](IDstr.utf8)
    bls48556.MPIN256.ENCODE_TO_CURVE(DST,CLIENT_ID,&HCID)
    print("Client ID Hashed to Curve= "); printBinary(HCID)

// Client approaches Trusted Authority and is issued secret
    bls48556.MPIN256.GET_CLIENT_SECRET(&S,HCID,&TOKEN);
    print("Client Secret CS: 0x",terminator: ""); printBinary(TOKEN);
// TA sends Client secret to Client

// Server is issued secret by TA
    bls48556.MPIN256.GET_SERVER_SECRET(S,&SST);
//    print("Server Secret SS: 0x",terminator: "");  printBinary(SST);


// Client extracts PIN from secret to create Token
        var pin:Int32=1234
        print("Client extracts PIN= \(pin)")
        var rtn=bls48556.MPIN256.EXTRACT_PIN(HCID,pin,&TOKEN)
        if rtn != 0 {
            print("FAILURE: EXTRACT_PIN rtn: \(rtn)")
            return
        }
        print("Client Token TK: 0x",terminator: ""); printBinary(TOKEN);

// MPin Protocol

// Client enters ID and PIN

    // ***** NOW ENTER PIN *******

        pin=1234

    // **************************


// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC

        var REALRNG : RAND? = rng
        rtn=bls48556.MPIN256.CLIENT_1(HCID,&REALRNG,&X,pin,TOKEN,&SEC,&xID)
        if rtn != 0 {
            print("FAILURE: CLIENT_1 rtn: \(rtn)")
            return
        }

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.m to points on the curve
                bls48556.MPIN256.ENCODE_TO_CURVE(DST,CLIENT_ID,&HSID)
                
// Server generates Random number Y and sends it to Client
                bls48556.MPIN256.RANDOM_GENERATE(&REALRNG!,&Y);

// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
        rtn=bls48556.MPIN256.CLIENT_2(X,Y,&SEC);
        if rtn != 0 {
            print("FAILURE: CLIENT_2 rtn: \(rtn)")
            return
        }

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST.
                rtn=bls48556.MPIN256.SERVER(HSID,Y,SST,xID,SEC);

                if rtn != 0 {
                    if (rtn == bls48556.MPIN256.BAD_PIN) {
                        print("Server says - Bad Pin. I don't know you. Feck off.\n");
                    } else {
                        print("FAILURE: SERVER rtn: \(rtn)")
                    }
                }
                else {print("Server says - PIN is good! You really are "+IDstr+"\n")}

    rng=REALRNG!
}


var RAW=[UInt8](repeating: 0,count: 100)
var rng=RAND()

rng.clean();
for i in 0 ..< 100 {RAW[i]=UInt8(i&0xff)}
rng.seed(100,RAW)

TestMPIN_bn254(&rng)
TestMPIN_bls12383(&rng)
TestMPIN_bls24479(&rng)
TestMPIN_bls48556(&rng)
