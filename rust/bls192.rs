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
use crate::xxx::ecp;
use crate::xxx::ecp::ECP;
use crate::xxx::ecp4::ECP4;
use crate::xxx::fp8::FP8;
use std::str;
//use xxx::fp24::FP24;
use crate::xxx::big;
use crate::xxx::big::BIG;
use crate::xxx::pair192;
use crate::xxx::rom;

use crate::rand::RAND;
use crate::sha3::SHA3;
use crate::sha3::SHAKE256;

/* BLS API Functions */

pub const BFS: usize = big::MODBYTES as usize;
pub const BGS: usize = big::MODBYTES as usize;
pub const BLS_OK: isize = 0;
pub const BLS_FAIL: isize = -1;

// NOTE this must be accessed in unsafe mode.
// But it is just written to once at start-up, so actually quite safe.
static mut G2_TAB: [FP8; ecp::G2_TABLE] = [FP8::new(); ecp::G2_TABLE];

/* hash a message to an ECP point, using SHA3 */

#[allow(non_snake_case)]
pub fn bls_hashit(m: &str) -> ECP {
    let mut sh = SHA3::new(SHAKE256);
    let mut hm: [u8; BFS] = [0; BFS];
    let t = m.as_bytes();
    for i in 0..m.len() {
        sh.process(t[i]);
    }
    sh.shake(&mut hm, BFS);
    let P = ECP::mapit(&hm);
    return P;
}

pub fn init() -> isize {
    let g = ECP4::generator();
    if g.is_infinity() {
        return BLS_FAIL;
    }
    unsafe {
        pair192::precomp(&mut G2_TAB, &g);
    }
    return BLS_OK;
}

/* generate key pair, private key s, public key w */
pub fn key_pair_generate(mut rng: &mut RAND, s: &mut [u8], w: &mut [u8]) -> isize {
    let q = BIG::new_ints(&rom::CURVE_ORDER);
    let g = ECP4::generator();
    if g.is_infinity() {
        return BLS_FAIL;
    }
    let mut sc = BIG::randtrunc(&q, 16 * ecp::AESKEY, &mut rng);
    sc.tobytes(s);
    pair192::g2mul(&g, &mut sc).tobytes(w,true); // true for public key compression
    return BLS_OK;
}

/* Sign message m using private key s to produce signature sig */

pub fn sign(sig: &mut [u8], m: &str, s: &[u8]) -> isize {
    let d = bls_hashit(m);
    let mut sc = BIG::frombytes(&s);
    pair192::g1mul(&d, &mut sc).tobytes(sig, true);
    return BLS_OK;
}

/* Verify signature given message m, the signature sig, and the public key w */

pub fn verify(sig: &[u8], m: &str, w: &[u8]) -> isize {
    let hm = bls_hashit(m);

    let mut d = ECP::frombytes(&sig);
    if !pair192::g1member(&d) {
        return BLS_FAIL;
    }
    d.neg();

    let pk = ECP4::frombytes(&w);

    // Use new multi-pairing mechanism
    let mut r = pair192::initmp();
    //    pair192::another(&mut r,&g,&d);
    unsafe {
        pair192::another_pc(&mut r, &G2_TAB, &d);
    }
    pair192::another(&mut r, &pk, &hm);
    let mut v = pair192::miller(&mut r);

    //.. or alternatively
    //    let mut v = pair192::ate2(&g, &d, &pk, &hm);

    v = pair192::fexp(&v);
    if v.isunity() {
        return BLS_OK;
    }
    return BLS_FAIL;
}
