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
    let PERMITS=true
    let PINERROR=true
    let FULL=true
    let SINGLE_PASS=true

    let EGS=bn254.MPIN.EGS
    let EFS=bn254.MPIN.EFS
    let G1S=2*EFS+1    // Group 1 Size
    let G2S=4*EFS+1;     // Group 2 Size
    let EAS=bn254.CONFIG_CURVE.AESKEY

    let sha=bn254.CONFIG_CURVE.HASH_TYPE

    var S=[UInt8](repeating: 0,count: EGS)
    var SST=[UInt8](repeating: 0,count: G2S)
    var TOKEN=[UInt8](repeating: 0,count: G1S)
    var PERMIT=[UInt8](repeating: 0,count: G1S)
    var SEC=[UInt8](repeating: 0,count: G1S)
    let xID=[UInt8](repeating: 0,count: G1S)
    let xCID=[UInt8](repeating: 0,count: G1S)
    var X=[UInt8](repeating: 0,count: EGS)
    var Y=[UInt8](repeating: 0,count: EGS)
    let E=[UInt8](repeating: 0,count: 12*EFS)
    let F=[UInt8](repeating: 0,count: 12*EFS)
    let HID=[UInt8](repeating: 0,count: G1S)
    let HTID=[UInt8](repeating: 0,count: G1S)

    var G1=[UInt8](repeating: 0,count: 12*EFS)
    var G2=[UInt8](repeating: 0,count: 12*EFS)
    var R=[UInt8](repeating: 0,count: EGS)
    var Z=[UInt8](repeating: 0,count: G1S)
    var W=[UInt8](repeating: 0,count: EGS)
    var T=[UInt8](repeating: 0,count: G1S)
    var CK=[UInt8](repeating: 0,count: EAS)
    var SK=[UInt8](repeating: 0,count: EAS)

    var HSID=[UInt8]()

    // Trusted Authority set-up

    bn254.MPIN.RANDOM_GENERATE(&rng,&S)
    print("\nTest Curve bn254");
    print("MPIN Master Secret s: 0x",terminator: "");  printBinary(S)

    // Create Client Identity
    let IDstr = "testUser@miracl.com"
    let CLIENT_ID=[UInt8](IDstr.utf8)

    var HCID=bn254.MPIN.HASH_ID(sha,CLIENT_ID)  // Either Client or TA calculates Hash(ID) - you decide!

    print("Client ID= "); printBinary(CLIENT_ID)

    // Client and Server are issued secrets by DTA
    bn254.MPIN.GET_SERVER_SECRET(S,&SST);
    print("Server Secret SS: 0x",terminator: "");  printBinary(SST);

    bn254.MPIN.GET_CLIENT_SECRET(&S,HCID,&TOKEN);
    print("Client Secret CS: 0x",terminator: ""); printBinary(TOKEN);

    // Client extracts PIN from secret to create Token
    var pin:Int32=1234
    print("Client extracts PIN= \(pin)")
    var rtn=bn254.MPIN.EXTRACT_PIN(sha,CLIENT_ID,pin,&TOKEN)
    if rtn != 0 {print("FAILURE: EXTRACT_PIN rtn: \(rtn)")}

    print("Client Token TK: 0x",terminator: ""); printBinary(TOKEN);

    if FULL
    {
        bn254.MPIN.PRECOMPUTE(TOKEN,HCID,&G1,&G2);
    }

    var date:Int32=0
    if (PERMITS)
    {
        date=bn254.MPIN.today()
        // Client gets "Time Token" permit from DTA
        bn254.MPIN.GET_CLIENT_PERMIT(sha,date,S,HCID,&PERMIT)
        print("Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)

        // This encoding makes Time permit look random - Elligator squared
        bn254.MPIN.ENCODING(&rng,&PERMIT);
        print("Encoded Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)
        bn254.MPIN.DECODING(&PERMIT)
        print("Decoded Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)
    }

    // ***** NOW ENTER PIN *******

        pin=1234

    // **************************

    // Set date=0 and PERMIT=null if time permits not in use

    //Client First pass: Inputs CLIENT_ID, optional RNG, pin, TOKEN and PERMIT. Output xID =x .H(CLIENT_ID) and re-combined secret SEC
    //If PERMITS are is use, then date!=0 and PERMIT is added to secret and xCID = x.(H(CLIENT_ID)+H(date|H(CLIENT_ID)))
    //Random value x is supplied externally if RNG=null, otherwise generated and passed out by RNG

    //IMPORTANT: To save space and time..
    //If Time Permits OFF set xCID = null, HTID=null and use xID and HID only
    //If Time permits are ON, AND pin error detection is required then all of xID, xCID, HID and HTID are required
    //If Time permits are ON, AND pin error detection is NOT required, set xID=null, HID=null and use xCID and HTID only.


    var pxID:[UInt8]?=xID
    var pxCID:[UInt8]?=xCID
    var pHID:[UInt8]=HID
    var pHTID:[UInt8]?=HTID
    var pE:[UInt8]?=E
    var pF:[UInt8]?=F
    var pPERMIT:[UInt8]?=PERMIT

    var REALRNG : RAND? = rng

    if date != 0
    {
        if (!PINERROR)
        {
            pxID=nil;  // problem here - either comment out here or dont use with ! later on
   //         pHID=nil;
        }
    }
    else
    {
        pPERMIT=nil;
        pxCID=nil;
        pHTID=nil;
    }
    if (!PINERROR)
    {
        pE=nil;
        pF=nil;
    }

    if (SINGLE_PASS)
    {
        print("MPIN Single Pass")
        let timeValue = bn254.MPIN.GET_TIME()

        rtn=bn254.MPIN.CLIENT(sha,date,CLIENT_ID,&REALRNG,&X,pin,TOKEN,&SEC,&pxID,&pxCID,pPERMIT,timeValue,&Y)

        if rtn != 0 {print("FAILURE: CLIENT rtn: \(rtn)")}

        if (FULL)
        {
            HCID=bn254.MPIN.HASH_ID(sha,CLIENT_ID);
            bn254.MPIN.GET_G1_MULTIPLE(&REALRNG,1,&R,HCID,&Z); // Also Send Z=r.ID to Server, remember random r
        }
        rtn=bn254.MPIN.SERVER(sha,date,&pHID,&pHTID,&Y,SST,pxID,pxCID!,SEC,&pE,&pF,CLIENT_ID,timeValue)
        if rtn != 0 {print("FAILURE: SERVER rtn: \(rtn)")}

        if (FULL)
        { // Also send T=w.ID to client, remember random w
            HSID=bn254.MPIN.HASH_ID(sha,CLIENT_ID);
            if date != 0 {bn254.MPIN.GET_G1_MULTIPLE(&REALRNG,0,&W,pHTID!,&T)}
            else {bn254.MPIN.GET_G1_MULTIPLE(&REALRNG,0,&W,pHID,&T)}

        }
    }
    else
    {
        print("MPIN Multi Pass");
        // Send U=x.ID to server, and recreate secret from token and pin
        rtn=bn254.MPIN.CLIENT_1(sha,date,CLIENT_ID,&REALRNG,&X,pin,TOKEN,&SEC,&pxID,&pxCID,pPERMIT)
        if rtn != 0 {print("FAILURE: CLIENT_1 rtn: \(rtn)")}

        if (FULL)
        {
            HCID=bn254.MPIN.HASH_ID(sha,CLIENT_ID);
            bn254.MPIN.GET_G1_MULTIPLE(&REALRNG,1,&R,HCID,&Z);  // Also Send Z=r.ID to Server, remember random r
        }
        // Server calculates H(ID) and H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp.
        bn254.MPIN.SERVER_1(sha,date,CLIENT_ID,&pHID,&pHTID);
            // Server generates Random number Y and sends it to Client
        bn254.MPIN.RANDOM_GENERATE(&REALRNG!,&Y);

        if (FULL)
        { // Also send T=w.ID to client, remember random w
            HSID=bn254.MPIN.HASH_ID(sha,CLIENT_ID);
            if date != 0 {bn254.MPIN.GET_G1_MULTIPLE(&REALRNG,0,&W,pHTID!,&T)}
            else {bn254.MPIN.GET_G1_MULTIPLE(&REALRNG,0,&W,pHID,&T)}
        }

        // Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
        rtn=bn254.MPIN.CLIENT_2(X,Y,&SEC);
        if rtn != 0 {print("FAILURE: CLIENT_2 rtn: \(rtn)")}

        // Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error.
        // If PIN error not required, set E and F = null

        rtn=bn254.MPIN.SERVER_2(date,pHID,pHTID,Y,SST,pxID,pxCID,SEC,&pE,&pF);

        if rtn != 0 {print("FAILURE: SERVER_1 rtn: \(rtn)")}
    }
    if (rtn == bn254.MPIN.BAD_PIN)
    {
        print("Server says - Bad Pin. I don't know you. Feck off.\n");
        if (PINERROR)
        {
            let err=bn254.MPIN.KANGAROO(pE,pF);
            if err != 0 {print("(Client PIN is out by \(err))\n")}
        }
        return;
    }
    else {print("Server says - PIN is good! You really are "+IDstr)}

    if (FULL)
    {
        var H=bn254.MPIN.HASH_ALL(sha,HCID,pxID,pxCID,SEC,Y,Z,T);
        bn254.MPIN.CLIENT_KEY(sha,G1,G2,pin,R,X,H,T,&CK);
        print("Client Key =  0x",terminator: "");  printBinary(CK)

        H=bn254.MPIN.HASH_ALL(sha,HSID,pxID,pxCID,SEC,Y,Z,T);
        bn254.MPIN.SERVER_KEY(sha,Z,SST,W,H,pHID,pxID!,pxCID,&SK);
        print("Server Key =  0x",terminator: "");  printBinary(SK)
    }
    rng=REALRNG!
}

public func TestMPIN_bls12383(_ rng: inout RAND)
{
    let PERMITS=true
    let PINERROR=true
    let FULL=true
    let SINGLE_PASS=true


    let EGS=bls12383.MPIN.EGS
    let EFS=bls12383.MPIN.EFS
    let G1S=2*EFS+1    // Group 1 Size
    let G2S=4*EFS+1;     // Group 2 Size
    let EAS=bls12383.CONFIG_CURVE.AESKEY

    let sha=bls12383.CONFIG_CURVE.HASH_TYPE

    var S=[UInt8](repeating: 0,count: EGS)
    var SST=[UInt8](repeating: 0,count: G2S)
    var TOKEN=[UInt8](repeating: 0,count: G1S)
    var PERMIT=[UInt8](repeating: 0,count: G1S)
    var SEC=[UInt8](repeating: 0,count: G1S)
    let xID=[UInt8](repeating: 0,count: G1S)
    let xCID=[UInt8](repeating: 0,count: G1S)
    var X=[UInt8](repeating: 0,count: EGS)
    var Y=[UInt8](repeating: 0,count: EGS)
    let E=[UInt8](repeating: 0,count: 12*EFS)
    let F=[UInt8](repeating: 0,count: 12*EFS)
    let HID=[UInt8](repeating: 0,count: G1S)
    let HTID=[UInt8](repeating: 0,count: G1S)

    var G1=[UInt8](repeating: 0,count: 12*EFS)
    var G2=[UInt8](repeating: 0,count: 12*EFS)
    var R=[UInt8](repeating: 0,count: EGS)
    var Z=[UInt8](repeating: 0,count: G1S)
    var W=[UInt8](repeating: 0,count: EGS)
    var T=[UInt8](repeating: 0,count: G1S)
    var CK=[UInt8](repeating: 0,count: EAS)
    var SK=[UInt8](repeating: 0,count: EAS)

    var HSID=[UInt8]()

    // Trusted Authority set-up

    bls12383.MPIN.RANDOM_GENERATE(&rng,&S)
    print("\nTest Curve bls12383");
    print("MPIN Master Secret s: 0x",terminator: "");  printBinary(S)

    // Create Client Identity
    let IDstr = "testUser@miracl.com"
    let CLIENT_ID=[UInt8](IDstr.utf8)

    var HCID=bls12383.MPIN.HASH_ID(sha,CLIENT_ID)  // Either Client or TA calculates Hash(ID) - you decide!

    print("Client ID= "); printBinary(CLIENT_ID)

    // Client and Server are issued secrets by DTA
    bls12383.MPIN.GET_SERVER_SECRET(S,&SST);
    print("Server Secret SS: 0x",terminator: "");  printBinary(SST);

    bls12383.MPIN.GET_CLIENT_SECRET(&S,HCID,&TOKEN);
    print("Client Secret CS: 0x",terminator: ""); printBinary(TOKEN);

    // Client extracts PIN from secret to create Token
    var pin:Int32=1234
    print("Client extracts PIN= \(pin)")
    var rtn=bls12383.MPIN.EXTRACT_PIN(sha,CLIENT_ID,pin,&TOKEN)
    if rtn != 0 {print("FAILURE: EXTRACT_PIN rtn: \(rtn)")}

    print("Client Token TK: 0x",terminator: ""); printBinary(TOKEN);

    if FULL
    {
        bls12383.MPIN.PRECOMPUTE(TOKEN,HCID,&G1,&G2);
    }

    var date:Int32=0
    if (PERMITS)
    {
        date=bls12383.MPIN.today()
        // Client gets "Time Token" permit from DTA
        bls12383.MPIN.GET_CLIENT_PERMIT(sha,date,S,HCID,&PERMIT)
        print("Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)

        // This encoding makes Time permit look random - Elligator squared
        bls12383.MPIN.ENCODING(&rng,&PERMIT);
        print("Encoded Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)
        bls12383.MPIN.DECODING(&PERMIT)
        print("Decoded Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)
    }

    // ***** NOW ENTER PIN *******

        pin=1234

    // **************************

    // Set date=0 and PERMIT=null if time permits not in use

    //Client First pass: Inputs CLIENT_ID, optional RNG, pin, TOKEN and PERMIT. Output xID =x .H(CLIENT_ID) and re-combined secret SEC
    //If PERMITS are is use, then date!=0 and PERMIT is added to secret and xCID = x.(H(CLIENT_ID)+H(date|H(CLIENT_ID)))
    //Random value x is supplied externally if RNG=null, otherwise generated and passed out by RNG

    //IMPORTANT: To save space and time..
    //If Time Permits OFF set xCID = null, HTID=null and use xID and HID only
    //If Time permits are ON, AND pin error detection is required then all of xID, xCID, HID and HTID are required
    //If Time permits are ON, AND pin error detection is NOT required, set xID=null, HID=null and use xCID and HTID only.


    var pxID:[UInt8]?=xID
    var pxCID:[UInt8]?=xCID
    var pHID:[UInt8]=HID
    var pHTID:[UInt8]?=HTID
    var pE:[UInt8]?=E
    var pF:[UInt8]?=F
    var pPERMIT:[UInt8]?=PERMIT

    var REALRNG : RAND? = rng

    if date != 0
    {
        if (!PINERROR)
        {
            pxID=nil;  // problem here - either comment out here or dont use with ! later on
   //         pHID=nil;
        }
    }
    else
    {
        pPERMIT=nil;
        pxCID=nil;
        pHTID=nil;
    }
    if (!PINERROR)
    {
        pE=nil;
        pF=nil;
    }

    if (SINGLE_PASS)
    {
        print("MPIN Single Pass")
        let timeValue = bls12383.MPIN.GET_TIME()

        rtn=bls12383.MPIN.CLIENT(sha,date,CLIENT_ID,&REALRNG,&X,pin,TOKEN,&SEC,&pxID,&pxCID,pPERMIT,timeValue,&Y)

        if rtn != 0 {print("FAILURE: CLIENT rtn: \(rtn)")}

        if (FULL)
        {
            HCID=bls12383.MPIN.HASH_ID(sha,CLIENT_ID);
            bls12383.MPIN.GET_G1_MULTIPLE(&REALRNG,1,&R,HCID,&Z); // Also Send Z=r.ID to Server, remember random r
        }
        rtn=bls12383.MPIN.SERVER(sha,date,&pHID,&pHTID,&Y,SST,pxID,pxCID!,SEC,&pE,&pF,CLIENT_ID,timeValue)
        if rtn != 0 {print("FAILURE: SERVER rtn: \(rtn)")}

        if (FULL)
        { // Also send T=w.ID to client, remember random w
            HSID=bls12383.MPIN.HASH_ID(sha,CLIENT_ID);
            if date != 0 {bls12383.MPIN.GET_G1_MULTIPLE(&REALRNG,0,&W,pHTID!,&T)}
            else {bls12383.MPIN.GET_G1_MULTIPLE(&REALRNG,0,&W,pHID,&T)}

        }
    }
    else
    {
        print("MPIN Multi Pass");
        // Send U=x.ID to server, and recreate secret from token and pin
        rtn=bls12383.MPIN.CLIENT_1(sha,date,CLIENT_ID,&REALRNG,&X,pin,TOKEN,&SEC,&pxID,&pxCID,pPERMIT)
        if rtn != 0 {print("FAILURE: CLIENT_1 rtn: \(rtn)")}

        if (FULL)
        {
            HCID=bls12383.MPIN.HASH_ID(sha,CLIENT_ID);
            bls12383.MPIN.GET_G1_MULTIPLE(&REALRNG,1,&R,HCID,&Z);  // Also Send Z=r.ID to Server, remember random r
        }
        // Server calculates H(ID) and H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp.
        bls12383.MPIN.SERVER_1(sha,date,CLIENT_ID,&pHID,&pHTID);
            // Server generates Random number Y and sends it to Client
        bls12383.MPIN.RANDOM_GENERATE(&REALRNG!,&Y);

        if (FULL)
        { // Also send T=w.ID to client, remember random w
            HSID=bls12383.MPIN.HASH_ID(sha,CLIENT_ID);
            if date != 0 {bls12383.MPIN.GET_G1_MULTIPLE(&REALRNG,0,&W,pHTID!,&T)}
            else {bls12383.MPIN.GET_G1_MULTIPLE(&REALRNG,0,&W,pHID,&T)}
        }

        // Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
        rtn=bls12383.MPIN.CLIENT_2(X,Y,&SEC);
        if rtn != 0 {print("FAILURE: CLIENT_2 rtn: \(rtn)")}

        // Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error.
        // If PIN error not required, set E and F = null

        rtn=bls12383.MPIN.SERVER_2(date,pHID,pHTID,Y,SST,pxID,pxCID,SEC,&pE,&pF);

        if rtn != 0 {print("FAILURE: SERVER_1 rtn: \(rtn)")}
    }
    if (rtn == bls12383.MPIN.BAD_PIN)
    {
        print("Server says - Bad Pin. I don't know you. Feck off.\n");
        if (PINERROR)
        {
            let err=bls12383.MPIN.KANGAROO(pE,pF);
            if err != 0 {print("(Client PIN is out by \(err))\n")}
        }
        return;
    }
    else {print("Server says - PIN is good! You really are "+IDstr)}

    if (FULL)
    {
        var H=bls12383.MPIN.HASH_ALL(sha,HCID,pxID,pxCID,SEC,Y,Z,T);
        bls12383.MPIN.CLIENT_KEY(sha,G1,G2,pin,R,X,H,T,&CK);
        print("Client Key =  0x",terminator: "");  printBinary(CK)

        H=bls12383.MPIN.HASH_ALL(sha,HSID,pxID,pxCID,SEC,Y,Z,T);
        bls12383.MPIN.SERVER_KEY(sha,Z,SST,W,H,pHID,pxID!,pxCID,&SK);
        print("Server Key =  0x",terminator: "");  printBinary(SK)
    }
    rng=REALRNG!
}


public func TestMPIN_bls24479(_ rng: inout RAND)
{
    let PERMITS=true
    let PINERROR=true
    let FULL=true
    let SINGLE_PASS=true


    let EGS=bls24479.MPIN192.EGS
    let EFS=bls24479.MPIN192.EFS
    let G1S=2*EFS+1    // Group 1 Size
    let G2S=8*EFS+1;     // Group 2 Size
    let EAS=bls24479.CONFIG_CURVE.AESKEY

    let sha=bls24479.CONFIG_CURVE.HASH_TYPE

    var S=[UInt8](repeating: 0,count: EGS)
    var SST=[UInt8](repeating: 0,count: G2S)
    var TOKEN=[UInt8](repeating: 0,count: G1S)
    var PERMIT=[UInt8](repeating: 0,count: G1S)
    var SEC=[UInt8](repeating: 0,count: G1S)
    let xID=[UInt8](repeating: 0,count: G1S)
    let xCID=[UInt8](repeating: 0,count: G1S)
    var X=[UInt8](repeating: 0,count: EGS)
    var Y=[UInt8](repeating: 0,count: EGS)
    let E=[UInt8](repeating: 0,count: 24*EFS)
    let F=[UInt8](repeating: 0,count: 24*EFS)
    let HID=[UInt8](repeating: 0,count: G1S)
    let HTID=[UInt8](repeating: 0,count: G1S)

    var G1=[UInt8](repeating: 0,count: 24*EFS)
    var G2=[UInt8](repeating: 0,count: 24*EFS)
    var R=[UInt8](repeating: 0,count: EGS)
    var Z=[UInt8](repeating: 0,count: G1S)
    var W=[UInt8](repeating: 0,count: EGS)
    var T=[UInt8](repeating: 0,count: G1S)
    var CK=[UInt8](repeating: 0,count: EAS)
    var SK=[UInt8](repeating: 0,count: EAS)

    var HSID=[UInt8]()

    // Trusted Authority set-up

    MPIN192.RANDOM_GENERATE(&rng,&S)
    print("\nTest Curve bls24479");
    print("MPIN Master Secret s: 0x",terminator: "");  printBinary(S)

    // Create Client Identity
    let IDstr = "testUser@miracl.com"
    let CLIENT_ID=[UInt8](IDstr.utf8)

    var HCID=MPIN192.HASH_ID(sha,CLIENT_ID)  // Either Client or TA calculates Hash(ID) - you decide!

    print("Client ID= "); printBinary(CLIENT_ID)

    // Client and Server are issued secrets by DTA
    MPIN192.GET_SERVER_SECRET(S,&SST);
    print("Server Secret SS: 0x",terminator: "");  printBinary(SST);

    MPIN192.GET_CLIENT_SECRET(&S,HCID,&TOKEN);
    print("Client Secret CS: 0x",terminator: ""); printBinary(TOKEN);

    // Client extracts PIN from secret to create Token
    var pin:Int32=1234
    print("Client extracts PIN= \(pin)")
    var rtn=MPIN192.EXTRACT_PIN(sha,CLIENT_ID,pin,&TOKEN)
    if rtn != 0 {print("FAILURE: EXTRACT_PIN rtn: \(rtn)")}

    print("Client Token TK: 0x",terminator: ""); printBinary(TOKEN);

    if FULL
    {
        MPIN192.PRECOMPUTE(TOKEN,HCID,&G1,&G2);
    }

    var date:Int32=0
    if (PERMITS)
    {
        date=MPIN192.today()
        // Client gets "Time Token" permit from DTA
        MPIN192.GET_CLIENT_PERMIT(sha,date,S,HCID,&PERMIT)
        print("Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)

        // This encoding makes Time permit look random - Elligator squared
        MPIN192.ENCODING(&rng,&PERMIT);
        print("Encoded Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)
        MPIN192.DECODING(&PERMIT)
        print("Decoded Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)
    }

    // ***** NOW ENTER PIN *******

        pin=1234

    // **************************

    // Set date=0 and PERMIT=null if time permits not in use

    //Client First pass: Inputs CLIENT_ID, optional rng, pin, TOKEN and PERMIT. Output xID =x .H(CLIENT_ID) and re-combined secret SEC
    //If PERMITS are is use, then date!=0 and PERMIT is added to secret and xCID = x.(H(CLIENT_ID)+H(date|H(CLIENT_ID)))
    //Random value x is supplied externally if RNG=null, otherwise generated and passed out by RNG

    //IMPORTANT: To save space and time..
    //If Time Permits OFF set xCID = null, HTID=null and use xID and HID only
    //If Time permits are ON, AND pin error detection is required then all of xID, xCID, HID and HTID are required
    //If Time permits are ON, AND pin error detection is NOT required, set xID=null, HID=null and use xCID and HTID only.


    var pxID:[UInt8]?=xID
    var pxCID:[UInt8]?=xCID
    var pHID:[UInt8]=HID
    var pHTID:[UInt8]?=HTID
    var pE:[UInt8]?=E
    var pF:[UInt8]?=F
    var pPERMIT:[UInt8]?=PERMIT

    var REALRNG : RAND? = rng

    if date != 0
    {
        if (!PINERROR)
        {
            pxID=nil;  // problem here - either comment out here or dont use with ! later on
   //         pHID=nil;
        }
    }
    else
    {
        pPERMIT=nil;
        pxCID=nil;
        pHTID=nil;
    }
    if (!PINERROR)
    {
        pE=nil;
        pF=nil;
    }

    if (SINGLE_PASS)
    {
        print("MPIN Single Pass")
        let timeValue = MPIN192.GET_TIME()

        rtn=MPIN192.CLIENT(sha,date,CLIENT_ID,&REALRNG,&X,pin,TOKEN,&SEC,&pxID,&pxCID,pPERMIT,timeValue,&Y)

        if rtn != 0 {print("FAILURE: CLIENT rtn: \(rtn)")}

        if (FULL)
        {
            HCID=MPIN192.HASH_ID(sha,CLIENT_ID);
            MPIN192.GET_G1_MULTIPLE(&REALRNG,1,&R,HCID,&Z); // Also Send Z=r.ID to Server, remember random r
        }
        rtn=MPIN192.SERVER(sha,date,&pHID,&pHTID,&Y,SST,pxID,pxCID!,SEC,&pE,&pF,CLIENT_ID,timeValue)
        if rtn != 0 {print("FAILURE: SERVER rtn: \(rtn)")}

        if (FULL)
        { // Also send T=w.ID to client, remember random w
            HSID=MPIN192.HASH_ID(sha,CLIENT_ID);
            if date != 0 {MPIN192.GET_G1_MULTIPLE(&REALRNG,0,&W,pHTID!,&T)}
            else {MPIN192.GET_G1_MULTIPLE(&REALRNG,0,&W,pHID,&T)}

        }
    }
    else
    {
        print("MPIN Multi Pass");
        // Send U=x.ID to server, and recreate secret from token and pin
        rtn=MPIN192.CLIENT_1(sha,date,CLIENT_ID,&REALRNG,&X,pin,TOKEN,&SEC,&pxID,&pxCID,pPERMIT)
        if rtn != 0 {print("FAILURE: CLIENT_1 rtn: \(rtn)")}

        if (FULL)
        {
            HCID=MPIN192.HASH_ID(sha,CLIENT_ID);
            MPIN192.GET_G1_MULTIPLE(&REALRNG,1,&R,HCID,&Z);  // Also Send Z=r.ID to Server, remember random r
        }
        // Server calculates H(ID) and H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp.
        MPIN192.SERVER_1(sha,date,CLIENT_ID,&pHID,&pHTID);
            // Server generates Random number Y and sends it to Client
        MPIN192.RANDOM_GENERATE(&REALRNG!,&Y);

        if (FULL)
        { // Also send T=w.ID to client, remember random w
            HSID=MPIN192.HASH_ID(sha,CLIENT_ID);
            if date != 0 {MPIN192.GET_G1_MULTIPLE(&REALRNG,0,&W,pHTID!,&T)}
            else {MPIN192.GET_G1_MULTIPLE(&REALRNG,0,&W,pHID,&T)}
        }

        // Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
        rtn=MPIN192.CLIENT_2(X,Y,&SEC);
        if rtn != 0 {print("FAILURE: CLIENT_2 rtn: \(rtn)")}

        // Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error.
        // If PIN error not required, set E and F = null

        rtn=MPIN192.SERVER_2(date,pHID,pHTID,Y,SST,pxID,pxCID,SEC,&pE,&pF);

        if rtn != 0 {print("FAILURE: SERVER_1 rtn: \(rtn)")}
    }
    if (rtn == MPIN192.BAD_PIN)
    {
        print("Server says - Bad Pin. I don't know you. Feck off.\n");
        if (PINERROR)
        {
            let err=MPIN192.KANGAROO(pE,pF);
            if err != 0 {print("(Client PIN is out by \(err))\n")}
        }
        return;
    }
    else {print("Server says - PIN is good! You really are "+IDstr)}

    if (FULL)
    {
        var H=MPIN192.HASH_ALL(sha,HCID,pxID,pxCID,SEC,Y,Z,T);
        MPIN192.CLIENT_KEY(sha,G1,G2,pin,R,X,H,T,&CK);
        print("Client Key =  0x",terminator: "");  printBinary(CK)

        H=MPIN192.HASH_ALL(sha,HSID,pxID,pxCID,SEC,Y,Z,T);
        MPIN192.SERVER_KEY(sha,Z,SST,W,H,pHID,pxID!,pxCID,&SK);
        print("Server Key =  0x",terminator: "");  printBinary(SK)
    }
    rng=REALRNG!
}

public func TestMPIN_bls48556(_ rng: inout RAND)
{
    let PERMITS=true
    let PINERROR=true
    let FULL=true
    let SINGLE_PASS=true


    let EGS=bls48556.MPIN256.EGS
    let EFS=bls48556.MPIN256.EFS
    let G1S=2*EFS+1    // Group 1 Size
    let G2S=16*EFS+1;     // Group 2 Size
    let EAS=bls48556.CONFIG_CURVE.AESKEY

    let sha=bls48556.CONFIG_CURVE.HASH_TYPE

    var S=[UInt8](repeating: 0,count: EGS)
    var SST=[UInt8](repeating: 0,count: G2S)
    var TOKEN=[UInt8](repeating: 0,count: G1S)
    var PERMIT=[UInt8](repeating: 0,count: G1S)
    var SEC=[UInt8](repeating: 0,count: G1S)
    let xID=[UInt8](repeating: 0,count: G1S)
    let xCID=[UInt8](repeating: 0,count: G1S)
    var X=[UInt8](repeating: 0,count: EGS)
    var Y=[UInt8](repeating: 0,count: EGS)
    let E=[UInt8](repeating: 0,count: 48*EFS)
    let F=[UInt8](repeating: 0,count: 48*EFS)
    let HID=[UInt8](repeating: 0,count: G1S)
    let HTID=[UInt8](repeating: 0,count: G1S)

    var G1=[UInt8](repeating: 0,count: 48*EFS)
    var G2=[UInt8](repeating: 0,count: 48*EFS)
    var R=[UInt8](repeating: 0,count: EGS)
    var Z=[UInt8](repeating: 0,count: G1S)
    var W=[UInt8](repeating: 0,count: EGS)
    var T=[UInt8](repeating: 0,count: G1S)
    var CK=[UInt8](repeating: 0,count: EAS)
    var SK=[UInt8](repeating: 0,count: EAS)

    var HSID=[UInt8]()

    // Trusted Authority set-up

    MPIN256.RANDOM_GENERATE(&rng,&S)
    print("\nTest Curve bls48556");
    print("MPIN Master Secret s: 0x",terminator: "");  printBinary(S)

    // Create Client Identity
    let IDstr = "testUser@miracl.com"
    let CLIENT_ID=[UInt8](IDstr.utf8)

    var HCID=MPIN256.HASH_ID(sha,CLIENT_ID)  // Either Client or TA calculates Hash(ID) - you decide!

    print("Client ID= "); printBinary(CLIENT_ID)

    // Client and Server are issued secrets by DTA
    MPIN256.GET_SERVER_SECRET(S,&SST);
    print("Server Secret SS: 0x",terminator: "");  printBinary(SST);

    MPIN256.GET_CLIENT_SECRET(&S,HCID,&TOKEN);
    print("Client Secret CS: 0x",terminator: ""); printBinary(TOKEN);

    // Client extracts PIN from secret to create Token
    var pin:Int32=1234
    print("Client extracts PIN= \(pin)")
    var rtn=MPIN256.EXTRACT_PIN(sha,CLIENT_ID,pin,&TOKEN)
    if rtn != 0 {print("FAILURE: EXTRACT_PIN rtn: \(rtn)")}

    print("Client Token TK: 0x",terminator: ""); printBinary(TOKEN);

    if FULL
    {
        MPIN256.PRECOMPUTE(TOKEN,HCID,&G1,&G2);
    }

    var date:Int32=0
    if (PERMITS)
    {
        date=MPIN256.today()
        // Client gets "Time Token" permit from DTA
        MPIN256.GET_CLIENT_PERMIT(sha,date,S,HCID,&PERMIT)
        print("Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)

        // This encoding makes Time permit look random - Elligator squared
        MPIN256.ENCODING(&rng,&PERMIT);
        print("Encoded Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)
        MPIN256.DECODING(&PERMIT)
        print("Decoded Time Permit TP: 0x",terminator: "");  printBinary(PERMIT)
    }

    // ***** NOW ENTER PIN *******

        pin=1234

    // **************************

    // Set date=0 and PERMIT=null if time permits not in use

    //Client First pass: Inputs CLIENT_ID, optional rng, pin, TOKEN and PERMIT. Output xID =x .H(CLIENT_ID) and re-combined secret SEC
    //If PERMITS are is use, then date!=0 and PERMIT is added to secret and xCID = x.(H(CLIENT_ID)+H(date|H(CLIENT_ID)))
    //Random value x is supplied externally if RNG=null, otherwise generated and passed out by RNG

    //IMPORTANT: To save space and time..
    //If Time Permits OFF set xCID = null, HTID=null and use xID and HID only
    //If Time permits are ON, AND pin error detection is required then all of xID, xCID, HID and HTID are required
    //If Time permits are ON, AND pin error detection is NOT required, set xID=null, HID=null and use xCID and HTID only.


    var pxID:[UInt8]?=xID
    var pxCID:[UInt8]?=xCID
    var pHID:[UInt8]=HID
    var pHTID:[UInt8]?=HTID
    var pE:[UInt8]?=E
    var pF:[UInt8]?=F
    var pPERMIT:[UInt8]?=PERMIT

    var REALRNG : RAND? = rng

    if date != 0
    {
        if (!PINERROR)
        {
            pxID=nil;  // problem here - either comment out here or dont use with ! later on
   //         pHID=nil;
        }
    }
    else
    {
        pPERMIT=nil;
        pxCID=nil;
        pHTID=nil;
    }
    if (!PINERROR)
    {
        pE=nil;
        pF=nil;
    }

    if (SINGLE_PASS)
    {
        print("MPIN Single Pass")
        let timeValue = MPIN256.GET_TIME()

        rtn=MPIN256.CLIENT(sha,date,CLIENT_ID,&REALRNG,&X,pin,TOKEN,&SEC,&pxID,&pxCID,pPERMIT,timeValue,&Y)

        if rtn != 0 {print("FAILURE: CLIENT rtn: \(rtn)")}

        if (FULL)
        {
            HCID=MPIN256.HASH_ID(sha,CLIENT_ID);
            MPIN256.GET_G1_MULTIPLE(&REALRNG,1,&R,HCID,&Z); // Also Send Z=r.ID to Server, remember random r
        }
        rtn=MPIN256.SERVER(sha,date,&pHID,&pHTID,&Y,SST,pxID,pxCID!,SEC,&pE,&pF,CLIENT_ID,timeValue)
        if rtn != 0 {print("FAILURE: SERVER rtn: \(rtn)")}

        if (FULL)
        { // Also send T=w.ID to client, remember random w
            HSID=MPIN256.HASH_ID(sha,CLIENT_ID);
            if date != 0 {MPIN256.GET_G1_MULTIPLE(&REALRNG,0,&W,pHTID!,&T)}
            else {MPIN256.GET_G1_MULTIPLE(&REALRNG,0,&W,pHID,&T)}

        }
    }
    else
    {
        print("MPIN Multi Pass");
        // Send U=x.ID to server, and recreate secret from token and pin
        rtn=MPIN256.CLIENT_1(sha,date,CLIENT_ID,&REALRNG,&X,pin,TOKEN,&SEC,&pxID,&pxCID,pPERMIT)
        if rtn != 0 {print("FAILURE: CLIENT_1 rtn: \(rtn)")}

        if (FULL)
        {
            HCID=MPIN256.HASH_ID(sha,CLIENT_ID);
            MPIN256.GET_G1_MULTIPLE(&REALRNG,1,&R,HCID,&Z);  // Also Send Z=r.ID to Server, remember random r
        }
        // Server calculates H(ID) and H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp.
        MPIN256.SERVER_1(sha,date,CLIENT_ID,&pHID,&pHTID);
            // Server generates Random number Y and sends it to Client
        MPIN256.RANDOM_GENERATE(&REALRNG!,&Y);

        if (FULL)
        { // Also send T=w.ID to client, remember random w
            HSID=MPIN256.HASH_ID(sha,CLIENT_ID);
            if date != 0 {MPIN256.GET_G1_MULTIPLE(&REALRNG,0,&W,pHTID!,&T)}
            else {MPIN256.GET_G1_MULTIPLE(&REALRNG,0,&W,pHID,&T)}
        }

        // Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
        rtn=MPIN256.CLIENT_2(X,Y,&SEC);
        if rtn != 0 {print("FAILURE: CLIENT_2 rtn: \(rtn)")}

        // Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error.
        // If PIN error not required, set E and F = null

        rtn=MPIN256.SERVER_2(date,pHID,pHTID,Y,SST,pxID,pxCID,SEC,&pE,&pF);

        if rtn != 0 {print("FAILURE: SERVER_1 rtn: \(rtn)")}
    }
    if (rtn == MPIN256.BAD_PIN)
    {
        print("Server says - Bad Pin. I don't know you. Feck off.\n");
        if (PINERROR)
        {
            let err=MPIN256.KANGAROO(pE,pF);
            if err != 0 {print("(Client PIN is out by \(err))\n")}
        }
        return;
    }
    else {print("Server says - PIN is good! You really are "+IDstr)}

    if (FULL)
    {
        var H=MPIN256.HASH_ALL(sha,HCID,pxID,pxCID,SEC,Y,Z,T);
        MPIN256.CLIENT_KEY(sha,G1,G2,pin,R,X,H,T,&CK);
        print("Client Key =  0x",terminator: "");  printBinary(CK)

        H=MPIN256.HASH_ALL(sha,HSID,pxID,pxCID,SEC,Y,Z,T);
        MPIN256.SERVER_KEY(sha,Z,SST,W,H,pHID,pxID!,pxCID,&SK);
        print("Server Key =  0x",terminator: "");  printBinary(SK)
    }
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
