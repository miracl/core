const CTX = require('../../index').CTX;

// C25519 context
function C25519() {
	var ctx = new CTX('C25519');
	
	console.log("X25519 Curve "+  "");
	var config_id=0x520;

	var pkE=[];
	var INFO=ctx.ECDH.stringtobytes("4f6465206f6e2061204772656369616e2055726e");
	var psk=ctx.ECDH.stringtobytes("0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82");
	var pskID=ctx.ECDH.stringtobytes("456e6e796e20447572696e206172616e204d6f726961");
	var plain=ctx.ECDH.stringtobytes("4265617574792069732074727574682c20747275746820626561757479");
	var aad=ctx.ECDH.stringtobytes("436f756e742d30");
    console.log("Testing HPKE code ");

	var key=[];
	var nonce=[];
	var exp_secret=[];
	var cipher=[];
	var tag=[];
	var seedE,seedR,seedS;
	var skR=[];
	var skE=[];
	var skS=[];
	var pkR=[];
	var pkS=[];
	var pkE=[];

	var mode,Z;
// Mode 0
    mode=0;
    console.log("Mode 0 ");

    seedE=ctx.ECDH.stringtobytes("1dacee520c81ade608f4fa3e5ccae0ecedcc7880e3fc6f3e5afd2e4af8396571");
    seedR=ctx.ECDH.stringtobytes("0a3367dadc97e200074936b5adedcd5680f30672d1ec7158fdfcb795040ec909");

	ctx.HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
	ctx.HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);

	Z=ctx.HPKE.encap(config_id,skE,pkE,pkR);
	console.log("pKE : 0x"+ctx.ECDH.bytestostring(pkE) );
	console.log("Encapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	Z=ctx.HPKE.decap(config_id,skR,pkE,pkR);
	console.log("Decapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	ctx.HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,null,null);
	console.log("Key :    0x"+ctx.ECDH.bytestostring(key) );
	console.log("Nonce :  0x"+ctx.ECDH.bytestostring(nonce) );
	console.log("Exporter secret :  0x"+ctx.ECDH.bytestostring(exp_secret) );

	ctx.GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	console.log("Cipher : 0x"+ctx.ECDH.bytestostring(cipher) );
	console.log("Tag :    0x"+ctx.ECDH.bytestostring(tag) );

// Mode 1
    mode=1;
    console.log(" Mode 1 ");

    seedE=ctx.ECDH.stringtobytes("7f5b697a87fb11d733e31265410c80b72525637e1b872f9ede00d507c0c534d7");
    seedR=ctx.ECDH.stringtobytes("89a6ce4b7b8eb12eebff3864615a2539e3e64fa2d4ab525c18c7bc451a2b5e40");

	ctx.HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
	ctx.HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);

	Z=ctx.HPKE.encap(config_id,skE,pkE,pkR);
	console.log("pKE : 0x"+ctx.ECDH.bytestostring(pkE) );
	console.log("Encapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	Z=ctx.HPKE.decap(config_id,skR,pkE,pkR);
	console.log("Decapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	ctx.HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,psk,pskID);
	console.log("Key :    0x"+ctx.ECDH.bytestostring(key) );
	console.log("Nonce :  0x"+ctx.ECDH.bytestostring(nonce) );
	console.log("Exporter secret :  0x"+ctx.ECDH.bytestostring(exp_secret) );

	ctx.GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	console.log("Cipher : 0x"+ctx.ECDH.bytestostring(cipher) );
	console.log("Tag :    0x"+ctx.ECDH.bytestostring(tag) );

// Mode 2
    mode=2;
    console.log(" Mode 2 ");

    seedE=ctx.ECDH.stringtobytes("f7faa81395a22af1071b5273f9f94db0609ab9a7f4eca54834ea408d97294cc2");
    seedR=ctx.ECDH.stringtobytes("a10d107753f85bf7128768693e53129fb44a281ea4849a86231632db48dbb28c");
    seedS=ctx.ECDH.stringtobytes("e1257ce84dd98bca7847d75661b658cd2cc6c4efbf9e989a40f390720b57d409");
	ctx.HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
	ctx.HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);
	ctx.HPKE.DeriveKeyPair(config_id,skS,pkS,seedS);

	Z=ctx.HPKE.authEncap(config_id,skE,skS,pkE,pkR,pkS);
	console.log("pKE : 0x"+ctx.ECDH.bytestostring(pkE) );
	console.log("Encapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	Z=ctx.HPKE.authDecap(config_id,skR,pkE,pkR,pkS);
	console.log("Decapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	ctx.HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,null,null);
	console.log("Key :    0x"+ctx.ECDH.bytestostring(key) );
	console.log("Nonce :  0x"+ctx.ECDH.bytestostring(nonce) );
	console.log("Exporter secret :  0x"+ctx.ECDH.bytestostring(exp_secret) );

	ctx.GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	console.log("Cipher : 0x"+ctx.ECDH.bytestostring(cipher) );
	console.log("Tag :    0x"+ctx.ECDH.bytestostring(tag) );

// Mode 3
    mode=3;
    console.log(" Mode 3 ");

    seedE=ctx.ECDH.stringtobytes("e98713f7e24e7025fb7abddfff317d865ad5f4c3b6be22cda48dbcdde6151a68");
    seedR=ctx.ECDH.stringtobytes("b281654cdb2cd6a9670c3035eaa702736a951585a938381198f7d29085ca7f3a");
    seedS=ctx.ECDH.stringtobytes("ea95c4d3c5e7868af29fc0cb0841485dead8d54b57ee1b8ed31a153295454835");
	ctx.HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
	ctx.HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);
	ctx.HPKE.DeriveKeyPair(config_id,skS,pkS,seedS);

	Z=ctx.HPKE.authEncap(config_id,skE,skS,pkE,pkR,pkS);
	console.log("pKE : 0x"+ctx.ECDH.bytestostring(pkE) );
	console.log("Encapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	Z=ctx.HPKE.authDecap(config_id,skR,pkE,pkR,pkS);
	console.log("Decapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	ctx.HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,psk,pskID);
	console.log("Key :    0x"+ctx.ECDH.bytestostring(key) );
	console.log("Nonce :  0x"+ctx.ECDH.bytestostring(nonce) );
	console.log("Exporter secret :  0x"+ctx.ECDH.bytestostring(exp_secret) );

	ctx.GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	console.log("Cipher : 0x"+ctx.ECDH.bytestostring(cipher) );
	console.log("Tag :    0x"+ctx.ECDH.bytestostring(tag) );
}

// NIST521 context
function NIST521() {
	var ctx = new CTX('NIST521');
	
	console.log("NIST521 Curve "+  "");
	var config_id=0xB12;

	var pkE=[];
	var INFO=ctx.ECDH.stringtobytes("4f6465206f6e2061204772656369616e2055726e");
	var psk=ctx.ECDH.stringtobytes("0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82");
	var pskID=ctx.ECDH.stringtobytes("456e6e796e20447572696e206172616e204d6f726961");
	var plain=ctx.ECDH.stringtobytes("4265617574792069732074727574682c20747275746820626561757479");
	var aad=ctx.ECDH.stringtobytes("436f756e742d30");
    console.log("Testing HPKE code ");

	var key=[];
	var nonce=[];
	var exp_secret=[];
	var cipher=[];
	var tag=[];
	var seedE,seedR,seedS;

	var skR=[];
	var skE=[];
	var skS=[];
	var pkR=[];
	var pkS=[];
	var pkE=[];

	var mode,Z;
// Mode 0
    mode=0;
    console.log("Mode 0 ");

    seedE=ctx.ECDH.stringtobytes("bb8f0c6c0d81a0a54d6084e3d7d8001875ce2b4831400075246ddeabc3f62d065d91fa8f2ca0cfee3a94f1fa6f43f53a2e39241ccd5ad9f0d889aba103e78286ca17");
    seedR=ctx.ECDH.stringtobytes("28b1403adcf3d2bba7d7c733df4ef8ab9f4c2cf92c840beb7de3fd139611a95be850589a49825450c033b2e1f6cb66127c83827d5464fca1e427c2bb31bf18e857e2");
	ctx.HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
	ctx.HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);

	Z=ctx.HPKE.encap(config_id,skE,pkE,pkR);
	console.log("pKE : 0x"+ctx.ECDH.bytestostring(pkE) );
	console.log("Encapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	Z=ctx.HPKE.decap(config_id,skR,pkE,pkR);
	console.log("Decapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	ctx.HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,null,null);
	console.log("Key :    0x"+ctx.ECDH.bytestostring(key) );
	console.log("Nonce :  0x"+ctx.ECDH.bytestostring(nonce) );
	console.log("Exporter secret :  0x"+ctx.ECDH.bytestostring(exp_secret) );

	ctx.GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	console.log("Cipher : 0x"+ctx.ECDH.bytestostring(cipher) );
	console.log("Tag :    0x"+ctx.ECDH.bytestostring(tag) );

// Mode 1
    mode=1;
    console.log(" Mode 1 ");

    seedE=ctx.ECDH.stringtobytes("bd12d21f5a36063ec0242aa26f22144aa7d24c3cc2e6a1805868250d8e9ce6fb2a04f8852cd6ec3241249fcf262b8f245d179636fcfbf275a75539cfc756a4dd3f53");
    seedR=ctx.ECDH.stringtobytes("e9a016952866c33da399e64ecd629cf32826285698726f2c22debd4799b8fac112ac1a546ce39452406396a389fb10f51036c7022dc1cdde177b54d005731fb79ce4");
	ctx.HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
	ctx.HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);


	Z=ctx.HPKE.encap(config_id,skE,pkE,pkR);
	console.log("pKE : 0x"+ctx.ECDH.bytestostring(pkE) );
	console.log("Encapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	Z=ctx.HPKE.decap(config_id,skR,pkE,pkR);
	console.log("Decapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	ctx.HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,psk,pskID);
	console.log("Key :    0x"+ctx.ECDH.bytestostring(key) );
	console.log("Nonce :  0x"+ctx.ECDH.bytestostring(nonce) );
	console.log("Exporter secret :  0x"+ctx.ECDH.bytestostring(exp_secret) );

	ctx.GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	console.log("Cipher : 0x"+ctx.ECDH.bytestostring(cipher) );
	console.log("Tag :    0x"+ctx.ECDH.bytestostring(tag) );

// Mode 2
    mode=2;
    console.log(" Mode 2 ");

    seedE=ctx.ECDH.stringtobytes("0e5ec115c172744e793ef333abfd5d9838fd31eef595f0b77a2f5623404aa42f0e77198c2ac78defeae75cc79cf5dc5c7587b6eda733e48f3b17d5e3496482f19cdb");
    seedR=ctx.ECDH.stringtobytes("3f3c12492eee6f1f8029a89002b6539ea9754b48412c14d2fe94368cf1fa473f0171cdb1117aaa447ad6914bfb794ead14a9cd22cd83caa22bd905bd7d614d401ddc");
    seedS=ctx.ECDH.stringtobytes("02d1581b120d0415bda1fb2cbe6a3c81edf28209f0fc1460745ad9018747c31e6d42a36ed1734214312311db80bb3f88c2da6ea7624cf2915d32f2993ebeb92c2934");
	ctx.HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
	ctx.HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);
	ctx.HPKE.DeriveKeyPair(config_id,skS,pkS,seedS);

	Z=ctx.HPKE.authEncap(config_id,skE,skS,pkE,pkR,pkS);
	console.log("pKE : 0x"+ctx.ECDH.bytestostring(pkE) );
	console.log("Encapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	Z=ctx.HPKE.authDecap(config_id,skR,pkE,pkR,pkS);
	console.log("Decapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	ctx.HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,null,null);
	console.log("Key :    0x"+ctx.ECDH.bytestostring(key) );
	console.log("Nonce :  0x"+ctx.ECDH.bytestostring(nonce) );
	console.log("Exporter secret :  0x"+ctx.ECDH.bytestostring(exp_secret) );

	ctx.GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	console.log("Cipher : 0x"+ctx.ECDH.bytestostring(cipher) );
	console.log("Tag :    0x"+ctx.ECDH.bytestostring(tag) );

// Mode 3
    mode=3;
    console.log(" Mode 3 ");

    seedE=ctx.ECDH.stringtobytes("c393a0a0cab3313f0526d72b8bef55b26a156493263fa1a653069ed5f2f3637f371b15704778c89a6e01c3fd958dfee5f85613e2d4b2eb17a152810690d8252df44b");
    seedR=ctx.ECDH.stringtobytes("e7fa07c9938a3d9d883d256abd7fb5777ed7c4195a8d244c0e682fd1a08fdcce58ab1aa7ba31b1fa120f03d9df438509a1e31992506dbc196ac4489079b3b64bea9b");
    seedS=ctx.ECDH.stringtobytes("403e8d5ff0155c71232a887f542df221c385956eaca3acceffdc4681b99c7e9613f90ba03b0c03c0c57a90b2dd31effc6278ab6dc059f3e5389dff6495bda12b4da4");
	ctx.HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
	ctx.HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);
	ctx.HPKE.DeriveKeyPair(config_id,skS,pkS,seedS);

	Z=ctx.HPKE.authEncap(config_id,skE,skS,pkE,pkR,pkS);
	console.log("pKE : 0x"+ctx.ECDH.bytestostring(pkE) );
	console.log("Encapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	Z=ctx.HPKE.authDecap(config_id,skR,pkE,pkR,pkS);
	console.log("Decapsulated Z : 0x"+ctx.ECDH.bytestostring(Z) );

	ctx.HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,psk,pskID);
	console.log("Key :    0x"+ctx.ECDH.bytestostring(key) );
	console.log("Nonce :  0x"+ctx.ECDH.bytestostring(nonce) );
	console.log("Exporter secret :  0x"+ctx.ECDH.bytestostring(exp_secret) );

	ctx.GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	console.log("Cipher : 0x"+ctx.ECDH.bytestostring(cipher) );
	console.log("Tag :    0x"+ctx.ECDH.bytestostring(tag) );

}

C25519();
console.log("\n");
NIST521();
console.log("\n");
