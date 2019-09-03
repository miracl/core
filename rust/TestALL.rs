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

use std::io;
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

fn mpin_bn254(mut rng: &mut RAND) {
    //use core::bn254;
    use core::bn254::ecp;
    use core::bn254::mpin;
    pub const PERMITS: bool = true;
    pub const PINERROR: bool = true;
    pub const FULL: bool = true;
    //pub const SINGLE_PASS:bool=false;

    const EFS: usize = mpin::EFS;
    const EGS: usize = mpin::EGS;

    let mut s: [u8; EGS] = [0; EGS];
    const RM: usize = EFS as usize;
    let mut hcid: [u8; RM] = [0; RM];
    let mut hsid: [u8; RM] = [0; RM];

    const G1S: usize = 2 * EFS + 1; /* Group 1 Size */
    const G2S: usize = 4 * EFS + 1; /* Group 2 Size */
    const EAS: usize = ecp::AESKEY;

    let mut sst: [u8; G2S] = [0; G2S];
    let mut token: [u8; G1S] = [0; G1S];
    let mut permit: [u8; G1S] = [0; G1S];
    let mut g1: [u8; 12 * EFS] = [0; 12 * EFS];
    let mut g2: [u8; 12 * EFS] = [0; 12 * EFS];
    let mut xid: [u8; G1S] = [0; G1S];
    let mut xcid: [u8; G1S] = [0; G1S];
    let mut x: [u8; EGS] = [0; EGS];
    let mut y: [u8; EGS] = [0; EGS];
    let mut sec: [u8; G1S] = [0; G1S];
    let mut r: [u8; EGS] = [0; EGS];
    let mut z: [u8; G1S] = [0; G1S];
    let mut hid: [u8; G1S] = [0; G1S];
    let mut htid: [u8; G1S] = [0; G1S];
    let mut rhid: [u8; G1S] = [0; G1S];
    let mut w: [u8; EGS] = [0; EGS];
    let mut t: [u8; G1S] = [0; G1S];
    let mut e: [u8; 12 * EFS] = [0; 12 * EFS];
    let mut f: [u8; 12 * EFS] = [0; 12 * EFS];
    let mut h: [u8; RM] = [0; RM];
    let mut ck: [u8; EAS] = [0; EAS];
    let mut sk: [u8; EAS] = [0; EAS];

    let sha = ecp::HASH_TYPE;

    println!("\nTesting MPIN - PIN is 1234");
    /* Trusted Authority set-up */

    mpin::random_generate(&mut rng, &mut s);
    print!("Master Secret s: 0x");
    printbinary(&s);

    /* Create Client Identity */
    let name = "testUser@miracl.com";
    let client_id = name.as_bytes();

    print!("Client ID= ");
    printbinary(&client_id);

    mpin::hash_id(sha, &client_id, &mut hcid); /* Either Client or TA calculates Hash(ID) - you decide! */

    /* Client and Server are issued secrets by DTA */
    mpin::get_server_secret(&s, &mut sst);
    print!("Server Secret SS: 0x");
    printbinary(&sst);

    mpin::get_client_secret(&mut s, &hcid, &mut token);
    print!("Client Secret CS: 0x");
    printbinary(&token);

    /* Client extracts PIN from secret to create Token */
    let pin: i32 = 1234;
    println!("Client extracts PIN= {}", pin);
    let mut rtn = mpin::extract_pin(sha, &client_id, pin, &mut token);
    if rtn != 0 {
        println!("FAILURE: EXTRACT_PIN rtn: {}", rtn);
    }

    print!("Client Token TK: 0x");
    printbinary(&token);

    if FULL {
        mpin::precompute(&token, &hcid, &mut g1, &mut g2);
    }

    let mut date = 0;
    if PERMITS {
        date = mpin::today();
        /* Client gets "Time Token" permit from DTA */

        mpin::get_client_permit(sha, date, &s, &hcid, &mut permit);
        print!("Time Permit TP: 0x");
        printbinary(&permit);

        /* This encoding makes Time permit look random - Elligator squared */
        mpin::encoding(&mut rng, &mut permit);
        print!("Encoded Time Permit TP: 0x");
        printbinary(&permit);
        mpin::decoding(&mut permit);
        print!("Decoded Time Permit TP: 0x");
        printbinary(&permit);
    }

    print!("\nPIN= ");
    let _ = io::Write::flush(&mut io::stdout());
    let mut input_text = String::new();
    let _ = io::stdin().read_line(&mut input_text);

    let pin = input_text.trim().parse::<usize>().unwrap();

    println!("MPIN Multi Pass");
    /* Send U=x.ID to server, and recreate secret from token and pin */
    rtn = mpin::client_1(
        sha,
        date,
        &client_id,
        Some(&mut rng),
        &mut x,
        pin,
        &token,
        &mut sec,
        Some(&mut xid[..]),
        Some(&mut xcid[..]),
        Some(&permit[..]),
    );
    if rtn != 0 {
        println!("FAILURE: CLIENT_1 rtn: {}", rtn);
    }

    if FULL {
        mpin::hash_id(sha, &client_id, &mut hcid);
        mpin::get_g1_multiple(Some(&mut rng), 1, &mut r, &hcid, &mut z); /* Also Send Z=r.ID to Server, remember random r */
    }

    /* Server calculates H(ID) and H(T|H(ID)) (if time mpin::PERMITS enabled), and maps them to points on the curve HID and HTID resp. */

    mpin::server_1(sha, date, &client_id, &mut hid, Some(&mut htid[..]));

    if date != 0 {
        rhid.clone_from_slice(&htid[..]);
    } else {
        rhid.clone_from_slice(&hid[..]);
    }

    /* Server generates Random number Y and sends it to Client */
    mpin::random_generate(&mut rng, &mut y);

    if FULL {
        mpin::hash_id(sha, &client_id, &mut hsid);
        mpin::get_g1_multiple(Some(&mut rng), 0, &mut w, &rhid, &mut t); /* Also send T=w.ID to client, remember random w  */
    }

    /* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
    rtn = mpin::client_2(&x, &y, &mut sec);
    if rtn != 0 {
        println!("FAILURE: CLIENT_2 rtn: {}", rtn);
    }

    /* Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error. */
    /* If PIN error not required, set E and F = null */

    if !PINERROR {
        rtn = mpin::server_2(
            date,
            &hid,
            Some(&htid[..]),
            &y,
            &sst,
            Some(&xid[..]),
            Some(&xcid[..]),
            &sec,
            None,
            None,
        );
    } else {
        rtn = mpin::server_2(
            date,
            &hid,
            Some(&htid[..]),
            &y,
            &sst,
            Some(&xid[..]),
            Some(&xcid[..]),
            &sec,
            Some(&mut e),
            Some(&mut f),
        );
    }

    if rtn == mpin::BAD_PIN {
        println!("Server says - Bad Pin. I don't know you. Feck off.");
        if PINERROR {
            let err = mpin::kangaroo(&e, &f);
            if err != 0 {
                println!("(Client PIN is out by {})", err)
            }
        }
        return;
    } else {
        println!("Server says - PIN is good! You really are {}", name);
    }

    if FULL {
        let mut pxcid = None;
        if PERMITS {
            pxcid = Some(&xcid[..])
        };

        mpin::hash_all(sha, &hcid, &xid, pxcid, &sec, &y, &z, &t, &mut h);
        mpin::client_key(sha, &g1, &g2, pin, &r, &x, &h, &t, &mut ck);
        print!("Client Key =  0x");
        printbinary(&ck);

        mpin::hash_all(sha, &hsid, &xid, pxcid, &sec, &y, &z, &t, &mut h);
        mpin::server_key(sha, &z, &sst, &w, &h, &hid, &xid, pxcid, &mut sk);
        print!("Server Key =  0x");
        printbinary(&sk);
    }
}

fn mpin_bls383(mut rng: &mut RAND) {
    //use core::bls383;
    use core::bls383::ecp;
    use core::bls383::mpin;
    pub const PERMITS: bool = true;
    pub const PINERROR: bool = true;
    pub const FULL: bool = true;
    //pub const SINGLE_PASS:bool=false;

    const EFS: usize = mpin::EFS;
    const EGS: usize = mpin::EGS;

    let mut s: [u8; EGS] = [0; EGS];
    const RM: usize = EFS as usize;
    let mut hcid: [u8; RM] = [0; RM];
    let mut hsid: [u8; RM] = [0; RM];

    const G1S: usize = 2 * EFS + 1; /* Group 1 Size */
    const G2S: usize = 4 * EFS + 1; /* Group 2 Size */
    const EAS: usize = ecp::AESKEY;

    let mut sst: [u8; G2S] = [0; G2S];
    let mut token: [u8; G1S] = [0; G1S];
    let mut permit: [u8; G1S] = [0; G1S];
    let mut g1: [u8; 12 * EFS] = [0; 12 * EFS];
    let mut g2: [u8; 12 * EFS] = [0; 12 * EFS];
    let mut xid: [u8; G1S] = [0; G1S];
    let mut xcid: [u8; G1S] = [0; G1S];
    let mut x: [u8; EGS] = [0; EGS];
    let mut y: [u8; EGS] = [0; EGS];
    let mut sec: [u8; G1S] = [0; G1S];
    let mut r: [u8; EGS] = [0; EGS];
    let mut z: [u8; G1S] = [0; G1S];
    let mut hid: [u8; G1S] = [0; G1S];
    let mut htid: [u8; G1S] = [0; G1S];
    let mut rhid: [u8; G1S] = [0; G1S];
    let mut w: [u8; EGS] = [0; EGS];
    let mut t: [u8; G1S] = [0; G1S];
    let mut e: [u8; 12 * EFS] = [0; 12 * EFS];
    let mut f: [u8; 12 * EFS] = [0; 12 * EFS];
    let mut h: [u8; RM] = [0; RM];
    let mut ck: [u8; EAS] = [0; EAS];
    let mut sk: [u8; EAS] = [0; EAS];

    let sha = ecp::HASH_TYPE;

    println!("\nTesting MPIN - PIN is 1234");
    /* Trusted Authority set-up */

    mpin::random_generate(&mut rng, &mut s);
    print!("Master Secret s: 0x");
    printbinary(&s);

    /* Create Client Identity */
    let name = "testUser@miracl.com";
    let client_id = name.as_bytes();

    print!("Client ID= ");
    printbinary(&client_id);

    mpin::hash_id(sha, &client_id, &mut hcid); /* Either Client or TA calculates Hash(ID) - you decide! */

    /* Client and Server are issued secrets by DTA */
    mpin::get_server_secret(&s, &mut sst);
    print!("Server Secret SS: 0x");
    printbinary(&sst);

    mpin::get_client_secret(&mut s, &hcid, &mut token);
    print!("Client Secret CS: 0x");
    printbinary(&token);

    /* Client extracts PIN from secret to create Token */
    let pin: i32 = 1234;
    println!("Client extracts PIN= {}", pin);
    let mut rtn = mpin::extract_pin(sha, &client_id, pin, &mut token);
    if rtn != 0 {
        println!("FAILURE: EXTRACT_PIN rtn: {}", rtn);
    }

    print!("Client Token TK: 0x");
    printbinary(&token);

    if FULL {
        mpin::precompute(&token, &hcid, &mut g1, &mut g2);
    }

    let mut date = 0;
    if PERMITS {
        date = mpin::today();
        /* Client gets "Time Token" permit from DTA */

        mpin::get_client_permit(sha, date, &s, &hcid, &mut permit);
        print!("Time Permit TP: 0x");
        printbinary(&permit);

        /* This encoding makes Time permit look random - Elligator squared */
        mpin::encoding(&mut rng, &mut permit);
        print!("Encoded Time Permit TP: 0x");
        printbinary(&permit);
        mpin::decoding(&mut permit);
        print!("Decoded Time Permit TP: 0x");
        printbinary(&permit);
    }

    print!("\nPIN= ");
    let _ = io::Write::flush(&mut io::stdout());
    let mut input_text = String::new();
    let _ = io::stdin().read_line(&mut input_text);

    let pin = input_text.trim().parse::<usize>().unwrap();

    println!("MPIN Multi Pass");
    /* Send U=x.ID to server, and recreate secret from token and pin */
    rtn = mpin::client_1(
        sha,
        date,
        &client_id,
        Some(&mut rng),
        &mut x,
        pin,
        &token,
        &mut sec,
        Some(&mut xid[..]),
        Some(&mut xcid[..]),
        Some(&permit[..]),
    );
    if rtn != 0 {
        println!("FAILURE: CLIENT_1 rtn: {}", rtn);
    }

    if FULL {
        mpin::hash_id(sha, &client_id, &mut hcid);
        mpin::get_g1_multiple(Some(&mut rng), 1, &mut r, &hcid, &mut z); /* Also Send Z=r.ID to Server, remember random r */
    }

    /* Server calculates H(ID) and H(T|H(ID)) (if time mpin::PERMITS enabled), and maps them to points on the curve HID and HTID resp. */

    mpin::server_1(sha, date, &client_id, &mut hid, Some(&mut htid[..]));

    if date != 0 {
        rhid.clone_from_slice(&htid[..]);
    } else {
        rhid.clone_from_slice(&hid[..]);
    }

    /* Server generates Random number Y and sends it to Client */
    mpin::random_generate(&mut rng, &mut y);

    if FULL {
        mpin::hash_id(sha, &client_id, &mut hsid);
        mpin::get_g1_multiple(Some(&mut rng), 0, &mut w, &rhid, &mut t); /* Also send T=w.ID to client, remember random w  */
    }

    /* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
    rtn = mpin::client_2(&x, &y, &mut sec);
    if rtn != 0 {
        println!("FAILURE: CLIENT_2 rtn: {}", rtn);
    }

    /* Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error. */
    /* If PIN error not required, set E and F = null */

    if !PINERROR {
        rtn = mpin::server_2(
            date,
            &hid,
            Some(&htid[..]),
            &y,
            &sst,
            Some(&xid[..]),
            Some(&xcid[..]),
            &sec,
            None,
            None,
        );
    } else {
        rtn = mpin::server_2(
            date,
            &hid,
            Some(&htid[..]),
            &y,
            &sst,
            Some(&xid[..]),
            Some(&xcid[..]),
            &sec,
            Some(&mut e),
            Some(&mut f),
        );
    }

    if rtn == mpin::BAD_PIN {
        println!("Server says - Bad Pin. I don't know you. Feck off.");
        if PINERROR {
            let err = mpin::kangaroo(&e, &f);
            if err != 0 {
                println!("(Client PIN is out by {})", err)
            }
        }
        return;
    } else {
        println!("Server says - PIN is good! You really are {}", name);
    }

    if FULL {
        let mut pxcid = None;
        if PERMITS {
            pxcid = Some(&xcid[..])
        };

        mpin::hash_all(sha, &hcid, &xid, pxcid, &sec, &y, &z, &t, &mut h);
        mpin::client_key(sha, &g1, &g2, pin, &r, &x, &h, &t, &mut ck);
        print!("Client Key =  0x");
        printbinary(&ck);

        mpin::hash_all(sha, &hsid, &xid, pxcid, &sec, &y, &z, &t, &mut h);
        mpin::server_key(sha, &z, &sst, &w, &h, &hid, &xid, pxcid, &mut sk);
        print!("Server Key =  0x");
        printbinary(&sk);
    }
}

fn mpin_bls24(mut rng: &mut RAND) {
    //use core::bls24;
    use core::bls24::ecp;
    use core::bls24::mpin192;

    pub const PERMITS: bool = true;
    pub const PINERROR: bool = true;
    pub const FULL: bool = true;
    //pub const SINGLE_PASS:bool=false;

    const EFS: usize = mpin192::EFS;
    const EGS: usize = mpin192::EGS;

    let mut s: [u8; EGS] = [0; EGS];
    const RM: usize = EFS as usize;
    let mut hcid: [u8; RM] = [0; RM];
    let mut hsid: [u8; RM] = [0; RM];

    const G1S: usize = 2 * EFS + 1; /* Group 1 Size */
    const G2S: usize = 8 * EFS + 1; /* Group 2 Size */
    const EAS: usize = ecp::AESKEY;

    let mut sst: [u8; G2S] = [0; G2S];
    let mut token: [u8; G1S] = [0; G1S];
    let mut permit: [u8; G1S] = [0; G1S];
    let mut g1: [u8; 24 * EFS] = [0; 24 * EFS];
    let mut g2: [u8; 24 * EFS] = [0; 24 * EFS];
    let mut xid: [u8; G1S] = [0; G1S];
    let mut xcid: [u8; G1S] = [0; G1S];
    let mut x: [u8; EGS] = [0; EGS];
    let mut y: [u8; EGS] = [0; EGS];
    let mut sec: [u8; G1S] = [0; G1S];
    let mut r: [u8; EGS] = [0; EGS];
    let mut z: [u8; G1S] = [0; G1S];
    let mut hid: [u8; G1S] = [0; G1S];
    let mut htid: [u8; G1S] = [0; G1S];
    let mut rhid: [u8; G1S] = [0; G1S];
    let mut w: [u8; EGS] = [0; EGS];
    let mut t: [u8; G1S] = [0; G1S];
    let mut e: [u8; 24 * EFS] = [0; 24 * EFS];
    let mut f: [u8; 24 * EFS] = [0; 24 * EFS];
    let mut h: [u8; RM] = [0; RM];
    let mut ck: [u8; EAS] = [0; EAS];
    let mut sk: [u8; EAS] = [0; EAS];

    let sha = ecp::HASH_TYPE;

    println!("\nTesting MPIN - PIN is 1234");
    /* Trusted Authority set-up */

    mpin192::random_generate(&mut rng, &mut s);
    print!("Master Secret s: 0x");
    printbinary(&s);

    /* Create Client Identity */
    let name = "testUser@miracl.com";
    let client_id = name.as_bytes();

    print!("Client ID= ");
    printbinary(&client_id);

    mpin192::hash_id(sha, &client_id, &mut hcid); /* Either Client or TA calculates Hash(ID) - you decide! */

    /* Client and Server are issued secrets by DTA */
    mpin192::get_server_secret(&s, &mut sst);
    print!("Server Secret SS: 0x");
    printbinary(&sst);

    mpin192::get_client_secret(&mut s, &hcid, &mut token);
    print!("Client Secret CS: 0x");
    printbinary(&token);

    /* Client extracts PIN from secret to create Token */
    let pin: i32 = 1234;
    println!("Client extracts PIN= {}", pin);
    let mut rtn = mpin192::extract_pin(sha, &client_id, pin, &mut token);
    if rtn != 0 {
        println!("FAILURE: EXTRACT_PIN rtn: {}", rtn);
    }

    print!("Client Token TK: 0x");
    printbinary(&token);

    if FULL {
        mpin192::precompute(&token, &hcid, &mut g1, &mut g2);
    }

    let mut date = 0;
    if PERMITS {
        date = mpin192::today();
        /* Client gets "Time Token" permit from DTA */

        mpin192::get_client_permit(sha, date, &s, &hcid, &mut permit);
        print!("Time Permit TP: 0x");
        printbinary(&permit);

        /* This encoding makes Time permit look random - Elligator squared */
        mpin192::encoding(&mut rng, &mut permit);
        print!("Encoded Time Permit TP: 0x");
        printbinary(&permit);
        mpin192::decoding(&mut permit);
        print!("Decoded Time Permit TP: 0x");
        printbinary(&permit);
    }

    print!("\nPIN= ");
    let _ = io::Write::flush(&mut io::stdout());
    let mut input_text = String::new();
    let _ = io::stdin().read_line(&mut input_text);

    let pin = input_text.trim().parse::<usize>().unwrap();

    println!("MPIN Multi Pass");
    /* Send U=x.ID to server, and recreate secret from token and pin */
    rtn = mpin192::client_1(
        sha,
        date,
        &client_id,
        Some(&mut rng),
        &mut x,
        pin,
        &token,
        &mut sec,
        Some(&mut xid[..]),
        Some(&mut xcid[..]),
        Some(&permit[..]),
    );
    if rtn != 0 {
        println!("FAILURE: CLIENT_1 rtn: {}", rtn);
    }

    if FULL {
        mpin192::hash_id(sha, &client_id, &mut hcid);
        mpin192::get_g1_multiple(Some(&mut rng), 1, &mut r, &hcid, &mut z); /* Also Send Z=r.ID to Server, remember random r */
    }

    /* Server calculates H(ID) and H(T|H(ID)) (if time mpin192::PERMITS enabled), and maps them to points on the curve HID and HTID resp. */

    mpin192::server_1(sha, date, &client_id, &mut hid, Some(&mut htid[..]));

    if date != 0 {
        rhid.clone_from_slice(&htid[..]);
    } else {
        rhid.clone_from_slice(&hid[..]);
    }

    /* Server generates Random number Y and sends it to Client */
    mpin192::random_generate(&mut rng, &mut y);

    if FULL {
        mpin192::hash_id(sha, &client_id, &mut hsid);
        mpin192::get_g1_multiple(Some(&mut rng), 0, &mut w, &rhid, &mut t); /* Also send T=w.ID to client, remember random w  */
    }

    /* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
    rtn = mpin192::client_2(&x, &y, &mut sec);
    if rtn != 0 {
        println!("FAILURE: CLIENT_2 rtn: {}", rtn);
    }

    /* Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error. */
    /* If PIN error not required, set E and F = null */

    if !PINERROR {
        rtn = mpin192::server_2(
            date,
            &hid,
            Some(&htid[..]),
            &y,
            &sst,
            Some(&xid[..]),
            Some(&xcid[..]),
            &sec,
            None,
            None,
        );
    } else {
        rtn = mpin192::server_2(
            date,
            &hid,
            Some(&htid[..]),
            &y,
            &sst,
            Some(&xid[..]),
            Some(&xcid[..]),
            &sec,
            Some(&mut e),
            Some(&mut f),
        );
    }

    if rtn == mpin192::BAD_PIN {
        println!("Server says - Bad Pin. I don't know you. Feck off.");
        if PINERROR {
            let err = mpin192::kangaroo(&e, &f);
            if err != 0 {
                println!("(Client PIN is out by {})", err)
            }
        }
        return;
    } else {
        println!("Server says - PIN is good! You really are {}", name);
    }

    if FULL {
        let mut pxcid = None;
        if PERMITS {
            pxcid = Some(&xcid[..])
        };

        mpin192::hash_all(sha, &hcid, &xid, pxcid, &sec, &y, &z, &t, &mut h);
        mpin192::client_key(sha, &g1, &g2, pin, &r, &x, &h, &t, &mut ck);
        print!("Client Key =  0x");
        printbinary(&ck);

        mpin192::hash_all(sha, &hsid, &xid, pxcid, &sec, &y, &z, &t, &mut h);
        mpin192::server_key(sha, &z, &sst, &w, &h, &hid, &xid, pxcid, &mut sk);
        print!("Server Key =  0x");
        printbinary(&sk);
    }
}

fn mpin_bls48(mut rng: &mut RAND) {
    //use core::bls48;
    use core::bls48::ecp;
    use core::bls48::mpin256;

    pub const PERMITS: bool = true;
    pub const PINERROR: bool = true;
    pub const FULL: bool = true;
    //pub const SINGLE_PASS:bool=false;

    const EFS: usize = mpin256::EFS;
    const EGS: usize = mpin256::EGS;

    let mut s: [u8; EGS] = [0; EGS];
    const RM: usize = EFS as usize;
    let mut hcid: [u8; RM] = [0; RM];
    let mut hsid: [u8; RM] = [0; RM];

    const G1S: usize = 2 * EFS + 1; /* Group 1 Size */
    const G2S: usize = 16 * EFS + 1; /* Group 2 Size */
    const EAS: usize = ecp::AESKEY;

    let mut sst: [u8; G2S] = [0; G2S];
    let mut token: [u8; G1S] = [0; G1S];
    let mut permit: [u8; G1S] = [0; G1S];
    let mut g1: [u8; 48 * EFS] = [0; 48 * EFS];
    let mut g2: [u8; 48 * EFS] = [0; 48 * EFS];
    let mut xid: [u8; G1S] = [0; G1S];
    let mut xcid: [u8; G1S] = [0; G1S];
    let mut x: [u8; EGS] = [0; EGS];
    let mut y: [u8; EGS] = [0; EGS];
    let mut sec: [u8; G1S] = [0; G1S];
    let mut r: [u8; EGS] = [0; EGS];
    let mut z: [u8; G1S] = [0; G1S];
    let mut hid: [u8; G1S] = [0; G1S];
    let mut htid: [u8; G1S] = [0; G1S];
    let mut rhid: [u8; G1S] = [0; G1S];
    let mut w: [u8; EGS] = [0; EGS];
    let mut t: [u8; G1S] = [0; G1S];
    let mut e: [u8; 48 * EFS] = [0; 48 * EFS];
    let mut f: [u8; 48 * EFS] = [0; 48 * EFS];
    let mut h: [u8; RM] = [0; RM];
    let mut ck: [u8; EAS] = [0; EAS];
    let mut sk: [u8; EAS] = [0; EAS];

    let sha = ecp::HASH_TYPE;

    println!("\nTesting MPIN - PIN is 1234");
    /* Trusted Authority set-up */

    mpin256::random_generate(&mut rng, &mut s);
    print!("Master Secret s: 0x");
    printbinary(&s);

    /* Create Client Identity */
    let name = "testUser@miracl.com";
    let client_id = name.as_bytes();

    print!("Client ID= ");
    printbinary(&client_id);

    mpin256::hash_id(sha, &client_id, &mut hcid); /* Either Client or TA calculates Hash(ID) - you decide! */

    /* Client and Server are issued secrets by DTA */
    mpin256::get_server_secret(&s, &mut sst);
    print!("Server Secret SS: 0x");
    printbinary(&sst);

    mpin256::get_client_secret(&mut s, &hcid, &mut token);
    print!("Client Secret CS: 0x");
    printbinary(&token);

    /* Client extracts PIN from secret to create Token */
    let pin: i32 = 1234;
    println!("Client extracts PIN= {}", pin);
    let mut rtn = mpin256::extract_pin(sha, &client_id, pin, &mut token);
    if rtn != 0 {
        println!("FAILURE: EXTRACT_PIN rtn: {}", rtn);
    }

    print!("Client Token TK: 0x");
    printbinary(&token);

    if FULL {
        mpin256::precompute(&token, &hcid, &mut g1, &mut g2);
    }

    let mut date = 0;
    if PERMITS {
        date = mpin256::today();
        /* Client gets "Time Token" permit from DTA */

        mpin256::get_client_permit(sha, date, &s, &hcid, &mut permit);
        print!("Time Permit TP: 0x");
        printbinary(&permit);

        /* This encoding makes Time permit look random - Elligator squared */
        mpin256::encoding(&mut rng, &mut permit);
        print!("Encoded Time Permit TP: 0x");
        printbinary(&permit);
        mpin256::decoding(&mut permit);
        print!("Decoded Time Permit TP: 0x");
        printbinary(&permit);
    }

    print!("\nPIN= ");
    let _ = io::Write::flush(&mut io::stdout());
    let mut input_text = String::new();
    let _ = io::stdin().read_line(&mut input_text);

    let pin = input_text.trim().parse::<usize>().unwrap();

    println!("MPIN Multi Pass");
    /* Send U=x.ID to server, and recreate secret from token and pin */
    rtn = mpin256::client_1(
        sha,
        date,
        &client_id,
        Some(&mut rng),
        &mut x,
        pin,
        &token,
        &mut sec,
        Some(&mut xid[..]),
        Some(&mut xcid[..]),
        Some(&permit[..]),
    );
    if rtn != 0 {
        println!("FAILURE: CLIENT_1 rtn: {}", rtn);
    }

    if FULL {
        mpin256::hash_id(sha, &client_id, &mut hcid);
        mpin256::get_g1_multiple(Some(&mut rng), 1, &mut r, &hcid, &mut z); /* Also Send Z=r.ID to Server, remember random r */
    }

    /* Server calculates H(ID) and H(T|H(ID)) (if time mpin256::PERMITS enabled), and maps them to points on the curve HID and HTID resp. */

    mpin256::server_1(sha, date, &client_id, &mut hid, Some(&mut htid[..]));

    if date != 0 {
        rhid.clone_from_slice(&htid[..]);
    } else {
        rhid.clone_from_slice(&hid[..]);
    }

    /* Server generates Random number Y and sends it to Client */
    mpin256::random_generate(&mut rng, &mut y);

    if FULL {
        mpin256::hash_id(sha, &client_id, &mut hsid);
        mpin256::get_g1_multiple(Some(&mut rng), 0, &mut w, &rhid, &mut t); /* Also send T=w.ID to client, remember random w  */
    }

    /* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
    rtn = mpin256::client_2(&x, &y, &mut sec);
    if rtn != 0 {
        println!("FAILURE: CLIENT_2 rtn: {}", rtn);
    }

    /* Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error. */
    /* If PIN error not required, set E and F = null */

    if !PINERROR {
        rtn = mpin256::server_2(
            date,
            &hid,
            Some(&htid[..]),
            &y,
            &sst,
            Some(&xid[..]),
            Some(&xcid[..]),
            &sec,
            None,
            None,
        );
    } else {
        rtn = mpin256::server_2(
            date,
            &hid,
            Some(&htid[..]),
            &y,
            &sst,
            Some(&xid[..]),
            Some(&xcid[..]),
            &sec,
            Some(&mut e),
            Some(&mut f),
        );
    }

    if rtn == mpin256::BAD_PIN {
        println!("Server says - Bad Pin. I don't know you. Feck off.");
        if PINERROR {
            let err = mpin256::kangaroo(&e, &f);
            if err != 0 {
                println!("(Client PIN is out by {})", err)
            }
        }
        return;
    } else {
        println!("Server says - PIN is good! You really are {}", name);
    }

    if FULL {
        let mut pxcid = None;
        if PERMITS {
            pxcid = Some(&xcid[..])
        };

        mpin256::hash_all(sha, &hcid, &xid, pxcid, &sec, &y, &z, &t, &mut h);
        mpin256::client_key(sha, &g1, &g2, pin, &r, &x, &h, &t, &mut ck);
        print!("Client Key =  0x");
        printbinary(&ck);

        mpin256::hash_all(sha, &hsid, &xid, pxcid, &sec, &y, &z, &t, &mut h);
        mpin256::server_key(sha, &z, &sst, &w, &h, &hid, &xid, pxcid, &mut sk);
        print!("Server Key =  0x");
        printbinary(&sk);
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
    mpin_bn254(&mut rng);
    mpin_bls383(&mut rng);
    mpin_bls24(&mut rng);
    mpin_bls48(&mut rng);
    rsa_2048(&mut rng);
}
