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

/* test driver and function exerciser for ECDH/ECIES/ECDSA API Functions */
package org.apache.milagro.amcl.XXX;

import java.util.Scanner;
import junit.framework.TestCase;
import org.apache.milagro.amcl.RAND;

public class TestRSA extends TestCase
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

	public static void testRSA()
	{
		byte[] RAW=new byte[100];
		RAND rng=new RAND();

		rng.clean();
		for (int i=0;i<100;i++) RAW[i]=(byte)(i);

		rng.seed(100,RAW);
		
		int i;
		int RFS=RSA.RFS;
		int sha=RSA.HASH_TYPE;

		String message="Hello World\n";

		public_key pub=new public_key(CONFIG_FF.FFLEN);
		private_key priv=new private_key(FF.HFLEN);

		byte[] ML=new byte[RFS];
		byte[] C=new byte[RFS];
		byte[] S=new byte[RFS];


		System.out.println("Testing RSA code");

		System.out.println("Generating public/private key pair");
		RSA.KEY_PAIR(rng,65537,priv,pub);

		byte[] M=message.getBytes();
		System.out.print("Encrypting test string\n");


		byte[] E=RSA.OAEP_ENCODE(sha,M,rng,null); /* OAEP encode message M to E  */

		if (E.length==0) {
			fail("Encoding failed");
		}

		RSA.ENCRYPT(pub,E,C);     /* encrypt encoded message */
		System.out.print("Ciphertext= 0x"); printBinary(C);

		System.out.print("Decrypting test string\n");
		RSA.DECRYPT(priv,C,ML); 
		byte[] MS=RSA.OAEP_DECODE(sha,null,ML); /* OAEP decode message  */

		message=new String(MS);
		System.out.print(message);

		System.out.println("Signing message");
		RSA.PKCS15(sha,M,C);

		RSA.DECRYPT(priv,C,S); /* create signature in S */ 

		System.out.print("Signature= 0x"); printBinary(S);

		RSA.ENCRYPT(pub,S,ML); 

		boolean cmp=true;
		if (C.length!=ML.length) cmp=false;
		else
		{
			for (int j=0;j<C.length;j++)
				if (C[j]!=ML[j]) cmp=false;
		}
		if (cmp) System.out.println("Signature is valid");
		else fail("Signature is INVALID");

		RSA.PRIVATE_KEY_KILL(priv);
		System.out.println("");

	}
}
