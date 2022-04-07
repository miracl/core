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

    seedE=ctx.ECDH.stringtobytes("7268600d403fce431561aef583ee1613527cff655c1343f29812e66706df3234");
    seedR=ctx.ECDH.stringtobytes("6db9df30aa07dd42ee5e8181afdb977e538f5e1fec8a06223f33f7013e525037");

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
    console.log("Mode 1 ");

    seedE=ctx.ECDH.stringtobytes("78628c354e46f3e169bd231be7b2ff1c77aa302460a26dbfa15515684c00130b");
    seedR=ctx.ECDH.stringtobytes("d4a09d09f575fef425905d2ab396c1449141463f698f8efdb7accfaff8995098");

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
    console.log("Mode 2 ");

    seedE=ctx.ECDH.stringtobytes("6e6d8f200ea2fb20c30b003a8b4f433d2f4ed4c2658d5bc8ce2fef718059c9f7");
    seedR=ctx.ECDH.stringtobytes("f1d4a30a4cef8d6d4e3b016e6fd3799ea057db4f345472ed302a67ce1c20cdec");
    seedS=ctx.ECDH.stringtobytes("94b020ce91d73fca4649006c7e7329a67b40c55e9e93cc907d282bbbff386f58");
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
    console.log("Mode 3 ");

    seedE=ctx.ECDH.stringtobytes("4303619085a20ebcf18edd22782952b8a7161e1dbae6e46e143a52a96127cf84");
    seedR=ctx.ECDH.stringtobytes("4b16221f3b269a88e207270b5e1de28cb01f847841b344b8314d6a622fe5ee90");
    seedS=ctx.ECDH.stringtobytes("62f77dcf5df0dd7eac54eac9f654f426d4161ec850cc65c54f8b65d2e0b4e345");
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

    seedE=ctx.ECDH.stringtobytes("7f06ab8215105fc46aceeb2e3dc5028b44364f960426eb0d8e4026c2f8b5d7e7a986688f1591abf5ab753c357a5d6f0440414b4ed4ede71317772ac98d9239f70904");
    seedR=ctx.ECDH.stringtobytes("2ad954bbe39b7122529f7dde780bff626cd97f850d0784a432784e69d86eccaade43b6c10a8ffdb94bf943c6da479db137914ec835a7e715e36e45e29b587bab3bf1");
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
    console.log("Mode 1 ");

    seedE=ctx.ECDH.stringtobytes("f3ebfa9a69a924e672114fcd9e06fa9559e937f7eccce4181a2b506df53dbe514be12f094bb28e01de19dd345b4f7ede5ad7eaa6b9c3019592ec68eaae9a14732ce0");
    seedR=ctx.ECDH.stringtobytes("a2a2458705e278e574f835effecd18232f8a4c459e7550a09d44348ae5d3b1ea9d95c51995e657ad6f7cae659f5e186126a471c017f8f5e41da9eba74d4e0473e179");
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
    console.log("Mode 2 ");

    seedE=ctx.ECDH.stringtobytes("fe1c589c2a05893895a537f38c7cb4300b5a7e8fef3d6ccb8f07a498029c61e90262e009dc254c7f6235f9c6b2fd6aeff0a714db131b09258c16e217b7bd2aa619b0");
    seedR=ctx.ECDH.stringtobytes("8feea0438481fc0ecd470d6adfcda334a759c6b8650452c5a5dd9b2dd2cc9be33d2bb7ee64605fc07ab4664a58bb9a8de80defe510b6c97d2daf85b92cd4bb0a66bf");
    seedS=ctx.ECDH.stringtobytes("2f66a68b85ef04822b054ef521838c00c64f8b6226935593b69e13a1a2461a4f1a74c10c836e87eed150c0db85d4e4f506cbb746149befac6f5c07dc48a615ef92db");
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
    console.log("Mode 3 ");

    seedE=ctx.ECDH.stringtobytes("54272797b1fbc128a6967ff1fd606e0c67868f7762ce1421439cbc9e90ce1b28d566e6c2acbce712e48eebf236696eb680849d6873e9959395b2931975d61d38bd6c");
    seedR=ctx.ECDH.stringtobytes("3db434a8bc25b27eb0c590dc64997ab1378a99f52b2cb5a5a5b2fa540888f6c0f09794c654f4468524e040e6b4eca2c9dcf229f908b9d318f960cc9e9baa92c5eee6");
    seedS=ctx.ECDH.stringtobytes("65d523d9b37e1273eb25ad0527d3a7bd33f67208dd1666d9904c6bc04969ae5831a8b849e7ff642581f2c3e56be84609600d3c6bbdaded3f6989c37d2892b1e978d5");
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
