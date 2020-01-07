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

extern crate core;

use std::str;

use core::rand::RAND;
use core::hmac;

pub fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}

fn ecdh_ed25519(mut rng: &mut RAND) {
    //use core::ed25519;
    use core::ed25519::ecdh;
    use core::ed25519::ecp;

    let pw = "M0ng00se";
    let pp: &[u8] = b"M0ng00se";
    const EFS: usize = ecdh::EFS;
    const EGS: usize = ecdh::EGS;
    const EAS: usize = ecp::AESKEY;

    let sha = ecp::HASH_TYPE;
    let mut salt: [u8; 8] = [0; 8];
    let mut s1: [u8; EGS] = [0; EGS];
    let mut w0: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut w1: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut z0: [u8; EFS] = [0; EFS];
    let mut z1: [u8; EFS] = [0; EFS];
    let mut key: [u8; EAS] = [0; EAS];
    let mut cs: [u8; EGS] = [0; EGS];
    let mut ds: [u8; EGS] = [0; EGS];
    let mut m: Vec<u8> = vec![0; 32]; // array that could be of any length. So use heap.
    let mut p1: [u8; 3] = [0; 3];
    let mut p2: [u8; 4] = [0; 4];
    let mut v: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut t: [u8; 12] = [0; 12];

    for i in 0..8 {
        salt[i] = (i + 1) as u8
    } // set Salt

    println!("\nTesting ECDH/ECDSA/ECIES");
    println!("Curve ed25519");
    println!("Alice's Passphrase= {}", pw);

    let mut s0: [u8; EFS] = [0; EGS];
    hmac::pbkdf2(hmac::MC_SHA2, sha, pp, &salt, 1000, EGS, &mut s0);

    print!("Alice's private key= 0x");
    printbinary(&s0);

    /* Generate Key pair S/W */
    ecdh::key_pair_generate(None, &mut s0, &mut w0);

    print!("Alice's public key= 0x");
    printbinary(&w0);

    let mut res = ecdh::public_key_validate(&w0);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }

    /* Random private key for other party */
    ecdh::key_pair_generate(Some(&mut rng), &mut s1, &mut w1);

    print!("Servers private key= 0x");
    printbinary(&s1);

    print!("Servers public key= 0x");
    printbinary(&w1);

    res = ecdh::public_key_validate(&w1);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }
    /* Calculate common key using DH - IEEE 1363 method */

    ecdh::ecpsvdp_dh(&s0, &w1, &mut z0);
    ecdh::ecpsvdp_dh(&s1, &w0, &mut z1);

    let mut same = true;
    for i in 0..EFS {
        if z0[i] != z1[i] {
            same = false
        }
    }

    if !same {
        println!("*** ECPSVDP-DH Failed");
        return;
    }

    hmac::kdf2(hmac::MC_SHA2, sha, &z0, None, EAS, &mut key);

    print!("Alice's DH Key=  0x");
    printbinary(&key);
    print!("Servers DH Key=  0x");
    printbinary(&key);

    if ecp::CURVETYPE != ecp::MONTGOMERY {
        for i in 0..17 {
            m[i] = i as u8
        }

        println!("Testing ECIES");

        p1[0] = 0x0;
        p1[1] = 0x1;
        p1[2] = 0x2;
        p2[0] = 0x0;
        p2[1] = 0x1;
        p2[2] = 0x2;
        p2[3] = 0x3;

        let cc = ecdh::ecies_encrypt(sha, &p1, &p2, &mut rng, &w1, &m[0..17], &mut v, &mut t);

        if let Some(mut c) = cc {
            println!("Ciphertext= ");
            print!("V= 0x");
            printbinary(&v);
            print!("C= 0x");
            printbinary(&c);
            print!("T= 0x");
            printbinary(&t);

            let mm = ecdh::ecies_decrypt(sha, &p1, &p2, &v, &mut c, &t, &s1);
            if let Some(rm) = mm {
                println!("Decryption succeeded");
                print!("Message is 0x");
                printbinary(&rm);
            } else {
                println!("*** ECIES Decryption Failed");
                return;
            }
        } else {
            println!("*** ECIES Encryption Failed");
            return;
        }

        println!("Testing ECDSA");

        if ecdh::ecpsp_dsa(sha, &mut rng, &s0, &m[0..17], &mut cs, &mut ds) != 0 {
            println!("***ECDSA Signature Failed");
            return;
        }
        println!("Signature= ");
        print!("C= 0x");
        printbinary(&cs);
        print!("D= 0x");
        printbinary(&ds);

        if ecdh::ecpvp_dsa(sha, &w0, &m[0..17], &cs, &ds) != 0 {
            println!("***ECDSA Verification Failed");
            return;
        } else {
            println!("ECDSA Signature/Verification succeeded ")
        }
    }
}

fn ecdh_nist256(mut rng: &mut RAND) {
    //use core::nist256;
    use core::nist256::ecdh;
    use core::nist256::ecp;

    let pw = "M0ng00se";
    let pp: &[u8] = b"M0ng00se";
    const EFS: usize = ecdh::EFS;
    const EGS: usize = ecdh::EGS;
    const EAS: usize = ecp::AESKEY;

    let sha = ecp::HASH_TYPE;
    let mut salt: [u8; 8] = [0; 8];
    let mut s1: [u8; EGS] = [0; EGS];
    let mut w0: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut w1: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut z0: [u8; EFS] = [0; EFS];
    let mut z1: [u8; EFS] = [0; EFS];
    let mut key: [u8; EAS] = [0; EAS];
    let mut cs: [u8; EGS] = [0; EGS];
    let mut ds: [u8; EGS] = [0; EGS];
    let mut m: Vec<u8> = vec![0; 32]; // array that could be of any length. So use heap.
    let mut p1: [u8; 3] = [0; 3];
    let mut p2: [u8; 4] = [0; 4];
    let mut v: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut t: [u8; 12] = [0; 12];

    for i in 0..8 {
        salt[i] = (i + 1) as u8
    } // set Salt

    println!("\nTesting ECDH/ECDSA/ECIES");
    println!("Curve nist256");
    println!("Alice's Passphrase= {}", pw);

    let mut s0: [u8; EFS] = [0; EGS];
    hmac::pbkdf2(hmac::MC_SHA2, sha, pp, &salt, 1000, EGS, &mut s0);

    print!("Alice's private key= 0x");
    printbinary(&s0);

    /* Generate Key pair S/W */
    ecdh::key_pair_generate(None, &mut s0, &mut w0);

    print!("Alice's public key= 0x");
    printbinary(&w0);

    let mut res = ecdh::public_key_validate(&w0);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }

    /* Random private key for other party */
    ecdh::key_pair_generate(Some(&mut rng), &mut s1, &mut w1);

    print!("Servers private key= 0x");
    printbinary(&s1);

    print!("Servers public key= 0x");
    printbinary(&w1);

    res = ecdh::public_key_validate(&w1);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }
    /* Calculate common key using DH - IEEE 1363 method */

    ecdh::ecpsvdp_dh(&s0, &w1, &mut z0);
    ecdh::ecpsvdp_dh(&s1, &w0, &mut z1);

    let mut same = true;
    for i in 0..EFS {
        if z0[i] != z1[i] {
            same = false
        }
    }

    if !same {
        println!("*** ECPSVDP-DH Failed");
        return;
    }

    hmac::kdf2(hmac::MC_SHA2, sha, &z0, None, EAS, &mut key);

    print!("Alice's DH Key=  0x");
    printbinary(&key);
    print!("Servers DH Key=  0x");
    printbinary(&key);

    if ecp::CURVETYPE != ecp::MONTGOMERY {
        for i in 0..17 {
            m[i] = i as u8
        }

        println!("Testing ECIES");

        p1[0] = 0x0;
        p1[1] = 0x1;
        p1[2] = 0x2;
        p2[0] = 0x0;
        p2[1] = 0x1;
        p2[2] = 0x2;
        p2[3] = 0x3;

        let cc = ecdh::ecies_encrypt(sha, &p1, &p2, &mut rng, &w1, &m[0..17], &mut v, &mut t);

        if let Some(mut c) = cc {
            println!("Ciphertext= ");
            print!("V= 0x");
            printbinary(&v);
            print!("C= 0x");
            printbinary(&c);
            print!("T= 0x");
            printbinary(&t);

            let mm = ecdh::ecies_decrypt(sha, &p1, &p2, &v, &mut c, &t, &s1);
            if let Some(rm) = mm {
                println!("Decryption succeeded");
                print!("Message is 0x");
                printbinary(&rm);
            } else {
                println!("*** ECIES Decryption Failed");
                return;
            }
        } else {
            println!("*** ECIES Encryption Failed");
            return;
        }

        println!("Testing ECDSA");

        if ecdh::ecpsp_dsa(sha, &mut rng, &s0, &m[0..17], &mut cs, &mut ds) != 0 {
            println!("***ECDSA Signature Failed");
            return;
        }
        println!("Signature= ");
        print!("C= 0x");
        printbinary(&cs);
        print!("D= 0x");
        printbinary(&ds);

        if ecdh::ecpvp_dsa(sha, &w0, &m[0..17], &cs, &ds) != 0 {
            println!("***ECDSA Verification Failed");
            return;
        } else {
            println!("ECDSA Signature/Verification succeeded ")
        }
    }
}

fn ecdh_goldilocks(mut rng: &mut RAND) {
    //use core::goldilocks;
    use core::goldilocks::ecdh;
    use core::goldilocks::ecp;

    
    let pw = "M0ng00se";
    let pp: &[u8] = b"M0ng00se";
    const EFS: usize = ecdh::EFS;
    const EGS: usize = ecdh::EGS;
    const EAS: usize = ecp::AESKEY;

    let sha = ecp::HASH_TYPE;
    let mut salt: [u8; 8] = [0; 8];
    let mut s1: [u8; EGS] = [0; EGS];
    let mut w0: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut w1: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut z0: [u8; EFS] = [0; EFS];
    let mut z1: [u8; EFS] = [0; EFS];
    let mut key: [u8; EAS] = [0; EAS];
    let mut cs: [u8; EGS] = [0; EGS];
    let mut ds: [u8; EGS] = [0; EGS];
    let mut m: Vec<u8> = vec![0; 32]; // array that could be of any length. So use heap.
    let mut p1: [u8; 3] = [0; 3];
    let mut p2: [u8; 4] = [0; 4];
    let mut v: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut t: [u8; 12] = [0; 12];

    for i in 0..8 {
        salt[i] = (i + 1) as u8
    } // set Salt

    println!("\nTesting ECDH/ECDSA/ECIES");
    println!("Curve goldilocks");
    println!("Alice's Passphrase= {}", pw);

    let mut s0: [u8; EFS] = [0; EGS];
    hmac::pbkdf2(hmac::MC_SHA2, sha, pp, &salt, 1000, EGS, &mut s0);

    print!("Alice's private key= 0x");
    printbinary(&s0);

    /* Generate Key pair S/W */
    ecdh::key_pair_generate(None, &mut s0, &mut w0);

    print!("Alice's public key= 0x");
    printbinary(&w0);

    let mut res = ecdh::public_key_validate(&w0);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }

    /* Random private key for other party */
    ecdh::key_pair_generate(Some(&mut rng), &mut s1, &mut w1);

    print!("Servers private key= 0x");
    printbinary(&s1);

    print!("Servers public key= 0x");
    printbinary(&w1);

    res = ecdh::public_key_validate(&w1);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }
    /* Calculate common key using DH - IEEE 1363 method */

    ecdh::ecpsvdp_dh(&s0, &w1, &mut z0);
    ecdh::ecpsvdp_dh(&s1, &w0, &mut z1);

    let mut same = true;
    for i in 0..EFS {
        if z0[i] != z1[i] {
            same = false
        }
    }

    if !same {
        println!("*** ECPSVDP-DH Failed");
        return;
    }

    hmac::kdf2(hmac::MC_SHA2, sha, &z0, None, EAS, &mut key);

    print!("Alice's DH Key=  0x");
    printbinary(&key);
    print!("Servers DH Key=  0x");
    printbinary(&key);

    if ecp::CURVETYPE != ecp::MONTGOMERY {
        for i in 0..17 {
            m[i] = i as u8
        }

        println!("Testing ECIES");

        p1[0] = 0x0;
        p1[1] = 0x1;
        p1[2] = 0x2;
        p2[0] = 0x0;
        p2[1] = 0x1;
        p2[2] = 0x2;
        p2[3] = 0x3;

        let cc = ecdh::ecies_encrypt(sha, &p1, &p2, &mut rng, &w1, &m[0..17], &mut v, &mut t);

        if let Some(mut c) = cc {
            println!("Ciphertext= ");
            print!("V= 0x");
            printbinary(&v);
            print!("C= 0x");
            printbinary(&c);
            print!("T= 0x");
            printbinary(&t);

            let mm = ecdh::ecies_decrypt(sha, &p1, &p2, &v, &mut c, &t, &s1);
            if let Some(rm) = mm {
                println!("Decryption succeeded");
                print!("Message is 0x");
                printbinary(&rm);
            } else {
                println!("*** ECIES Decryption Failed");
                return;
            }
        } else {
            println!("*** ECIES Encryption Failed");
            return;
        }

        println!("Testing ECDSA");

        if ecdh::ecpsp_dsa(sha, &mut rng, &s0, &m[0..17], &mut cs, &mut ds) != 0 {
            println!("***ECDSA Signature Failed");
            return;
        }
        println!("Signature= ");
        print!("C= 0x");
        printbinary(&cs);
        print!("D= 0x");
        printbinary(&ds);

        if ecdh::ecpvp_dsa(sha, &w0, &m[0..17], &cs, &ds) != 0 {
            println!("***ECDSA Verification Failed");
            return;
        } else {
            println!("ECDSA Signature/Verification succeeded ")
        }
    }
}

fn rsa_2048(mut rng: &mut RAND) {
    //use core::rsa2048;
    use core::rsa2048::ff;
    use core::rsa2048::rsa;

    let sha = rsa::HASH_TYPE;
    let message: &[u8] = b"Hello World\n";
    const RFS: usize = rsa::RFS;

    let mut pbc = rsa::new_public_key(ff::FFLEN);
    let mut prv = rsa::new_private_key(ff::HFLEN);

    let mut ml: [u8; RFS] = [0; RFS];
    let mut ms: [u8; RFS] = [0; RFS];
    let mut c: [u8; RFS] = [0; RFS];
    let mut s: [u8; RFS] = [0; RFS];
    let mut e: [u8; RFS] = [0; RFS];

    println!("\nTesting RSA");
    println!("Generating public/private key pair");
    rsa::key_pair(&mut rng, 65537, &mut prv, &mut pbc);

    println!("Encrypting test string\n");
    rsa::oaep_encode(sha, &message, &mut rng, None, &mut e); /* OAEP encode message M to E  */

    rsa::encrypt(&pbc, &e, &mut c); /* encrypt encoded message */
    print!("Ciphertext= 0x");
    printbinary(&c);

    println!("Decrypting test string");
    rsa::decrypt(&prv, &c, &mut ml);
    let mlen = rsa::oaep_decode(sha, None, &mut ml); /* OAEP decode message  */

    let mess = str::from_utf8(&ml[0..mlen]).unwrap();
    print!("{}", &mess);

    println!("Signing message");
    rsa::pkcs15(sha, message, &mut c);

    rsa::decrypt(&prv, &c, &mut s); /* create signature in S */

    print!("Signature= 0x");
    printbinary(&s);

    rsa::encrypt(&pbc, &s, &mut ms);

    let mut cmp = true;
    if c.len() != ms.len() {
        cmp = false;
    } else {
        for j in 0..c.len() {
            if c[j] != ms[j] {
                cmp = false
            }
        }
    }
    if cmp {
        println!("Signature is valid");
    } else {
        println!("Signature is INVALID");
    }

    rsa::private_key_kill(&mut prv);
}

fn main() {
    let mut raw: [u8; 100] = [0; 100];

    let mut rng = RAND::new();
    rng.clean();
    for i in 0..100 {
        raw[i] = i as u8
    }

    rng.seed(100, &raw);

    ecdh_ed25519(&mut rng);
    ecdh_nist256(&mut rng);
    ecdh_goldilocks(&mut rng);
    rsa_2048(&mut rng);
}
