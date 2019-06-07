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

//
//  rand.swift
//
//  Created by Michael Scott on 17/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//
//  Cryptographic strong random number generator

/* Marsaglia & Zaman Random number generator constants */
public struct RAND {
    private static let NK:Int=21
    private static let NJ:Int=6
    private static let NV:Int=8
    private var ira=[UInt32](repeating: 0,count: NK)
    private var rndptr:Int=0
    private var borrow:UInt32=0
    private var pool_ptr:Int=0
    private var pool=[UInt8](repeating: 0,count: 32)
    
    public mutating func clean()
    {
        pool_ptr=0
        rndptr=0
        for i in 0 ..< 32 {pool[i]=0}
        for i in 0 ..< RAND.NK {ira[i]=0}
        borrow=0;
    }
    
    public init() {clean()}
    
    private mutating func sbrand() -> UInt32
    { /* Marsaglia & Zaman random number generator */
        rndptr+=1;
        if rndptr<RAND.NK {return ira[rndptr]}
        rndptr=0;
        var k=RAND.NK-RAND.NJ
        for i in 0 ..< RAND.NK
        {
            if k==RAND.NK {k=0}
            let t=ira[k];
            let pdiff=t &- ira[i] &- borrow
            if pdiff<t {borrow=0}
            if pdiff>t {borrow=1}
            ira[i]=pdiff
            k += 1;
        }
        return ira[0]
    }
    
    mutating func sirand(_ seed: UInt32)
    {
        var m:UInt32=1
        var s:UInt32=seed
        borrow=0;
        rndptr=0
        ira[0]^=s
        for i in 1 ..< RAND.NK
        { /* fill initialisation vector */
            let ipn=(RAND.NV*i)%RAND.NK
            ira[ipn]^=m
            let t=m
            m=s &- m
            s=t
        }
        for _ in 0 ..< 10000 {_ = sbrand()}
    }
    
    private mutating func fill_pool()
    {
        var sh=HASH256()
        for _ in 0 ..< 128 {sh.process(UInt8(sbrand()&0xff))}
        pool=sh.hash()
        pool_ptr=0
    }
    
    private mutating func pack(_ b: [UInt8]) -> UInt32
    {
	var r=UInt32(b[3])<<24
	r |=  UInt32(b[2])<<16
	r |=  UInt32(b[1])<<8
	r |=  UInt32(b[0])
	return r
        //return (UInt32(b[3])<<24)|(UInt32(b[2])<<16)|(UInt32(b[1])<<8)|(UInt32(b[0]))
    }
  
/* Initialize RNG with some real entropy from some external source */
    public mutating func seed(_ rawlen: Int,_ raw: [UInt8])
    { /* initialise from at least 128 byte string of raw random entropy */
        var digest=[UInt8]()
        var b=[UInt8](repeating: 0, count: 4)
        var sh=HASH256()
        pool_ptr=0
        for i in 0 ..< RAND.NK {ira[i]=0}
        if rawlen>0
        {
            for i in 0 ..< rawlen {sh.process(raw[i])}
            digest=sh.hash()
            
            for i in 0 ..< 8
            {
                b[0]=digest[4*i]; b[1]=digest[4*i+1]; b[2]=digest[4*i+2]; b[3]=digest[4*i+3]
                sirand(pack(b))
            }
            
        }
        fill_pool()
    }
    
    public mutating func getByte() -> UInt8
    {
        let r=pool[pool_ptr]; pool_ptr+=1
        if pool_ptr>=32 {fill_pool()}
        return r
    }
    
    
}
