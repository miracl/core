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

/* test driver and function exerciser for BLS API Functions */
package org.miracl.core.XXX;  //

import junit.framework.TestCase;      //
import org.miracl.core.RAND;

public class TestBLS192 extends TestCase //
{
	private static void printBinary(byte[] array)
	{
		int i;
		for (i=0;i<array.length;i++)
		{
			System.out.printf("%02x", array[i]);
		}
		System.out.println();
	}    

	public static void testBLS192()
	{
		RAND rng=new RAND();
		int BGS=BLS192.BGS;
		int BFS=BLS192.BFS;
		int G1S=BFS+1; /* Group 1 Size */
		int G2S=8*BFS; /* Group 2 Size */

		byte[] S = new byte[BGS];
		byte[] W = new byte[G2S];
		byte[] SIG = new byte[G1S];

		byte[] RAW=new byte[100];

		rng.clean();
		for (int i=0;i<100;i++) RAW[i]=(byte)(i);
		rng.seed(100,RAW);

		System.out.println("\nTesting BLS code");

		BLS192.init();

		String mess=new String("This is a test message");

		BLS192.KeyPairGenerate(rng,S,W);
		System.out.print("Private key : 0x");  printBinary(S);
		System.out.print("Public  key : 0x");  printBinary(W);

		BLS192.sign(SIG,mess,S);
		System.out.print("Signature : 0x");  printBinary(SIG);

		int res=BLS192.verify(SIG,mess,W);

		if (res==0)
			System.out.println("Signature is OK");
		else
			fail("Signature is *NOT* OK");
 
	}
}
