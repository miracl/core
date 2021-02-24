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

/* CORE X.509 Functions */

pub struct PKTYPE {
    pub kind: usize,
    pub hash: usize,
    pub curve: usize,
    pub len: usize,
}

pub struct FDTYPE {
    pub index: usize,
    pub length: usize,
}

// Supported Encryption Methods

pub const ECC:usize = 1;
pub const RSA:usize = 2;

// Supported Hash functions

pub const H256:usize = 2;
pub const H384:usize = 3;
pub const H512:usize = 4;

// Supported Curves

pub const USE_NIST256:usize = 4;    /**< For the NIST 256-bit standard curve - WEIERSTRASS only */
pub const USE_C25519:usize = 1;     /**< Bernstein's Modulus 2^255-19 - EDWARDS or MONTGOMERY only */
//const USE_BRAINPOOL:usize = 2;  /**< For Brainpool 256-bit curve - WEIERSTRASS only */
//const USE_ANSSI:usize = 3;      /**< For French 256-bit standard curve - WEIERSTRASS only */
pub const USE_NIST384:usize = 10;   /**< For the NIST 384-bit standard curve - WEIERSTRASS only */
pub const USE_NIST521:usize = 12;   /**< For the NIST 521-bit standard curve - WEIERSTRASS only */

const ANY: u8 = 0x00;
const SEQ: u8 = 0x30;
const OID: u8 = 0x006;
const INT: u8 = 0x02;
//const NUL: u8 = 0x05;
//const ZER: u8 = 0x00;
//const UTF: u8 = 0x0C;
const UTC: u8 = 0x17;
//const LOG: u8 = 0x01;
const BIT: u8 = 0x03;
const OCT: u8 = 0x04;
//const STR: u8 = 0x13;
const SET: u8 = 0x31;
//const IA5: u8 = 0x16;
const EXT: u8 = 0xA3;
const DNS: u8 = 0x82;

// Define some OIDs
// Elliptic Curve with SHA256
const ECCSHA256:[u8;8]=[0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02];
const ECCSHA384:[u8;8]=[0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03];
const ECCSHA512:[u8;8]=[0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x04];
const ECPK:[u8;7]=[0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01];
const PRIME25519:[u8;9]=[0x2B, 0x06, 0x01, 0x04, 0x01, 0xDA, 0x47, 0x0F, 0x01];
const PRIME256V1:[u8;8]=[0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07];
const SECP384R1:[u8;5]=[0x2B, 0x81, 0x04, 0x00, 0x22];
const SECP521R1:[u8;5]=[0x2B, 0x81, 0x04, 0x00, 0x23];
const RSAPK:[u8;9]=[0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01];
const RSASHA256:[u8;9]=[0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b];
const RSASHA384:[u8;9]=[0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0];
const RSASHA512:[u8;9]=[0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0d];

// Cert details

pub const CN:[u8;3]=[0x55, 0x04, 0x06]; // countryName
pub const SN:[u8;3]=[0x55, 0x04, 0x08]; // stateName
pub const LN:[u8;3]=[0x55, 0x04, 0x07]; // localName
pub const ON:[u8;3]=[0x55, 0x04, 0x0A]; // orgName
pub const UN:[u8;3]=[0x55, 0x04, 0x0B]; // unitName
pub const MN:[u8;3]=[0x55, 0x04, 0x03]; // myName
pub const EN:[u8;9]=[0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01]; // emailName

// Extensions
pub const AN:[u8;3]=[0x55,0x1D,0x11]; // altName
pub const KU:[u8;3]=[0x55,0x1D,0x0F]; // keyUsage
pub const BC:[u8;3]=[0x55,0x1D,0x13]; // basicConstraints

fn getalen(tag: u8,b:&[u8],j:usize) -> usize {
    let mut k=j;
    let mut len:usize;
    if tag!=0 && b[k]!=tag {
        return 0;
    }
    k+=1;
    if b[k] == 0x81 {
        k+=1;
        len=b[k] as usize;
    } else if b[k]==0x82 {
        k+=1;
        len=256*(b[k] as usize); k+=1;
        len+= b[k] as usize;
    } else {
        len=b[k] as usize;
        if len>127 {
            return 0;
        }
    }
    return len;
}

fn skip(len: usize) -> usize {
    if len<128 {
        return 2;
    }
    if len<256 {
        return 3;
    }
    return 4;
}

fn bround(len:usize) -> usize {
    if len%8 == 0 {
        return len;
    }
    return len+(8-len%8);
}

impl PKTYPE {
    pub fn new() -> PKTYPE {
        PKTYPE {
            kind: 0,
            hash: 0,
            curve:0,
            len:0,
        }
    }
}

impl FDTYPE {
    pub fn new() -> FDTYPE {
        FDTYPE {
            index: 0,
            length: 0,
        }
    }
}


//  Input signed cert as octet, and extract signature
//  Return 0 for failure, ECC for Elliptic Curve signature, RSA for RSA signature
//  Note that signature type is not provided here - its the type of the public key that
//  is used to verify it that matters, and which determines for example the curve to be used!

pub fn extract_cert_sig(sc: &[u8],sig: &mut [u8]) -> PKTYPE {
    let mut soid:[u8;9]=[0;9];
    let mut ret=PKTYPE::new();

    let mut j=0 as usize;
    let mut len=getalen(SEQ,sc,j);  // Check for expected SEQ clause, and get length
    if len == 0  {                  // if not a SEQ clause, there is a problem, exit
        return ret;
    }
    j+=skip(len);                   // skip over length to clause contents. Add len to skip clause

    if len+j != sc.len() {
        return ret;
    }

    len=getalen(SEQ,sc,j);
    if len==0 {
        return ret;
    }
    j+=skip(len) + len; // jump over cert to signature OID

    len=getalen(SEQ,sc,j);
    if len==0 {
        return ret;
    }
    j+=skip(len);

    let sj=j+len;      // Needed to jump over signature OID

// dive in to extract OID
    len=getalen(OID,sc,j);
    if len==0 {
        return ret;
    }
    j+=skip(len);

    let mut fin=j+len;
    let mut slen=0;
    while j<fin {
        soid[slen]=sc[j];
        slen+=1;
        j+=1;

    }

    if ECCSHA256 == soid[0..slen] {
        ret.kind=ECC;
        ret.hash=H256;
    }
    if ECCSHA384 == soid[0..slen] {
        ret.kind=ECC;
        ret.hash=H384;
    } 
    if ECCSHA512 == soid[0..slen] {
        ret.kind=ECC;
        ret.hash=H512;
    }
    if RSASHA256 == soid[0..slen] {
        ret.kind=RSA;
        ret.hash=H256;
    }
    if RSASHA384 == soid[0..slen] {
        ret.kind=RSA;
        ret.hash=H384;
    }
    if RSASHA512 == soid[0..slen] {
        ret.kind=RSA;
        ret.hash=H512;
    }

    if ret.kind==0 { 
        return ret;  // unsupported type
    }

    j=sj;
    len=getalen(BIT,sc,j);
    if len==0 {
        ret.kind=0;
        return ret;
    }
    j+=skip(len);
    j+=1;
    len-=1; // skip bit shift (hopefully 0!)

    if ret.kind==ECC {
        len=getalen(SEQ,sc,j);
        if len==0 {
            ret.kind=0;
            return ret;
        }
        j+=skip(len);

        // pick up r part of signature
        len=getalen(INT,sc,j);
        if len==0 {
            ret.kind=0;
            return ret;
        }
        j+=skip(len);
        if sc[j]==0 { // skip leading zero
            j+=1;
            len-=1;
        }
        let mut rlen=bround(len);
        let mut ex=rlen-len;
        ret.len=2*rlen;

        slen=0;
        for _ in 0..ex {
            sig[slen]=0;
            slen+=1;
        }
        fin=j+len;
        while j<fin {
            sig[slen]=sc[j];
            j+=1;
            slen+=1;
        }
        // pick up s part of signature
        len=getalen(INT,sc,j);
        if len==0 {
            ret.kind=0;
            return ret;
        }
        j+=skip(len);
        if sc[j]==0 { // skip leading zero
            j+=1;
            len-=1;
        }
        rlen=bround(len);
        ex=rlen-len;
        for _ in 0..ex {
            sig[slen]=0;
            slen+=1;
        }
        fin=j+len;
        while j<fin {
            sig[slen]=sc[j];
            j+=1;
            slen+=1;
        }
        if ret.hash==H256 {
            ret.curve=USE_NIST256;
        }
        if ret.hash==H384 {
            ret.curve=USE_NIST384;
        }
        if ret.hash==H512 {
            ret.curve=USE_NIST521;
        }
    }
    if ret.kind==RSA {
        let rlen=bround(len);
        let ex=rlen-len;
        ret.len=rlen;
        slen=0;
        for _ in 0..ex {
            sig[slen]=0;
            slen+=1;
        }
        fin=j+len;
        while j<fin {
            sig[slen]=sc[j];
            j+=1;
            slen+=1;
        }
        ret.curve=8*rlen;
    }
    return ret;
}

// Extract certificate from signed cert
pub fn extract_cert(sc: &[u8],cert: &mut [u8]) -> usize {
    let mut j:usize=0;

    let mut len=getalen(SEQ,sc,j);
    if len==0 {
        return 0;
    }
    j+=skip(len);

    let k=j;
    len=getalen(SEQ,sc,j);
    if len==0 {
        return 0;
    }
    j+=skip(len);

    let fin=j+len;
    for i in k..fin {
        cert[i-k]=sc[i];
    }
    return fin-k;
}

// Extract Public Key from inside Certificate
pub fn extract_public_key(c: &[u8],key: &mut [u8]) -> PKTYPE {
    let mut koid:[u8;12]=[0;12];
    let mut ret=PKTYPE::new();

    let mut j:usize=0;
    let mut len=getalen(SEQ,c,j);
    if len==0 {
        return ret;
    }
    j+=skip(len);

    if len+j != c.len() {
        return ret;
    }

    len=getalen(ANY,c,j);
    if len==0 {
        return ret;
    }
    j+=skip(len)+len; //jump over version clause

    len=getalen(INT,c,j);
    if len>0 {
        j+=skip(len)+len; // jump over serial number clause (if there is one)
    }

    len=getalen(SEQ,c,j);
    if len==0 {
        return ret;
    }
    j+=skip(len)+len; // jump over signature algorithm

    len=getalen(SEQ,c,j);
    if len==0 {
        return ret;
    }
    j += skip(len) + len; // skip issuer

    len=getalen(SEQ,c,j);
    if len==0 {
        return ret;
    }
    j += skip(len) + len; // skip validity

    len=getalen(SEQ,c,j);
    if len==0 {
        return ret;
    }
    j += skip(len) + len; // skip subject

    len=getalen(SEQ,c,j);
    if len==0 {
        return ret;
    }
    j += skip(len); // 

    len=getalen(SEQ,c,j);
    if len==0 {
        return ret;
    }
    j += skip(len); //

// ** Maybe dive in and check Public Key OIDs here?
// ecpublicKey & prime256v1, secp384r1 or secp521r1 for ECC
// rsapublicKey for RSA

    let sj=j+len;

    len=getalen(OID,c,j);
    if len==0 {
        return ret;
    }
    j += skip(len); 

    let mut fin=j+len;
    let mut slen=0;
    while j<fin {
        koid[slen]=c[j];
        slen+=1;
        j+=1;
    }
    ret.kind=0;
    if ECPK == koid[0..slen] {
        ret.kind=ECC;
    }
    if RSAPK == koid[0..slen] {
        ret.kind=RSA;
    }

    if ret.kind==0 {
        return ret;
    }
    if ret.kind==ECC {
        len=getalen(OID,c,j);
        if len==0 {
            ret.kind=0;
            return ret;
        }
        j+=skip(len);

        fin=j+len;
        slen=0;
        while j<fin {
            koid[slen]=c[j];
            slen+=1;
            j+=1;
        }
        if PRIME25519==koid[0..slen] {
            ret.curve=USE_C25519;
        }
        if PRIME256V1==koid[0..slen] {
            ret.curve=USE_NIST256;
        }
        if SECP384R1==koid[0..slen] {
            ret.curve=USE_NIST384;
        }
        if SECP521R1==koid[0..slen] {
            ret.curve=USE_NIST521;
        }
    }
    j=sj;

    len=getalen(BIT,c,j);
    if len==0 {
        ret.kind=0;
        return ret;
    }
    j+=skip(len);
    j+=1;
    len-=1; // skip bit shift (hopefully 0!)

    if ret.kind==ECC {
        ret.len=len;
        fin=j+len;
        slen=0;
        while j<fin {
            key[slen]=c[j];
            slen+=1;
            j+=1;
        }
    }
    if ret.kind==RSA { // // Key is (modulus,exponent) - assume exponent is 65537
        len=getalen(SEQ,c,j);
        if len==0 {
            ret.kind=0;
            return ret;
        }
        j+=skip(len);

        len=getalen(INT,c,j);
        if len==0 {
            ret.kind=0;
            return ret;
        }
        j+=skip(len);
        if c[j]==0 {
            j+=1;
            len-=1;
        }

        ret.len=len;
        fin=j+len;
        slen=0;
        while j<fin {
            key[slen]=c[j];
            slen+=1;
            j+=1;
        }
        ret.curve=8*len;
    }
    return ret; 
}

pub fn find_issuer(c: &[u8]) -> usize {
    let mut j:usize=0;
    let mut len=getalen(SEQ,c,j);
    if len==0 {
        return 0;
    }
    j += skip(len); 

    if len+j!=c.len() {
        return 0;
    }

    len=getalen(ANY,c,j);
    if len==0 {
        return 0;
    }
    j += skip(len)+len; // jump over version clause
    
    len=getalen(INT,c,j);
    if len>0 {
        j+=skip(len)+len; // jump over serial number clause (if there is one)
    }

    len=getalen(SEQ,c,j);
    if len==0 {
        return 0;
    }
    j += skip(len) + len; // jump over signature algorithm

    return j;
}

pub fn find_validity(c: &[u8]) -> usize {
    let mut j=find_issuer(c);
    let len=getalen(SEQ,c,j);
    if len==0 {
        return 0;
    }
    j+=skip(len)+len; // skip issuer
    return j;
}

pub fn find_subject(c: &[u8]) -> usize {
    let mut j=find_validity(c);
    let len=getalen(SEQ,c,j);
    if len==0 {
        return 0;
    }
    j+=skip(len)+len; // skip validity
    return j;
}

pub fn self_signed(c: &[u8]) -> bool {
    let mut ksub=find_subject(c);
    let mut kiss=find_issuer(c);

    let sublen=getalen(SEQ,c,ksub);
    let isslen=getalen(SEQ,c,kiss);
    if sublen != isslen {
        return false;
    }
    ksub+=skip(sublen);
    kiss+=skip(isslen);
    let mut m:u8=0;
    for i in 0..sublen {
        m |= c[i+ksub]-c[i+kiss];
    }
    if m!=0 {
        return false;
    }
    return true;
}

// NOTE: When extracting cert information, we actually return just an index to the data inside the cert, and maybe its length
// So no memory is assigned to store cert info. It is the callers responsibility to allocate such memory if required, and copy
// cert information into it.

// Find entity property indicated by SOID, given start of issuer or subject field. Return index in cert, flen=length of field

pub fn find_entity_property(c: &[u8],soid: &[u8],start: usize) -> FDTYPE {
    let mut ret=FDTYPE::new();
    let mut foid:[u8;32]=[0;32];
    
    let mut j=start;
    let tlen=getalen(SEQ,c,j);
    if tlen==0 {
        return ret;
    }
    j+=skip(tlen);
    let k=j;
    while j<k+tlen {
        let mut len=getalen(SET,c,j);
        if len==0 {
            return ret;
        }
        j+=skip(len);
        len=getalen(SEQ,c,j);
        if len==0 {
            return ret;
        }
        j+=skip(len);
        len=getalen(OID,c,j);
        if len==0 {
            return ret;
        }
        j+=skip(len);
        let fin=j+len;
        let mut flen:usize=0;
        while j<fin {
            foid[flen]=c[j];
            flen+=1;
            j+=1;
        }
        len=getalen(ANY,c,j); // get text, could be any type
        if len==0 {
            return ret;
        }
        j+=skip(len);
        if foid[0..flen]==*soid {
            ret.index=j; // if its the right one..
            ret.length=len;
            return ret;
        }
        j+=len; // skip over it
    }
    return ret;
}

pub fn find_start_date(c: &[u8],start: usize) -> usize {
    let mut j=start;
    let mut len=getalen(SEQ,c,j);
    if len==0 {
        return 0;
    }
    j+=skip(len);

    len=getalen(UTC,c,j);
    if len==0 {
        return 0;
    }
    j+=skip(len);
    return j;
}

pub fn find_expiry_date(c: &[u8],start: usize) -> usize {
    let mut j=start;
    let mut len=getalen(SEQ,c,j);
    if len==0 {
        return 0;
    }
    j+=skip(len);

    len=getalen(UTC,c,j);
    if len==0 {
        return 0;
    }
    j+=skip(len)+len;

    len=getalen(UTC,c,j);
    if len==0 {
        return 0;
    }
    j+=skip(len);
    return j;
}

pub fn find_extensions(c: &[u8]) -> usize {
    let mut j=find_subject(c);

    let mut len=getalen(SEQ,c,j);
    if len==0 {
        return 0;
    }
    j+=skip(len)+len; // skip subject

    len=getalen(SEQ,c,j);
    if len==0 {
        return 0;
    }
    j+=skip(len)+len; // skip public key

    if j>=c.len() {
        return 0;
    }
    return j;
}

pub fn find_extension(c: &[u8],soid: &[u8],start:usize) -> FDTYPE {
    let mut ret=FDTYPE::new();
    let mut foid:[u8;32]=[0;32];
    
    let mut j=start;
    let tlen=getalen(EXT,c,j);
    if tlen==0 {
        return ret;
    }
    j+=skip(tlen);

    let tlen=getalen(SEQ,c,j);
    if tlen==0 {
        return ret;
    }
    j+=skip(tlen);

    let k=j;
    while j<k+tlen {
        let mut len=getalen(SEQ,c,j);
        if len==0 {
            return ret;
        } 
        j+=skip(len);
        let nj=j+len;
        len=getalen(OID,c,j);
        j+=skip(len);
        let fin=j+len;
        let mut flen:usize=0;
        while j<fin {
            foid[flen]=c[j];
            flen+=1;
            j+=1;
        }
        if foid[0..flen]==*soid {
            ret.index=j; // if its the right one..
            ret.length=nj-j;
            return ret;
        }
        j=nj;  // skip over this extension
    }
    return ret;
}

// return 1 if name found, else 0, where name is URL
// input cert, and pointer to SAN extension
// Takes wild-card into consideration

pub fn find_alt_name(c: &[u8],start: usize,name: &[u8]) -> bool {
    if start==0 {
        return false;
    }
    let mut j=start;
    let mut tlen=getalen(OCT,c,j);
    if tlen==0 {
        return false;
    }
    j+=skip(tlen);

    tlen=getalen(SEQ,c,j);
    if tlen==0 {
        return false;
    }
    j+=skip(tlen);
    let k=j;
    while j<k+tlen {
        let tag=c[j];
        let mut len=getalen(ANY,c,j);
        if len==0 {
            return false;
        }
        j+=skip(len); // ?? If its not dns, skip over it j+=len
        if tag!=DNS { // only interested in URLs
            j+=len;
            continue;
        }
        let mut cmp=true;
        let mut m=0;
        let nlen=name.len();
        if c[j]=='*' as u8 {
            j+=1; len-=1; // skip over *
            while m<nlen { // advance to first .
                if name[m]=='.' as u8 {
                    break;
                }
                m+=1;
            }
        }
        for _ in 0..len {
            if m==nlen { // name has ended before comparison completed
                cmp=false;
                j+=1;
                continue;
            }
            if c[j] != name[m] {
                cmp=false;
            }
            m+=1; j+=1;
        }
        if m!=nlen {
            cmp=false;
        }
        if cmp {
            return true;
        }
    }
    return false;
}

