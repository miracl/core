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
//  sha3.swift - Implementation of SHA3
//
//  Created by Michael Scott on 7/08/2017.
//

import Foundation

public struct SHA3{

    	private var length:UInt64
	private var rate:Int
	private var len:Int
	private var s=[[UInt64]](repeating:[UInt64](repeating:0,count:5),count:5)

    	static let RC:[UInt64]=[
		0x0000000000000001,0x0000000000008082,0x800000000000808A,0x8000000080008000,
		0x000000000000808B,0x0000000080000001,0x8000000080008081,0x8000000000008009,
		0x000000000000008A,0x0000000000000088,0x0000000080008009,0x000000008000000A,
		0x000000008000808B,0x800000000000008B,0x8000000000008089,0x8000000000008003,
		0x8000000000008002,0x8000000000000080,0x000000000000800A,0x800000008000000A,
		0x8000000080008081,0x8000000000008080,0x0000000080000001,0x8000000080008008]

	static public let HASH224:Int=28
	static public let HASH256:Int=32
	static public let HASH384:Int=48
	static public let HASH512:Int=64
	static public let SHAKE128:Int=16
	static public let SHAKE256:Int=32
    	
	static let ROUNDS:Int=24
    
    	private static func rotl(_ x: UInt64,_ n: UInt32) -> UInt64
    	{
        	return ((x<<UInt64(n))|(x>>(64-UInt64(n))))
    	}
   
    	private mutating func transform()
    	{ /* basic transformation step */
        	var c=[UInt64](repeating:0,count:5)
		var d=[UInt64](repeating:0,count:5)
		var b=[[UInt64]](repeating:[UInt64](repeating:0,count:5),count:5)
	
		for k in 0 ..< SHA3.ROUNDS 
		{
			c[0]=s[0][0]^s[0][1]^s[0][2]^s[0][3]^s[0][4]
			c[1]=s[1][0]^s[1][1]^s[1][2]^s[1][3]^s[1][4]
			c[2]=s[2][0]^s[2][1]^s[2][2]^s[2][3]^s[2][4]
			c[3]=s[3][0]^s[3][1]^s[3][2]^s[3][3]^s[3][4]
			c[4]=s[4][0]^s[4][1]^s[4][2]^s[4][3]^s[4][4]

			d[0]=c[4]^SHA3.rotl(c[1],1)
			d[1]=c[0]^SHA3.rotl(c[2],1)
			d[2]=c[1]^SHA3.rotl(c[3],1)
			d[3]=c[2]^SHA3.rotl(c[4],1)
			d[4]=c[3]^SHA3.rotl(c[0],1)

			for i in 0 ..< 5 {
				for j in 0 ..< 5 {
					s[i][j]^=d[i]
				}
			}
		
			b[0][0]=s[0][0]
			b[1][3]=SHA3.rotl(s[0][1],36)
			b[2][1]=SHA3.rotl(s[0][2],3)
			b[3][4]=SHA3.rotl(s[0][3],41)
			b[4][2]=SHA3.rotl(s[0][4],18)

			b[0][2]=SHA3.rotl(s[1][0],1)
			b[1][0]=SHA3.rotl(s[1][1],44)
			b[2][3]=SHA3.rotl(s[1][2],10)
			b[3][1]=SHA3.rotl(s[1][3],45)
			b[4][4]=SHA3.rotl(s[1][4],2)

			b[0][4]=SHA3.rotl(s[2][0],62)
			b[1][2]=SHA3.rotl(s[2][1],6)
			b[2][0]=SHA3.rotl(s[2][2],43)
			b[3][3]=SHA3.rotl(s[2][3],15)
			b[4][1]=SHA3.rotl(s[2][4],61)

			b[0][1]=SHA3.rotl(s[3][0],28)
			b[1][4]=SHA3.rotl(s[3][1],55)
			b[2][2]=SHA3.rotl(s[3][2],25)
			b[3][0]=SHA3.rotl(s[3][3],21)
			b[4][3]=SHA3.rotl(s[3][4],56)

			b[0][3]=SHA3.rotl(s[4][0],27)
			b[1][1]=SHA3.rotl(s[4][1],20)
			b[2][4]=SHA3.rotl(s[4][2],39)
			b[3][2]=SHA3.rotl(s[4][3],8)
			b[4][0]=SHA3.rotl(s[4][4],14)

			for i in 0 ..< 5 {
				for j in 0 ..< 5 {
					s[i][j]=b[i][j]^(~b[(i+1)%5][j]&b[(i+2)%5][j])
				}
			}
			s[0][0]^=SHA3.RC[k]
		}
    	}
    
    /* Re-Initialise Hash function */
    	mutating func init_it(_ olen: Int)
    	{ /* initialise */
		len=olen
		rate=200-2*olen 
    	}
    
    	public init(_ olen: Int)
    	{
      		for i in 0 ..< 5 {
			for j in 0..<5 {
				s[i][j]=0
			}
		}
        	length=0
		len=0
		rate=0 
        	init_it(olen)
    	}
    
    /* process a single byte */
    	public mutating func process(_ byt: UInt8)
    	{ /* process the next message byte */
		let cnt=Int(length%UInt64(rate))
		let b=cnt%8
		let ind=cnt/8
		let i=ind%5
		let j=ind/5
		s[i][j]^=(UInt64(byt)&0xff)<<UInt64(8*b)
		length+=1
		if cnt+1 == rate {transform()}
    	}

	public mutating func squeeze(_ buff:inout [UInt8],_ olen: Int)
	{
		var done=false
		var m: Int = 0
	/* extract by columns */
		while true {
			for j in 0 ..< 5 {
				for i in 0 ..< 5 
				{
					var el=s[i][j]
					for _ in 0 ..< 8 
					{
						buff[m]=UInt8(el&0xff)
						m+=1
						if m>=olen || (m%rate)==0 {
							done=true
							break
						}
						el>>=8
					}
					if done {break}
				}
				if done {break}
			}
			if m>=olen {break}
			done=false
			transform()
		}		
	}
    
    /* Generate Hash */
	public mutating func hash(_ digest:inout [UInt8])
    	{ /* pad message and finish - supply digest */
        	let q=rate-Int(length%UInt64(rate))
		if q==1 {process(0x86)}
		else {
			process(0x06)
			while Int(length%UInt64(rate)) != rate-1 {process(0x00)}
			process(0x80)
		}
		squeeze(&digest,len)
	}

    public mutating func shake(_ digest:inout [UInt8],_ olen: Int)
	{
		let q=rate-Int(length%UInt64(rate))
		if q==1 {process(0x9f)}
		else {
			process(0x1f)
			while Int(length%UInt64(rate)) != rate-1 {process(0x00)}
			process(0x80)
		}
 		squeeze(&digest,olen)
	}   
}

//916f6061fe879741ca6469b43971dfdb28b1a32dc36cb3254e812be27aad1d18
//afebb2ef542e6579c50cad06d2e578f9f8dd6881d7dc824d26360feebf18a4fa73e3261122948efcfd492e74e82e2189ed0fb440d187f382270cb455f21dd185
//98be04516c04cc73593fef3ed0352ea9f6443942d6950e29a372a681c3deaf4535423709b02843948684e029010badcc0acd8303fc85fdad3eabf4f78cae165635f57afd28810fc2

/*
public func TestSHA3()
{
    	let test=String("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu")
    
    	var digest=[UInt8](repeating: 0,count: 172)
    
    	let t=[UInt8]( (test!).utf8)

	var sh=SHA3(SHA3.HASH256)
	for i in 0..<t.count {
		sh.process(t[i])
	}
	sh.hash(&digest)

       	for i in 0 ..< 32
        {
            let h=String(digest[i],radix:16)
            print("\(h)", terminator: "")
        }
        print("")
	    
	sh=SHA3(SHA3.HASH512)
	for i in 0..<t.count {
		sh.process(t[i])
	}
	sh.hash(&digest)

       	for i in 0 ..< 64
        {
            let h=String(digest[i],radix:16)
            print("\(h)", terminator: "")
        }
        print("")

	sh=SHA3(SHA3.SHAKE256)
	for i in 0..<t.count {
		sh.process(t[i])
	}
	sh.shake(&digest,72)

       	for i in 0 ..< 72
        {
            let h=String(digest[i],radix:16)
            print("\(h)", terminator: "")
        }
        print("")
}

TestSHA3()  */
