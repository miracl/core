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

/* test driver and function exerciser for HPKE API Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hpke_C25519.h"
#include "hpke_NIST521.h"
#include "randapi.h"

using namespace core;

int hpke_C25519(csprng *RNG)
{
    using namespace C25519;

    int res=0;
    int config_id=0x2A; // 01|01|010 = 1, 1, 2
    int i,mode;
    char info[32];
    char skr[EGS_C25519];
    char ski[EGS_C25519];
    char ske[EGS_C25519];
    char psks[6];
    char pskid[32];
    char pkr[EFS_C25519];
    char pki[EFS_C25519];
    char pke[EFS_C25519];
    char z[72];
    char plain[80];
    char cipher[80];
    char aad[32];
    char tag[32];
    
    octet INFO = {0, sizeof(info), info};
    octet skR = {0, sizeof(skr), skr};
    octet skI = {0, sizeof(ski), ski};
    octet skE = {0, sizeof(ske), ske};
    octet psk = {0, sizeof(psks), psks};
    octet pskID = {0,sizeof(pskid), pskid};
    octet pkR = {0, sizeof(pkr), pkr};
    octet pkI = {0, sizeof(pki), pki};
    octet pkE = {0, sizeof(pke), pke};
    octet Z = {0, sizeof(z), z};
    octet PLAIN = {0,sizeof(plain),plain};
    octet CIPHER = {0,sizeof(cipher),cipher};
    octet AAD = {0,sizeof(aad),aad};
    octet TAG = {0,sizeof(tag),tag};

    OCT_fromHex(&INFO,(char *)"4f6465206f6e2061204772656369616e2055726e");
    OCT_fromHex(&psk,(char *)"6d656c6c6f6e");
    OCT_fromHex(&pskID,(char *)"456e6e796e20447572696e206172616e204d6f726961");
    OCT_fromHex(&PLAIN,(char *)"4265617574792069732074727574682c20747275746820626561757479");
    OCT_fromHex(&AAD,(char *)"436f756e742d30");

    char key[32];
    char nonce[12];
    octet KEY = {0, sizeof(key), key};
    octet NONCE = {0, sizeof(nonce), nonce};

// Mode 0

    mode=0;
    printf("\nMode 0\n");
    OCT_fromHex(&skR,(char *)"139c8d38df6d8dfa2c8de98af621667c76c3f63f65c7c3966c4258c316f05033");
    OCT_fromHex(&skI,(char *)"dd5f9525b2fa94f21b7237ade72006a76f612dabf020a02527fcb75db6bebe6f");
    OCT_fromHex(&skE,(char *)"7227b3fee6b8e01b293b826ec8671b69894a1142981bb1513afa42819f2a22ef"); 
    OCT_fromHex(&pkR,(char *)"186c394e175b7b161760b1bd5b822a0804bd066b170c695c0df123176fa7df6f"); 
    OCT_fromHex(&pkI,(char *)"55f618acd854b99bc5167b72c6fbaa70056b6b30d709768658468e830c62dd5d");

    HPKE_Encap(config_id,NULL,&skE,&Z,&pkE,&pkR);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_Decap(config_id,&Z,&pkE,&skR);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,mode,&pkR,&Z,&pkE,&INFO,NULL,NULL,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 1
    mode=1;
    printf("\nMode 1\n");
    OCT_fromHex(&skR,(char *)"fdf4507c4507ececd2b2232a761ce2cdb50d993b6216e6cb9538a1fc53baed02");
    OCT_fromHex(&skI,(char *)"01f60c92806f04327cee19d1822f78a8e88fc0e6d1cada7cd94f902511bb2457");
    OCT_fromHex(&skE,(char *)"0dc7bf88c1849fd7800f8bd52eed75f26160a3cb9d634190c48ced1daa4c341a");
    OCT_fromHex(&pkR,(char *)"696977f50e929c99287852ded3fc9b2709f8cd51c0d270c45c338dece959f437");
    OCT_fromHex(&pkI,(char *)"5bbcd7531308ffc2653b89ad79d6c9df64bd0c981536c08e887f147b8f92d16a");
    
    HPKE_Encap(config_id,NULL,&skE,&Z,&pkE,&pkR);
    
    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_Decap(config_id,&Z,&pkE,&skR);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,mode,&pkR,&Z,&pkE,&INFO,&psk,&pskID,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 2
    mode=2;
    printf("\nMode 2\n");
    OCT_fromHex(&skR,(char *)"a3f26ded71d69e6b7c4924e5756388efce4c8857b89cd00e492cd5a5778c40d7");
    OCT_fromHex(&skI,(char *)"42962c361fe2e92343113407b95abe4eebef311e315726c579fc6fb7f22e16c5");
    OCT_fromHex(&skE,(char *)"372c79a960e1c33c6b6f69c7f4c63567f743a018d5d04fc4a243e0af2aeb3708");
    OCT_fromHex(&pkR,(char *)"39c07ff4a258dfe61a1e9e23ef1d9d39efce89c5326da949a9dfa3b11b9f2f2e");
    OCT_fromHex(&pkI,(char *)"9888840b794623840275e594ec76a526e690b9111d21cdec3fe2faf18227f676");

    HPKE_AuthEncap(config_id,NULL,&skE,&Z,&pkE,&pkR,&skI);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_AuthDecap(config_id,&Z,&pkE,&skR,&pkI);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,mode,&pkR,&Z,&pkE,&INFO,NULL,NULL,&pkI);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 3
    mode=3;
    printf("\nMode 3\n");

    OCT_fromHex(&skR,(char *)"2d7c739195ba102216de162f9435991aa3ad42aeefdb7e22391ae34bae7e5a13");
    OCT_fromHex(&skI,(char *)"59c77f5734aef369f30d83c7e30c6bf372e120391cdaf13f34c915030284b75d");
    OCT_fromHex(&skE,(char *)"6827bbf4f7ebd0182a8ebc0ea364c7ddae1d1c8a4f58a903fa9f9f9d4228b126");
    OCT_fromHex(&pkR,(char *)"cc980df06e532bdb6b957f9f5a5caf55c55f46822cdfbd97e76f6ad4c62b322b");
    OCT_fromHex(&pkI,(char *)"db6ee4a53276b7bc90657cdde514f948af83c140540797ec717881490afed921");

    HPKE_AuthEncap(config_id,NULL,&skE,&Z,&pkE,&pkR,&skI);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_AuthDecap(config_id,&Z,&pkE,&skR,&pkI);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,mode,&pkR,&Z,&pkE,&INFO,&psk,&pskID,&pkI);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);


    return res;
}

int hpke_NIST521(csprng *RNG)
{
    using namespace NIST521;

    int res=0;
    int config_id=0x53; // 10|10|011 = 2, 2, 3
    int i,mode;
    char info[32];
    char skr[EGS_NIST521];
    char ski[EGS_NIST521];
    char ske[EGS_NIST521];
    char psks[6];
    char pskid[32];
    char pkr[2*EFS_NIST521+1];
    char pki[2*EFS_NIST521+1];
    char pke[2*EFS_NIST521+1];
    char z[144];
    char plain[80];
    char cipher[80];
    char aad[32];
    char tag[32];
    
    octet INFO = {0, sizeof(info), info};
    octet skR = {0, sizeof(skr), skr};
    octet skI = {0, sizeof(ski), ski};
    octet skE = {0, sizeof(ske), ske};
    octet psk = {0, sizeof(psks), psks};
    octet pskID = {0,sizeof(pskid), pskid};
    octet pkR = {0, sizeof(pkr), pkr};
    octet pkI = {0, sizeof(pki), pki};
    octet pkE = {0, sizeof(pke), pke};
    octet Z = {0, sizeof(z), z};
    octet PLAIN = {0,sizeof(plain),plain};
    octet CIPHER = {0,sizeof(cipher),cipher};
    octet AAD = {0,sizeof(aad),aad};
    octet TAG = {0,sizeof(tag),tag};

    OCT_fromHex(&INFO,(char *)"4f6465206f6e2061204772656369616e2055726e");
    OCT_fromHex(&psk,(char *)"6d656c6c6f6e");
    OCT_fromHex(&pskID,(char *)"456e6e796e20447572696e206172616e204d6f726961");
    OCT_fromHex(&PLAIN,(char *)"4265617574792069732074727574682c20747275746820626561757479");
    OCT_fromHex(&AAD,(char *)"436f756e742d30");

    char key[32];
    char nonce[12];
    octet KEY = {0, sizeof(key), key};
    octet NONCE = {0, sizeof(nonce), nonce};

// Mode 0

    mode=0;
    printf("\nMode 0\n");
    OCT_fromHex(&skR,(char *)"011d0d02be6fe4eb49654d5641f0ebb40b594aca84a7c1e76faa1fa0f5f3582502d29fd140d07e71f538c75b93c8ca416af95ad79c5e7e2c0889adce490367492aee");
    OCT_fromHex(&skI,(char *)"01f6343e4068d73fc49b7d1701612b264b2ce45c035ac197ad11e3e057333c6b7520a7a76936c69f04315d2a4e9497dceee45ed96e392ebe2f571e2beb8d6b465a8f");
    OCT_fromHex(&skE,(char *)"0099f6326effd78a5c54eeba93e911f35bdd418f2e1de3281ba2172aebf251ff73614598c81686775c20618721c8c84466f2f6ca7b33636a3edbc69697087f8b414a");
    OCT_fromHex(&pkR,(char *)"04015c0e75e50b266e5fb8e3a3f317608bbf64843c47cca29faa3b894a4f9b95ba533bd40d343e5747f9e85d9ffee5b4ed803a20a23ff0a246ca4d9d59eae9c38291d000c396eade14ed2d42fafe7c4be26da6f07af8b6902af5fbb39f8b0b15c7120d7f57f46ed24e711dda2469f418377d8cc8de7ec8b0594e155b8d6d6ca699f99a4189");
    OCT_fromHex(&pkI,(char *)"0400430b1dbd3c329055a1e76b57abf0e9c0e28f01c6daf8cdddee13c0d0668789edcf4bbf7157e75b12b5f91294aaac807060bc8270b5d590fdb5c7e4d6d5b5c8cb5d01c01e9ea663d2a487ea671ea08939e88a300bb8a14c3e439cfcd58266e0124cf02f86dc32dab2aefea0f5485fa85540e2577a0b0e4d3936f03a20a6da341c59546c");

    HPKE_Encap(config_id,NULL,&skE,&Z,&pkE,&pkR);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_Decap(config_id,&Z,&pkE,&skR);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,mode,&pkR,&Z,&pkE,&INFO,NULL,NULL,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 1
    mode=1;
    printf("\nMode 1\n");

    OCT_fromHex(&skR,(char *)"00953f712b5c288be50460d0372612f88c3da171196a4cd958b0f658e988aa56ce5edc018e5899acda38649fc51049610bc8d1423189a9b5ff5f7e4fe1081df3c7b3");
    OCT_fromHex(&skI,(char *)"00c1842b428c23730b0d4eb3859b20bbbe8fef23068e4a6431647379e4a2b92e9c1ab58682e0043b9df9c48df14863a6ded7a3a283258fe534aada53b68082eaa7a9");
    OCT_fromHex(&skE,(char *)"00416c4a6236475f01a0cd0a9a7a0d7229be88a63281fca350ea98d4438de57a23bd5053572bf0f7bed8bc84aba6f7322cabc6b2540e789eb5823660eaad8e206fd9");
    OCT_fromHex(&pkR,(char *)"040108f48ce8fcd71e7e07270fa566cfd66d9b7ab124df141d700b82141264a01306b94265afc3c361c0c0dc514864d4b12e687697d24ddce2f0c77f9c257b8a2f5a380109cb328c87f9833f729967a74675acc31bccdccf5016d442e7b33d2ecaf6f79d8a0e79d8259b07cd173e320ca25c68acf5aa4f3793e7fc6d076baa953060847a02");
    OCT_fromHex(&pkI,(char *)"04016c7b375e83c3e9252a4768986fff255917ca307345b4a953031a0314b8209b5b4d55521fb576db505c9afda9fdfe00ce6f0ad64f6547223179bf851a7cb7b9a71201b9b5b3de83c31691ff10d41e3c38aca4ebf0a858a19d5dd01d247425e6697ef10ad9da8e9467104c32d7ec5605b5835c074da5ce9000edaa8fbc4623d53431815b");
    
    HPKE_Encap(config_id,NULL,&skE,&Z,&pkE,&pkR);
    
    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_Decap(config_id,&Z,&pkE,&skR);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,mode,&pkR,&Z,&pkE,&INFO,&psk,&pskID,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 2
    mode=2;
    printf("\nMode 2\n");

    OCT_fromHex(&skR,(char *)"006ae74c6d37982c4a6087500b66948a715ca971e7aa43260bae4683d78818cfc728b0d72d6834f4d401f35db13f932e414b44d03071805fcbd513a57130e18e8323");
    OCT_fromHex(&skI,(char *)"01d9581f65c8cf1a90f1711fe377c15e68f534be11ea5e0158a8adebaa04f0be9c80d0f2517abf0cd117d9ca2073b604743076cee2405f4db2825ace05e0eae83354");
    OCT_fromHex(&skE,(char *)"0195e8805187cf89fc17007d90a75dff2dc9ae824aea70adf001ef539832932f0d8cd7d3bbc94e712fb64d0e5f0acbb7cb79e5bde9d24304c8b4ed0091c8905da986");
    OCT_fromHex(&pkR,(char *)"040054116ccfb36d9cd99e59b100ef9dbc70a6992b38632ee7650659275cdcab37bce7e74f2381cc7292ba418051432c3a8aeb706b3c05fcb886b3a95a306ae9863f5900e9b6db3150e7241fec23c607db539e1a2b2c1898b5d2b78ec1a3254bf022dee6e8c2f6265f71ac8e6003614accb8532dd58d5a07a959bde06b763f2f41a9c3ac32");
    OCT_fromHex(&pkI,(char *)"0400fdb5f8a16b80e8a61c967f4f4f6b83740fc3521cd1654ee7e1cc42f4f645a57e68942303a48d37e09b7cf2be067f03ed0b7076fe0eda129bc65209c83bafbc0b5d012ba9db99b61236f645076e7f8b10c8763517dfcefd07241e90aa6a595209fc6aafc227fd9d870c8c6b8d037dd5386513608f7779887e47db733fe97f74169d21c7");

    HPKE_AuthEncap(config_id,NULL,&skE,&Z,&pkE,&pkR,&skI);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_AuthDecap(config_id,&Z,&pkE,&skR,&pkI);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,mode,&pkR,&Z,&pkE,&INFO,NULL,NULL,&pkI);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 3
    mode=3;
    printf("\nMode 3\n");

    OCT_fromHex(&skR,(char *)"010bf0c9af1d5dda4c97151d6a9425c8f590aa7c0adff53c06d23380ae82bbd32f64ddf0c344d221d2f7657711d73de7fa25a75bd8fa75662029e118087276a5ef0b");
    OCT_fromHex(&skI,(char *)"01800af579dbd0c91a09fa0ff3ddf21d9a1447649528d777e00962c748159e6de03880a70331fdab67761e25f78977128f81c7fee9897eea74b5ee80e94414b6aed6");
    OCT_fromHex(&skE,(char *)"013e65c19df676b1d7da923475c72c359fbdd91f224d68785bdf5891bbadfd136a33cc8c31408b9652be389f52e8a19d9401aedaf549a0654f246c277af48f59b2ba");
    OCT_fromHex(&pkR,(char *)"0400a1735a659c6b281e603f170b5e886ccfff35d274b042d43a815447fc80208f599704d63a67355b4d04bfdc367f60961f1b36b1e11bf3ed7599f58948f364a1023501ec6b5acd7ce1cc2c3ec6dba8d72b08e472809623ac2df65dcb658dbd7d5f599c9ac624517d780b49b3d7619f5647be76a56a73fe2c3fc2ae620159cb1b7a437a94");
    OCT_fromHex(&pkI,(char *)"0400f0fe8e86b8d86d02a4dc5db6988f6343067174caff2e2700834b98d25045013eb24cf00c03dd506d562625d0fe5c576910d176705cd4ba1fcd10f5a98f466a0a2200a629f9f62f6053b554bf09b2a547b844f3e040c2b92c548babbc73cf05e77f23d0cffa9e5dfd0a57f9be64bf453ec48cbd00f2e47349572fead07dc73658256331");

    HPKE_AuthEncap(config_id,NULL,&skE,&Z,&pkE,&pkR,&skI);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_AuthDecap(config_id,&Z,&pkE,&skR,&pkI);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,mode,&pkR,&Z,&pkE,&INFO,&psk,&pskID,&pkI);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);
}

int main()
{
    int i, res;
    unsigned long ran;

    char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNG;                // Crypto Strong RNG

    time((time_t *)&ran);

    RAW.len = 100;              // fake random seed source
    RAW.val[0] = ran;
    RAW.val[1] = ran >> 8;
    RAW.val[2] = ran >> 16;
    RAW.val[3] = ran >> 24;
    for (i = 4; i < 100; i++) RAW.val[i] = i;

    CREATE_CSPRNG(&RNG, &RAW);  // initialise strong RNG

    printf("%d bit build", CHUNK);

    printf("\nTesting HPKE for curve C25519\n");
    hpke_C25519(&RNG);

    printf("\nTesting HPKE for curve NIST521\n");
    hpke_NIST521(&RNG);

    KILL_CSPRNG(&RNG);
}


