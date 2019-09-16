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

//  TestNHS.rs - Tests NewHope Simple API

// See https://eprint.iacr.org/2016/1157 (Alkim, Ducas, Popplemann and Schwabe)

// rustc TestNHS.rs --extern core=libcore.rlib

extern crate core;

//use std::str;
//use std::io;

use core::rand::RAND;
use core::nhs;

fn main() {
    let mut raw: [u8; 100] = [0; 100];

    println!("\nTesting New Hope Key Exchange");

    let mut srng = RAND::new();
    srng.clean();
    for i in 0..100 {
        raw[i] = (i + 1) as u8
    }

    srng.seed(100, &raw);

    let mut crng = RAND::new();
    crng.clean();
    for i in 0..100 {
        raw[i] = (i + 2) as u8
    }

    crng.seed(100, &raw);

    let mut ss: [u8; 1792] = [0; 1792];
    let mut sb: [u8; 1824] = [0; 1824];
    let mut uc: [u8; 2176] = [0; 2176];

    let mut keya: [u8; 32] = [0; 32];
    let mut keyb: [u8; 32] = [0; 32];

    nhs::server_1(&mut srng, &mut sb, &mut ss);

    nhs::client(&mut crng, &sb, &mut uc, &mut keyb);

    nhs::server_2(&ss, &uc, &mut keya);

    print!("Alice key= ");
    for i in 0..keya.len() {
        print!("{:02X}", keya[i]);
    }
    println!("");

    print!("Bob's key= ");
    for i in 0..keyb.len() {
        print!("{:02X}", keyb[i]);
    }
    println!("");
}
