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


use crate::hash256::HASH256;
use crate::hash384::HASH384;
use crate::hash512::HASH512;
use crate::sha3::SHA3;

pub const MC_SHA2: usize = 2;
pub const MC_SHA3: usize = 3;
pub const SHA256: usize = 32;
pub const SHA384: usize = 48;
pub const SHA512: usize = 64;

#[allow(non_snake_case)]

/* General Purpose Hash function */

pub fn GPhashit(hash: usize, sha: usize,w: &mut [u8],pad: usize,a: Option<&[u8]>, n: isize, b: Option<&[u8]>) {
    let mut r: [u8; 64] = [0; 64];

    if hash == MC_SHA2 {
        if sha == SHA256 {
            let mut h = HASH256::new();
            if let Some(x) = a {
                h.process_array(x);
            }
            if n >= 0 {
                h.process_num(n as i32)
            }
            if let Some(x) = b {
                h.process_array(x);
            }
            let hs = h.hash();
            for i in 0..sha {
                r[i] = hs[i];
            }
        }
        if sha == SHA384 {
            let mut h = HASH384::new();
            if let Some(x) = a {
                h.process_array(x);
            }
            if n >= 0 {
                h.process_num(n as i32)
            }
            if let Some(x) = b {
                h.process_array(x);
            }
            let hs = h.hash();
            for i in 0..sha {
                r[i] = hs[i];
            }
        }
        if sha == SHA512 {
            let mut h = HASH512::new();
            if let Some(x) = a {
                h.process_array(x);
            }
            if n >= 0 {
                h.process_num(n as i32)
            }
            if let Some(x) = b {
                h.process_array(x);
            }
            let hs = h.hash();
            for i in 0..sha {
                r[i] = hs[i];
            }
        }
    }
    if hash == MC_SHA3 {
        let mut h = SHA3::new(sha);
        if let Some(x) = a {
            h.process_array(x);
        }
        if n >= 0 {
            h.process_num(n as i32)
        }
        if let Some(x) = b {
            h.process_array(x);
        }
        h.hash(&mut r);
    }

    if pad == 0 {
        for i in 0..sha {
            w[i] = r[i]
        }
    } else {
        if pad <= sha {
            for i in 0..pad {
                w[i] = r[i]
            }
        } else {
            for i in 0..sha {
                w[i + pad - sha] = r[i]
            }
            for i in 0..(pad - sha) {
                w[i] = 0
            }
        }
    }
}

#[allow(non_snake_case)]
pub fn SPhashit(hash: usize, sha: usize,w: &mut [u8],a: Option<&[u8]>) {
    GPhashit(hash,sha,w,0,a,-1,None);
}

pub fn inttobytes(n: usize, b: &mut [u8]) {
    let mut i = b.len();
    let mut m = n;
    while m > 0 && i > 0 {
        i -= 1;
        b[i] = (m & 0xff) as u8;
        m /= 256;
    }
}

pub fn kdf2(hash: usize, sha: usize, z: &[u8], p: Option<&[u8]>, olen: usize, k: &mut [u8]) {
    /* NOTE: the parameter olen is the length of the output K in bytes */
    let hlen = sha;
    let mut lk = 0;

    let mut cthreshold = olen / hlen;
    if olen % hlen != 0 {
        cthreshold += 1
    }

    for counter in 1..cthreshold + 1 {
        let mut b: [u8; 64] = [0; 64];
        GPhashit(hash, sha, &mut b,0,Some(z), counter as isize, p);
        if lk + hlen > olen {
            for i in 0..(olen % hlen) {
                k[lk] = b[i];
                lk += 1
            }
        } else {
            for i in 0..hlen {
                k[lk] = b[i];
                lk += 1
            }
        }
    }
}

/* Password based Key Derivation Function */
/* Input password p, salt s, and repeat count */
/* Output key of length olen */
pub fn pbkdf2(hash: usize, sha: usize, pass: &[u8], salt: &[u8], rep: usize, olen: usize, k: &mut [u8]) {
    let mut d = olen / sha;
    if olen % sha != 0 {
        d += 1
    }
    let mut f: [u8; 64] = [0; 64];
    let mut u: [u8; 64] = [0; 64];
    let mut ku: [u8; 64] = [0; 64];
    let mut s: [u8; 36] = [0; 36]; // Maximum salt of 32 bytes + 4
    let mut n: [u8; 4] = [0; 4];

    let sl = salt.len();
    let mut kp = 0;
    for i in 0..d {
        for j in 0..sl {
            s[j] = salt[j]
        }
        inttobytes(i + 1, &mut n);
        for j in 0..4 {
            s[sl + j] = n[j]
        }

        hmac1(hash, sha, &mut f, sha, pass, &s[0..sl + 4]);

        for j in 0..sha {
            u[j] = f[j]
        }
        for _ in 1..rep {
            hmac1(hash, sha, &mut ku, sha, pass, &mut u);
            for k in 0..sha {
                u[k] = ku[k];
                f[k] ^= u[k]
            }
        }
        for j in 0..sha {
            if kp < olen {
                k[kp] = f[j]
            }
            kp += 1
        }
    }
}

/* Calculate HMAC of m using key k. HMAC is tag of length olen (which is length of tag) */
pub fn hmac1(hash: usize, sha: usize, tag: &mut [u8], olen: usize, k: &[u8], m: &[u8]) -> bool {
    /* Input is from an octet m        *
    	* olen is requested output length in bytes. k is the key  *
    	* The output is the calculated tag */
    let mut b: [u8; 64] = [0; 64]; /* Not good */
    let mut k0: [u8; 128] = [0; 128];

    let mut lb=0;
    if hash == MC_SHA2 {
        lb=64;
        if sha > 32 {
            lb=128;
        }
    }
    if hash == MC_SHA3 {
        lb=200-2*sha;
    }

    if lb == 0 {
        return false;
    }

    for i in 0..lb {
        k0[i] = 0
    }

    if k.len() > lb {
        SPhashit(hash,sha,&mut b,Some(k));
        //GPhashit(hash, sha, &mut b,0,k, 0, None);
        for i in 0..sha {
            k0[i] = b[i]
        }
    } else {
        for i in 0..k.len() {
            k0[i] = k[i]
        }
    }

    for i in 0..lb {
        k0[i] ^= 0x36
    }
    GPhashit(hash, sha, &mut b,0,Some(&mut k0[0..lb]), -1, Some(m));

    for i in 0..lb {
        k0[i] ^= 0x6a
    }
    GPhashit(hash, sha, tag,olen,Some(&mut k0[0..lb]), -1, Some(&b[0..sha]));

    return true;
}

pub fn hkdf_extract(hash: usize, hlen: usize, prk: &mut [u8],salt: Option<&[u8]>,ikm: &[u8]) {
    if let Some(x)=salt {
        hmac1(hash,hlen,prk,hlen,x,ikm);
    } else {
        let h: [u8; 64] = [0; 64];
        hmac1(hash,hlen,prk,hlen,&h[0..hlen],ikm);
    }   
}

pub fn hkdf_expand(hash: usize, hlen: usize, okm: &mut [u8], olen: usize, prk: &[u8], info: &[u8]) {
    let n=olen/hlen;
    let flen=olen%hlen;

    let mut t: [u8; 200] = [0; 200];
    let mut k: [u8; 64] = [0; 64];

    let mut l=0;
    let mut m=0;
    for i in 1..=n {
        for j in 0..info.len() {
            t[l]=info[j]; l=l+1;
        }
        t[l]=i as u8; l=l+1;
        hmac1(hash,hlen,&mut k,hlen,prk,&t[0..l]);
        l=0;
        for j in 0..hlen {
            okm[m]=k[j]; m=m+1;
            t[l]=k[j]; l=l+1;
        }
    }
    if flen>0 {
        for j in 0..info.len() {
            t[l]=info[j]; l=l+1;
        }
        t[l]=(n+1) as u8; l=l+1;
        hmac1(hash,hlen,&mut k,flen,prk,&t[0..l]);
        for j in 0..flen {
            okm[m]=k[j]; m=m+1;
        }
    }
}

/*

    let mut ikm: [u8;22]=[0;22];
    let mut salt: [u8;13]=[0;13];
    let mut info: [u8;10]=[0;10];
    let mut prk: [u8;32]=[0;32];
    let mut okm: [u8;42]=[0;42];

    for i in 0..22 {ikm[i]=0x0b;}
    for i in 0..13 {salt[i]=i as u8;}
    for i in 0..10 {info[i]=(0xf0+i) as u8;}

    hmac::hkdf_extract(hmac::MC_SHA2,32,&mut prk,Some(&salt),&ikm);

    print!("PRK= ");
    for i in 0..32 {
        print!("{:02X}",prk[i]);
    }

    hmac::hkdf_expand(hmac::MC_SHA2,32,&mut okm,42,&prk,&info);
    print!("OKM= ");
    for i in 0..42 {
        print!("{:02X}",okm[i]);
    }


*/

