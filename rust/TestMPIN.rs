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

extern crate mcore;

use std::io;

use mcore::rand::RAND;
use mcore::share::SHARE;

pub fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}

#[allow(unused_variables)]
fn mpin_bn254(rng: &mut RAND) {

    use mcore::bn254::mpin;

    const EFS: usize = mpin::EFS;
    const EGS: usize = mpin::EGS;
    const G1S: usize = 2 * EFS + 1; // Group 1 Size
    const G2S: usize = 4 * EFS + 1; // Group 2 Size

    let mut s: [u8; EGS] = [0; EGS];
    let mut sst: [u8; G2S] = [0; G2S];
    let mut token: [u8; G1S] = [0; G1S];
    let mut xid: [u8; G1S] = [0; G1S];
    let mut x: [u8; EGS] = [0; EGS];
    let mut y: [u8; EGS] = [0; EGS];
    let mut sec: [u8; G1S] = [0; G1S];
    let mut hcid: [u8; G1S] = [0; G1S];
    let mut hsid: [u8; G1S] = [0; G1S];

    let domain = "BN254G1_XMD:SHA-256_SVDW_NU_MPIN";
    let dst = domain.as_bytes();

    println!("\nTesting MPIN 2-factor authentication protocol on curve bn254");

// Trusted Authority (TA) set-up
    mpin::random_generate(rng, &mut s);
    print!("Master Secret s: 0x");
    printbinary(&s);

// Create Client Identity
        let name = "testUser@miracl.com";
        let client_id = name.as_bytes();

        mpin::encode_to_curve(&dst,&client_id,&mut hcid);
        print!("Client ID Hashed to Curve= ");
        printbinary(&hcid);

// Client approaches Trusted Authority and is issued secret

    mpin::get_client_secret(&mut s, &hcid, &mut token);
    print!("Client Secret CS: 0x");
    printbinary(&token);

// TA sends Client secret to Client

// Server is issued secret by TA
    mpin::get_server_secret(&s, &mut sst);
//   print!("Server Secret SS: 0x");
//   printbinary(&sst);

// Client extracts PIN from secret to create Token
        let pin: i32 = 1234;
        println!("Client extracts PIN= {}", pin);
        let mut rtn = mpin::extract_pin(&hcid, pin, &mut token);
        if rtn != 0 {
            println!("FAILURE: EXTRACT_PIN rtn: {}", rtn);
            return;
        }
        print!("Client Token TK: 0x");
        printbinary(&token);

// Exercise Secret Sharing
        let mut r: [u8; 128] = [0; 128];
        let mut s1b: [u8; G1S] = [0; G1S];
        let mut s2b: [u8; G1S] = [0; G1S];
        let mut s3b: [u8; G1S] = [0; G1S];
        let mut s4b: [u8; G1S] = [0; G1S];

        for i in 0..128 {
            r[i]=rng.getbyte();
        }
    let mut rng2 = RAND::new();

    // create 4 unique shares of TOKEN
        rng2.clean();
        rng2.seed(128, &r);
        let sh1 = SHARE::new(1,3,&mut s1b,&token, &mut rng2);  // indicate 3 shares required for recovery
        rng2.clean();
        rng2.seed(128, &r);
        let sh2 = SHARE::new(2,3,&mut s2b,&token, &mut rng2);
        rng2.clean();
        rng2.seed(128, &r);
        let sh3 = SHARE::new(3,3,&mut s3b,&token, &mut rng2);
        rng2.clean();
        rng2.seed(128, &r);
        let sh4 = SHARE::new(4,3,&mut s4b,&token, &mut rng2);

        let shares: [SHARE;3]=[sh1,sh2,sh4]; // any 3 shares to recover TOKEN

        SHARE::recover(&mut token,&shares);  // recover token

// MPin Protocol

// Client enters ID and PIN

        print!("PIN= ");
        let _ = io::Write::flush(&mut io::stdout());
        let mut input_text = String::new();
        let _ = io::stdin().read_line(&mut input_text);

        let pin = input_text.trim().parse::<usize>().unwrap();

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC
        rtn = mpin::client_1(
            &hcid,
            Some(rng),
            &mut x,
            pin,
            &token,
            &mut sec,
            &mut xid
        );
        if rtn != 0 {
            println!("FAILURE: CLIENT_1 rtn: {}", rtn);
            return;
        }


// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
                mpin::encode_to_curve(&dst,&client_id, &mut hsid);

// Server generates Random number Y and sends it to Client
                mpin::random_generate(rng, &mut y);

// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
        rtn = mpin::client_2(&x, &y, &mut sec);
        if rtn != 0 {
            println!("FAILURE: CLIENT_2 rtn: {}", rtn);
            return;
        }

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST.

                rtn = mpin::server(
                    &hsid,
                    &y,
                    &sst,
                    &xid,
                    &sec
                );

                if rtn != 0 {
                    if rtn == mpin::BAD_PIN {
                        println!("Server says - Bad Pin. I don't know you. Feck off.\n");
                    } else {
                        println!("FAILURE: SERVER rtn: {}",rtn);
                    }
                } else {
                    println!("Server says - PIN is good! You really are {}\n", name);
                }
}

fn mpin_bls12383(rng: &mut RAND) {
    use mcore::bls12383::mpin;

    const EFS: usize = mpin::EFS;
    const EGS: usize = mpin::EGS;

    const G1S: usize = 2 * EFS + 1; // Group 1 Size
    const G2S: usize = 4 * EFS + 1; // Group 2 Size

    let mut s: [u8; EGS] = [0; EGS];
    let mut sst: [u8; G2S] = [0; G2S];
    let mut token: [u8; G1S] = [0; G1S];
    let mut xid: [u8; G1S] = [0; G1S];
    let mut x: [u8; EGS] = [0; EGS];
    let mut y: [u8; EGS] = [0; EGS];
    let mut sec: [u8; G1S] = [0; G1S];
    let mut hcid: [u8; G1S] = [0; G1S];
    let mut hsid: [u8; G1S] = [0; G1S];

    let domain = "BLS12383G1_XMD:SHA-256_SVDW_NU_MPIN";
    let dst = domain.as_bytes();

    println!("\nTesting MPIN 2-factor authentication protocol on curve bls12383");

// Trusted Authority (TA) set-up
    mpin::random_generate(rng, &mut s);
    print!("Master Secret s: 0x");
    printbinary(&s);

// Create Client Identity
        let name = "testUser@miracl.com";
        let client_id = name.as_bytes();

        mpin::encode_to_curve(&dst,&client_id,&mut hcid);
        print!("Client ID Hashed to Curve= ");
        printbinary(&hcid);

// Client approaches Trusted Authority and is issued secret

    mpin::get_client_secret(&mut s, &hcid, &mut token);
    print!("Client Secret CS: 0x");
    printbinary(&token);

// TA sends Client secret to Client

// Server is issued secret by TA
    mpin::get_server_secret(&s, &mut sst);
//   print!("Server Secret SS: 0x");
//   printbinary(&sst);

// Client extracts PIN from secret to create Token
        let pin: i32 = 1234;
        println!("Client extracts PIN= {}", pin);
        let mut rtn = mpin::extract_pin(&hcid, pin, &mut token);
        if rtn != 0 {
            println!("FAILURE: EXTRACT_PIN rtn: {}", rtn);
            return;
        }
        print!("Client Token TK: 0x");
        printbinary(&token);

// MPin Protocol

// Client enters ID and PIN

        print!("PIN= ");
        let _ = io::Write::flush(&mut io::stdout());
        let mut input_text = String::new();
        let _ = io::stdin().read_line(&mut input_text);

        let pin = input_text.trim().parse::<usize>().unwrap();

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC
        rtn = mpin::client_1(
            &hcid,
            Some(rng),
            &mut x,
            pin,
            &token,
            &mut sec,
            &mut xid
        );
        if rtn != 0 {
            println!("FAILURE: CLIENT_1 rtn: {}", rtn);
            return;
        }


// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
                mpin::encode_to_curve(&dst,&client_id, &mut hsid);

// Server generates Random number Y and sends it to Client
                mpin::random_generate(rng, &mut y);

// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
        rtn = mpin::client_2(&x, &y, &mut sec);
        if rtn != 0 {
            println!("FAILURE: CLIENT_2 rtn: {}", rtn);
            return;
        }

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST.

                rtn = mpin::server(
                    &hsid,
                    &y,
                    &sst,
                    &xid,
                    &sec
                );

                if rtn != 0 {
                    if rtn == mpin::BAD_PIN {
                        println!("Server says - Bad Pin. I don't know you. Feck off.\n");
                    } else {
                        println!("FAILURE: SERVER rtn: {}",rtn);
                    }
                } else {
                    println!("Server says - PIN is good! You really are {}\n", name);
                }
}

fn mpin_bls24479(rng: &mut RAND) {
    use mcore::bls24479::mpin192;

    const EFS: usize = mpin192::EFS;
    const EGS: usize = mpin192::EGS;
    const G1S: usize = 2 * EFS + 1; // Group 1 Size
    const G2S: usize = 8 * EFS + 1; // Group 2 Size

    let mut s: [u8; EGS] = [0; EGS];
    let mut sst: [u8; G2S] = [0; G2S];
    let mut token: [u8; G1S] = [0; G1S];
    let mut xid: [u8; G1S] = [0; G1S];
    let mut x: [u8; EGS] = [0; EGS];
    let mut y: [u8; EGS] = [0; EGS];
    let mut sec: [u8; G1S] = [0; G1S];
    let mut hcid: [u8; G1S] = [0; G1S];
    let mut hsid: [u8; G1S] = [0; G1S];

    let domain = "BLS24479G1_XMD:SHA-256_SVDW_NU_MPIN";
    let dst = domain.as_bytes();

    println!("\nTesting MPIN 2-factor authentication protocol on curve bls24479");

// Trusted Authority (TA) set-up
    mpin192::random_generate(rng, &mut s);
    print!("Master Secret s: 0x");
    printbinary(&s);

// Create Client Identity
        let name = "testUser@miracl.com";
        let client_id = name.as_bytes();

        mpin192::encode_to_curve(&dst,&client_id,&mut hcid);
        print!("Client ID Hashed to Curve= ");
        printbinary(&hcid);

// Client approaches Trusted Authority and is issued secret

    mpin192::get_client_secret(&mut s, &hcid, &mut token);
    print!("Client Secret CS: 0x");
    printbinary(&token);

// TA sends Client secret to Client

// Server is issued secret by TA
    mpin192::get_server_secret(&s, &mut sst);
//   print!("Server Secret SS: 0x");
//   printbinary(&sst);

// Client extracts PIN from secret to create Token
        let pin: i32 = 1234;
        println!("Client extracts PIN= {}", pin);
        let mut rtn = mpin192::extract_pin(&hcid, pin, &mut token);
        if rtn != 0 {
            println!("FAILURE: EXTRACT_PIN rtn: {}", rtn);
            return;
        }
        print!("Client Token TK: 0x");
        printbinary(&token);

// MPin Protocol

// Client enters ID and PIN

        print!("PIN= ");
        let _ = io::Write::flush(&mut io::stdout());
        let mut input_text = String::new();
        let _ = io::stdin().read_line(&mut input_text);

        let pin = input_text.trim().parse::<usize>().unwrap();

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC
        rtn = mpin192::client_1(
            &hcid,
            Some(rng),
            &mut x,
            pin,
            &token,
            &mut sec,
            &mut xid
        );
        if rtn != 0 {
            println!("FAILURE: CLIENT_1 rtn: {}", rtn);
            return;
        }


// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
                mpin192::encode_to_curve(&dst,&client_id, &mut hsid);

// Server generates Random number Y and sends it to Client
                mpin192::random_generate(rng, &mut y);

// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
        rtn = mpin192::client_2(&x, &y, &mut sec);
        if rtn != 0 {
            println!("FAILURE: CLIENT_2 rtn: {}", rtn);
            return;
        }

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST.

                rtn = mpin192::server(
                    &hsid,
                    &y,
                    &sst,
                    &xid,
                    &sec
                );

                if rtn != 0 {
                    if rtn == mpin192::BAD_PIN {
                        println!("Server says - Bad Pin. I don't know you. Feck off.\n");
                    } else {
                        println!("FAILURE: SERVER rtn: {}",rtn);
                    }
                } else {
                    println!("Server says - PIN is good! You really are {}\n", name);
                }
}

fn mpin_bls48556(rng: &mut RAND) {
    use mcore::bls48556::mpin256;

    const EFS: usize = mpin256::EFS;
    const EGS: usize = mpin256::EGS;
    const G1S: usize = 2 * EFS + 1; // Group 1 Size
    const G2S: usize = 16 * EFS + 1; // Group 2 Size

    let mut s: [u8; EGS] = [0; EGS];
    let mut sst: [u8; G2S] = [0; G2S];
    let mut token: [u8; G1S] = [0; G1S];
    let mut xid: [u8; G1S] = [0; G1S];
    let mut x: [u8; EGS] = [0; EGS];
    let mut y: [u8; EGS] = [0; EGS];
    let mut sec: [u8; G1S] = [0; G1S];
    let mut hcid: [u8; G1S] = [0; G1S];
    let mut hsid: [u8; G1S] = [0; G1S];

    let domain = "BLS48556G1_XMD:SHA-256_SVDW_NU_MPIN";
    let dst = domain.as_bytes();

    println!("\nTesting MPIN 2-factor authentication protocol on curve bls48556");

// Trusted Authority (TA) set-up
    mpin256::random_generate(rng, &mut s);
    print!("Master Secret s: 0x");
    printbinary(&s);

// Create Client Identity
        let name = "testUser@miracl.com";
        let client_id = name.as_bytes();

        mpin256::encode_to_curve(&dst,&client_id,&mut hcid);
        print!("Client ID Hashed to Curve= ");
        printbinary(&hcid);

// Client approaches Trusted Authority and is issued secret

    mpin256::get_client_secret(&mut s, &hcid, &mut token);
    print!("Client Secret CS: 0x");
    printbinary(&token);

// TA sends Client secret to Client

// Server is issued secret by TA
    mpin256::get_server_secret(&s, &mut sst);
//   print!("Server Secret SS: 0x");
//   printbinary(&sst);

// Client extracts PIN from secret to create Token
        let pin: i32 = 1234;
        println!("Client extracts PIN= {}", pin);
        let mut rtn = mpin256::extract_pin(&hcid, pin, &mut token);
        if rtn != 0 {
            println!("FAILURE: EXTRACT_PIN rtn: {}", rtn);
            return;
        }
        print!("Client Token TK: 0x");
        printbinary(&token);

// MPin Protocol

// Client enters ID and PIN

        print!("PIN= ");
        let _ = io::Write::flush(&mut io::stdout());
        let mut input_text = String::new();
        let _ = io::stdin().read_line(&mut input_text);

        let pin = input_text.trim().parse::<usize>().unwrap();

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC
        rtn = mpin256::client_1(
            &hcid,
            Some(rng),
            &mut x,
            pin,
            &token,
            &mut sec,
            &mut xid
        );
        if rtn != 0 {
            println!("FAILURE: CLIENT_1 rtn: {}", rtn);
            return;
        }


// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
                mpin256::encode_to_curve(&dst,&client_id, &mut hsid);

// Server generates Random number Y and sends it to Client
                mpin256::random_generate(rng, &mut y);

// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
        rtn = mpin256::client_2(&x, &y, &mut sec);
        if rtn != 0 {
            println!("FAILURE: CLIENT_2 rtn: {}", rtn);
            return;
        }

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST.

                rtn = mpin256::server(
                    &hsid,
                    &y,
                    &sst,
                    &xid,
                    &sec
                );

                if rtn != 0 {
                    if rtn == mpin256::BAD_PIN {
                        println!("Server says - Bad Pin. I don't know you. Feck off.\n");
                    } else {
                        println!("FAILURE: SERVER rtn: {}",rtn);
                    }
                } else {
                    println!("Server says - PIN is good! You really are {}\n", name);
                }
}

fn main() {
    let mut raw: [u8; 100] = [0; 100];

    let mut rng = RAND::new();
    rng.clean();
    for i in 0..100 {
        raw[i] = i as u8
    }

    rng.seed(100, &raw);

    mpin_bn254(&mut rng);
    mpin_bls12383(&mut rng);
    mpin_bls24479(&mut rng);
    mpin_bls48556(&mut rng);
}
