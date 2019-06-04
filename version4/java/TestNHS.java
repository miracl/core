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

/* test driver and function exerciser for NewHope API Functions */

// See https://eprint.iacr.org/2016/1157 (Alkim, Ducas, Popplemann and Schwabe)

// javac -cp amcl.jar; TestNHS.java
// java -cp amcl.jar; TestNHS

package org.apache.milagro;

import org.apache.milagro.amcl.RAND;
import org.apache.milagro.amcl.NHS;

public class TestNHS
{	
		public static void main(String[] args) {
		int i;
		byte[] RAW=new byte[100];
		byte[] S=new byte[1792];
		byte[] SB=new byte[1824];
		byte[] UC=new byte[2176];
		byte[] KEYA=new byte[32];
		byte[] KEYB=new byte[32];

		RAND SRNG=new RAND();
		RAND CRNG=new RAND();
		SRNG.clean(); CRNG.clean();

		for (i=0;i<100;i++) RAW[i]=(byte)(i+1);
		SRNG.seed(100,RAW);

										for (i=0;i<100;i++) RAW[i]=(byte)(i+2);
										CRNG.seed(100,RAW);

// NewHope Simple key exchange

		NHS.SERVER_1(SRNG,SB,S);
										NHS.CLIENT(CRNG,SB,UC,KEYB);
		NHS.SERVER_2(S,UC,KEYA);

		System.out.printf("Alice key= 0x");
		for (i=0;i<KEYA.length;i++)
			System.out.printf("%02x", KEYA[i]);
		System.out.println();


										System.out.printf("Bob's key= 0x");
										for (i=0;i<KEYA.length;i++)
											System.out.printf("%02x", KEYB[i]);
										System.out.println();
	}
}