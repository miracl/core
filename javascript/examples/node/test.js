/*
var readline = require('readline');

var rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
  terminal: false
});

function ask(questionText) {
  return new Promise((resolve, reject) => {
    rl.question(questionText, (input) => resolve(input) );
  });
}

async function start() {
	let pin = await ask("pin: ");
	pin = parseInt(pin);
	console.log("The pin is: " + pin);
	let name = await ask("name: ");
	console.log("The name is: " + name);
	rl.close();
}
start();
*/

const CTX = require('../../index').CTX;


const readline = require('readline');

var rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
  terminal: false
});

function ask(questionText) {
  return new Promise((resolve, reject) => {
    rl.question(questionText, (input) => resolve(input) );
  });
}
let pinNew;
async function start() {
	pinNew = await ask("ENTER PIN= ");
	BN254();
	rl.close();
}
start();
//const rlSync = require('readline-sync');

// BN254 context
async function BN254() {
	var ctx = new CTX('BN254');
	
	console.log("Testing MPIN 2-factor authentication protocol on curve BN254 ");

	let i;

	let S=[];
	let SST=[];
	let TOKEN = [];
	let SEC = [];
	let U = [];
	let X= [];
	let Y= [];
	let HCID=[];
	let HSID=[];

	let RAW=[];

 	let dst = "BN254G1_XMD:SHA-256_SVDW_NU_MPIN";
	let DST = ctx.MPIN.asciitobytes(dst);

	let rng=new ctx.RAND();
	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;
	rng.seed(100,RAW);

// Trusted Authority (TA) set-up
	ctx.MPIN.RANDOM_GENERATE(rng,S);
	console.log("M-Pin Master Secret s: 0x"+ctx.MPIN.bytestostring(S) );

 // Create Client Identity
 		let IDstr = "testUser@miracl.com";
		let CLIENT_ID = ctx.MPIN.asciitobytes(IDstr);
		ctx.MPIN.ENCODE_TO_CURVE(DST,CLIENT_ID,HCID);
		console.log("Client ID Hashed to Curve= "+ctx.MPIN.bytestostring(HCID) );

// Client approaches Trusted Authority and is issued secret

	ctx.MPIN.GET_CLIENT_SECRET(S,HCID,TOKEN);
	console.log("Client Secret CS: 0x"+ctx.MPIN.bytestostring(TOKEN) );
// TA sends Client secret to Client

// Server is issued secret by TA
	ctx.MPIN.GET_SERVER_SECRET(S,SST);
//	console.log("Server Secret SS: 0x"+ctx.MPIN.bytestostring(SST) );


// Client extracts PIN from secret to create Token
		let pin=1234;
		console.log("Client extracts PIN= "+pin );
		let rtn=ctx.MPIN.EXTRACT_PIN(HCID,pin,TOKEN);
		if (rtn != 0)
			console.log("Failed to extract PIN " );
		console.log("Client Token TK: 0x"+ctx.MPIN.bytestostring(TOKEN) );

// Exercise Secret Sharing
            let R=[];
            for (let i=0;i<128;i++)
                R[i]=rng.getByte();
       // create 4 unique shares of TOKEN
            var Sh1=new ctx.SHARE(1,3,TOKEN,R);  // indicate 3 shares required for recovery
            var Sh2=new ctx.SHARE(2,3,TOKEN,R);
            var Sh3=new ctx.SHARE(3,3,TOKEN,R);
            var Sh4=new ctx.SHARE(4,3,TOKEN,R);

            var Shares=[];

            Shares[0]=Sh1;  // any 3 shares to recover TOKEN
            Shares[1]=Sh2;
            Shares[2]=Sh4;

            TOKEN=ctx.SHARE.recover(Shares);  // recover token

// MPin Protocol

// Client enters ID and PIN
		//pin=parseInt(mywindow.prompt("Enter PIN= "));
		//rl.resume();
		//pin = await ask("Enter PIN= ");
		//rl.pause();
		console.log("pin...: " + pin);
		//pin = rlSync.question("Enter PIN= ");
		pin = parseInt(pinNew);

		rtn=ctx.MPIN.CLIENT_1(HCID,rng,X,pin,TOKEN,SEC,U);
		if (rtn != 0)
			console.log("FAILURE: CLIENT_1 rtn: " + rtn );

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
				ctx.MPIN.ENCODE_TO_CURVE(DST,CLIENT_ID,HSID);

// Server generates Random number Y and sends it to Client
				ctx.MPIN.RANDOM_GENERATE(rng,Y);

// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
		rtn=ctx.MPIN.CLIENT_2(X,Y,SEC);
		if (rtn != 0)
			console.log("FAILURE: CLIENT_2 rtn: " + rtn );

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST.
				rtn=ctx.MPIN.SERVER(HSID,Y,SST,U,SEC);

				if (rtn != 0)
				{
					if (rtn == ctx.MPIN.BAD_PIN)
					{
						console.log("Server says - Bad Pin. I don't know you. Feck off." );
					} else {
						console.log("FAILURE: SERVER rtn: " + rtn);
					}
				} else {
					console.log("Server says - PIN is good! You really are "+IDstr );
				}
	//rl.close();
}

//BN254();
//console.log("something...");
//rl.prompt();
//BN254();
//rl.close();
