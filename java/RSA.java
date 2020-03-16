/*
   Copyright (C) 2019 MIRACL UK Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

     https://www.gnu.org/licenses/agpl-3.0.en.html

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing     
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.     
*/

/* RSA API high-level functions  */

package org.miracl.core.XXX;

import org.miracl.core.RAND;
import org.miracl.core.HMAC;

public final class RSA {

	public static final int RFS=CONFIG_BIG.MODBYTES*CONFIG_FF.FFLEN;
	public static final int SHA256=32;
	public static final int SHA384=48;
	public static final int SHA512=64;

	public static final int HASH_TYPE=SHA256;

/* Hash number (optional) and string to array size of Bigs */

/* generate an RSA key pair */

	public static void KEY_PAIR(RAND rng,int e,private_key PRIV,public_key PUB)
	{ /* IEEE1363 A16.11/A16.12 more or less */

		int n=PUB.n.getlen()/2;
		FF t = new FF(n);
		FF p1=new FF(n);
		FF q1=new FF(n);

		for (;;)
		{
			PRIV.p.random(rng);
			while (PRIV.p.lastbits(2)!=3) PRIV.p.inc(1);
			while (!FF.prime(PRIV.p,rng)) PRIV.p.inc(4);

			p1.copy(PRIV.p);
			p1.dec(1);

			if (p1.cfactor(e)) continue;
			break;
		}

		for (;;)
		{
			PRIV.q.random(rng);
			while (PRIV.q.lastbits(2)!=3) PRIV.q.inc(1);
			while (!FF.prime(PRIV.q,rng)) PRIV.q.inc(4);
			
			q1.copy(PRIV.q);
			q1.dec(1);

			if (q1.cfactor(e)) continue;

			break;
		}

		PUB.n=FF.mul(PRIV.p,PRIV.q);
		PUB.e=e;

		t.copy(p1);
		t.shr();
		PRIV.dp.set(e);
		PRIV.dp.invmodp(t);
		if (PRIV.dp.parity()==0) PRIV.dp.add(t);
		PRIV.dp.norm();

		t.copy(q1);
		t.shr();
		PRIV.dq.set(e);
		PRIV.dq.invmodp(t);
		if (PRIV.dq.parity()==0) PRIV.dq.add(t);
		PRIV.dq.norm();

		PRIV.c.copy(PRIV.p);
		PRIV.c.invmodp(PRIV.q);

		return;
	}

/* Mask Generation Function */

	public static void MGF1(int sha,byte[] Z,int olen,byte[] K)
	{
		int hlen=sha;
		byte[] B;

		int counter,cthreshold,k=0;

		for (int i=0;i<K.length;i++) K[i]=0;

		cthreshold=olen/hlen; if (olen%hlen!=0) cthreshold++;
		for (counter=0;counter<cthreshold;counter++)
		{
			//B=hashit(sha,Z,counter);
            B=HMAC.GPhashit(HMAC.MC_SHA2,sha,0,0,Z,counter,null);
			if (k+hlen>olen) for (int i=0;i<olen%hlen;i++) K[k++]=B[i];
			else for (int i=0;i<hlen;i++) K[k++]=B[i];
		}	
	}

	public static void printBinary(byte[] array)
	{
		int i;
		for (i=0;i<array.length;i++)
		{
			System.out.printf("%02x", array[i]);
		}
		System.out.println();
	}  



/* SHAXXX identifier strings */
	private static final byte[] SHA256ID={0x30,0x31,0x30,0x0d,0x06,0x09,0x60,(byte)0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20};
	private static final byte[] SHA384ID={0x30,0x41,0x30,0x0d,0x06,0x09,0x60,(byte)0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02,0x05,0x00,0x04,0x30};
	private static final byte[] SHA512ID={0x30,0x51,0x30,0x0d,0x06,0x09,0x60,(byte)0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03,0x05,0x00,0x04,0x40};

/* PKCS 1.5 padding of a message to be signed */

	public static boolean PKCS15(int sha,byte[] m,byte[] w)
	{
		int olen=FF.FF_BITS/8;
		int i,hlen=sha;
		int idlen=19; 

		if (olen<idlen+hlen+10) return false;
		//byte[] H=hashit(sha,m,-1);
        byte[] H=HMAC.SPhashit(HMAC.MC_SHA2,sha,m);

		for (i=0;i<w.length;i++) w[i]=0;
		i=0;
		w[i++]=0;
		w[i++]=1;
		for (int j=0;j<olen-idlen-hlen-3;j++)
			w[i++]=(byte)0xff;
		w[i++]=0;


		if (hlen==SHA256) for (int j=0;j<idlen;j++) w[i++]=SHA256ID[j];
		if (hlen==SHA384) for (int j=0;j<idlen;j++) w[i++]=SHA384ID[j];
		if (hlen==SHA512) for (int j=0;j<idlen;j++) w[i++]=SHA512ID[j];

		for (int j=0;j<hlen;j++)
			w[i++]=H[j];

		return true;
	}


	/* OAEP Message Encoding for Encryption */
	public static byte[] OAEP_ENCODE(int sha,byte[] m,RAND rng,byte[] p)
	{ 
		int i,slen,olen=RFS-1;
		int mlen=m.length;
		int hlen,seedlen;
		byte[] f=new byte[RFS];

		hlen=sha;
		byte[] SEED=new byte[hlen];
		seedlen=hlen;

		if (mlen>olen-hlen-seedlen-1) return new byte[0]; 

		byte[] DBMASK=new byte[olen-seedlen];

		//byte[] h=hashit(sha,p,-1);
        byte[] h = HMAC.SPhashit(HMAC.MC_SHA2,sha,p);

		for (i=0;i<hlen;i++) f[i]=h[i];

		slen=olen-mlen-hlen-seedlen-1;      

		for (i=0;i<slen;i++) f[hlen+i]=0;
		f[hlen+slen]=1;
		for (i=0;i<mlen;i++) f[hlen+slen+1+i]=m[i];

		for (i=0;i<seedlen;i++) SEED[i]=(byte)rng.getByte();

		MGF1(sha,SEED,olen-seedlen,DBMASK);

		for (i=0;i<olen-seedlen;i++) DBMASK[i]^=f[i];

		MGF1(sha,DBMASK,seedlen,f);

		for (i=0;i<seedlen;i++) f[i]^=SEED[i];

		for (i=0;i<olen-seedlen;i++) f[i+seedlen]=DBMASK[i];

		/* pad to length RFS */
		int d=1;
		for (i=RFS-1;i>=d;i--)
			f[i]=f[i-d];
		for (i=d-1;i>=0;i--)
			f[i]=0;

		return f;
	}

	/* OAEP Message Decoding for Decryption */
	public static byte[] OAEP_DECODE(int sha,byte[] p,byte[] f)
	{
		int x,t;
		boolean comp;
		int i,k,olen=RFS-1;
		int hlen,seedlen;

		hlen=sha;
		byte[] SEED=new byte[hlen];
		seedlen=hlen;
		byte[] CHASH=new byte[hlen];
	
		if (olen<seedlen+hlen+1) return new byte[0];
		byte[] DBMASK=new byte[olen-seedlen];
		for (i=0;i<olen-seedlen;i++) DBMASK[i]=0;

		if (f.length<RFS)
		{
			int d=RFS-f.length;
			for (i=RFS-1;i>=d;i--)
				f[i]=f[i-d];
			for (i=d-1;i>=0;i--)
				f[i]=0;

		}

		//byte[] h=hashit(sha,p,-1);
        byte[] h = HMAC.SPhashit(HMAC.MC_SHA2,sha,p);
		for (i=0;i<hlen;i++) CHASH[i]=h[i];

		x=f[0];

		for (i=seedlen;i<olen;i++)
			DBMASK[i-seedlen]=f[i+1]; 

		MGF1(sha,DBMASK,seedlen,SEED);
		for (i=0;i<seedlen;i++) SEED[i]^=f[i+1];
		MGF1(sha,SEED,olen-seedlen,f);
		for (i=0;i<olen-seedlen;i++) DBMASK[i]^=f[i];

		comp=true;
		for (i=0;i<hlen;i++)
		{
			if (CHASH[i]!=DBMASK[i]) comp=false;
		}

		for (i=0;i<olen-seedlen-hlen;i++)
			DBMASK[i]=DBMASK[i+hlen];

		for (i=0;i<hlen;i++)
			SEED[i]=CHASH[i]=0;
		
		for (k=0;;k++)
		{
			if (k>=olen-seedlen-hlen) return new byte[0];
			if (DBMASK[k]!=0) break;
		}

		t=DBMASK[k];
		if (!comp || x!=0 || t!=0x01) 
		{
			for (i=0;i<olen-seedlen;i++) DBMASK[i]=0;
			return new byte[0];
		}

		byte[] r=new byte[olen-seedlen-hlen-k-1];

		for (i=0;i<olen-seedlen-hlen-k-1;i++)
			r[i]=DBMASK[i+k+1];
	
		for (i=0;i<olen-seedlen;i++) DBMASK[i]=0;

		return r;
	}

	/* destroy the Private Key structure */
	public static void PRIVATE_KEY_KILL(private_key PRIV)
	{
		PRIV.p.zero();
		PRIV.q.zero();
		PRIV.dp.zero();
		PRIV.dq.zero();
		PRIV.c.zero();
	}

	/* RSA encryption with the public key */
	public static void ENCRYPT(public_key PUB,byte[] F,byte[] G)
	{
		int n=PUB.n.getlen();
		FF f=new FF(n);
		FF.fromBytes(f,F);
		f.power(PUB.e,PUB.n);
		f.toBytes(G);
	}

	/* RSA decryption with the private key */
	public static void DECRYPT(private_key PRIV,byte[] G,byte[] F)
	{
		int n=PRIV.p.getlen();
		FF g=new FF(2*n);

		FF.fromBytes(g,G);
		FF jp=g.dmod(PRIV.p);
		FF jq=g.dmod(PRIV.q);

		jp.skpow(PRIV.dp,PRIV.p);
		jq.skpow(PRIV.dq,PRIV.q);

		g.zero();
		g.dscopy(jp);
		jp.mod(PRIV.q);
		if (FF.comp(jp,jq)>0) jq.add(PRIV.q);
		jq.sub(jp);
		jq.norm();

		FF t=FF.mul(PRIV.c,jq);
		jq=t.dmod(PRIV.q);

		t=FF.mul(jq,PRIV.p);
		g.add(t);
		g.norm();

		g.toBytes(F);
	}
}
