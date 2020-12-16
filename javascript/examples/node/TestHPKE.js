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

    seedE=ctx.ECDH.stringtobytes("6305de86b3cec022fae6f2f2d2951f0f90c8662112124fd62f17e0a99bdbd08e");
    seedR=ctx.ECDH.stringtobytes("6d9014e4609687b0a3670a22f2a14eac5ae6ad8c0beb62fb3ecb13dc8ebf5e06");

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

    seedE=ctx.ECDH.stringtobytes("abd63dfd2fb9ccf8789cae5a6aff91e07f9f5925c27f005e702bf956b0000a85");
    seedR=ctx.ECDH.stringtobytes("654e8b44e8e29fc75f3beadf7f28dc065e38a53c1a731e15f2d46fd6130574da");

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

    seedE=ctx.ECDH.stringtobytes("42d691088397246b00e9d9ce8f5406a317433558dc28132e02618970005d02fd");
    seedR=ctx.ECDH.stringtobytes("fb953f486ef5f7a1ceddff40bffe02b857c8af9d611966e417a24d6efa7c9d1c");
    seedS=ctx.ECDH.stringtobytes("131aa907c85b05726e7a058b064bf29cb2cb72a2afbffbd8076a884291f3143e");
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

    seedE=ctx.ECDH.stringtobytes("0c3a14fe896a7166f4d5e2a21c903b69f9ca71783290ca10f1b8c5eb258149be");
    seedR=ctx.ECDH.stringtobytes("b759021868fba28c1ddd509eaac450a896ab0f5edffead0a019fecb574950d64");
    seedS=ctx.ECDH.stringtobytes("2e7219703b6659698e4c2d141d13e0092df7039212db9c97e347c7d2c0aee239");
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

    seedE=ctx.ECDH.stringtobytes("ea39fec1941c5f516e19533f40d415c65fde023c10c559f3845e71ffccea478101573d069cc67874d5b2aba6a22eb51cdd689836b7e9cabbb4469c57947db7316fa7");
    seedR=ctx.ECDH.stringtobytes("8249fd42416aba5b0d51dcd3548d774ae172148cbba1519107c5d84a160225441a9c018fdf3b9ffc2c41c1c62e29208d5165a59e7f14fe93b4f911cbbebda1904391");
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

    seedE=ctx.ECDH.stringtobytes("b563555965facaa37a5e754cf2e50193953e1d527e61637e521df1868354799258f0d15d8807750cea08e9b6a358d2440ab06725861b237fb69973cf41e802434433");
    seedR=ctx.ECDH.stringtobytes("2be954cd856c0c548ec6de490c821be20c4fc9610b5dba1831c698045fa01dbceb49f8324bae26e5d1ee62db3246d65492812e2c539b96bc580d46c247304adfc55e");
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

    seedE=ctx.ECDH.stringtobytes("c9621b9ef899275dc970606a2b0806fe860f62d539f3ee618a9409009b8ae154bc4acf495dd9fa8f850c4dca82b923b42270d7b16ed343c7e86e3036c88d0d7d77ee");
    seedR=ctx.ECDH.stringtobytes("ef805c20cda1fa06e06cfb968ad68c748a3dd94337f7357ca0060a382a84fa5de6df3e3216f886957694547264d5bc63450cfdcc4d2b33fc8ebf8d7c708f8b5e4bb5");
    seedS=ctx.ECDH.stringtobytes("d8779e14425887ebb21b1952b1a0b77842830aef910724b082807dfebc8ec309b4969da762369e77834593970215b85510c9a0347ff14c8583aae7c9c2208275b740");
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

    seedE=ctx.ECDH.stringtobytes("d7537fd470c0beece615e26dd109922460292e973127cb4e22da16c0756fc336224e07dbecdf36edd144ebcd82aece3db52f814a33a639b5e7c964b27f6e3195cd73");
    seedR=ctx.ECDH.stringtobytes("f0858f5e1865db4fe45dc3274bcd273a29088d80f9203a16ec1210e3d81dd50a99f15c427d547fea55593e2ef834beb5f80c536fdd2881a8943c05488a371a3c988d");
    seedS=ctx.ECDH.stringtobytes("1e8d0026273feb61537181872e03ed2c7756f0ed1c4bb9ecd159614c2afdcaacc5fcf70f6d30d7ea6760c98a1ce1138a82497eb72461ca5da50c8729d431de53857f");
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
