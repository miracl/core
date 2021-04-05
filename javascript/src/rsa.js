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

/* RSA API Functions */
var RSA,
    rsa_private_key,
    rsa_public_key;

RSA = function(ctx) {
    "use strict";

    var RSA = {
        RFS: ctx.BIG.MODBYTES * ctx.FF.FFLEN,
        HASH_TYPE: 32,

        bytestohex: function(b) {
            var s = "",
                len = b.length,
                ch, i;

            for (i = 0; i < len; i++) {
                ch = b[i];
                s += ((ch >>> 4) & 15).toString(16);
                s += (ch & 15).toString(16);

            }
            return s;
        },

        bytestostring: function(b) {
            var s = "",
                i;

            for (i = 0; i < b.length; i++) {
                s += String.fromCharCode(b[i]);
            }

            return s;
        },

        stringtobytes: function(s) {
            var b = [],
                i;

            for (i = 0; i < s.length; i++) {
                b.push(s.charCodeAt(i));
            }

            return b;
        },

// Input private key from OpenSSL format
// e.g as in openssl rsa -in privkey.pem -noout -text
// Note order swap - For MIRACL c=1/p mod q, for OpenSSL c=1/q mod p
        PRIVATE_KEY_FROM_OPENSSL: function(P,Q,DP,DQ,C,PRIV) {
            ctx.FF.fromBytes(PRIV.p,Q);
            ctx.FF.fromBytes(PRIV.q,P);
            ctx.FF.fromBytes(PRIV.dp,DQ);
            ctx.FF.fromBytes(PRIV.dq,DP);
            ctx.FF.fromBytes(PRIV.c,C);
        },
        
        KEY_PAIR_FROM_OPENSSL: function(e,P,Q,DP,DQ,C,PRIV,PUB) {
            ctx.FF.fromBytes(PRIV.p,Q);
            ctx.FF.fromBytes(PRIV.q,P);
            ctx.FF.fromBytes(PRIV.dp,DQ);
            ctx.FF.fromBytes(PRIV.dq,DP);
            ctx.FF.fromBytes(PRIV.c,C);
            PUB.n = ctx.FF.mul(PRIV.p, PRIV.q);
            PUB.e = e;
        },

        KEY_PAIR: function(rng, e, PRIV, PUB) { /* IEEE1363 A16.11/A16.12 more or less */
            var n = PUB.n.length >> 1,
                t = new ctx.FF(n),
                p1 = new ctx.FF(n),
                q1 = new ctx.FF(n);

            for (;;) {
                PRIV.p.random(rng);

                while (PRIV.p.lastbits(2) != 3) {
                    PRIV.p.inc(1);
                }

                while (!ctx.FF.prime(PRIV.p, rng)) {
                    PRIV.p.inc(4);
                }

                p1.copy(PRIV.p);
                p1.dec(1);

                if (p1.cfactor(e)) {
                    continue;
                }

                break;
            }

            for (;;) {
                PRIV.q.random(rng);

                while (PRIV.q.lastbits(2) != 3) {
                    PRIV.q.inc(1);
                }

                while (!ctx.FF.prime(PRIV.q, rng)) {
                    PRIV.q.inc(4);
                }

                q1.copy(PRIV.q);
                q1.dec(1);

                if (q1.cfactor(e)) {
                    continue;
                }

                break;
            }

            PUB.n = ctx.FF.mul(PRIV.p, PRIV.q);
            PUB.e = e;

            t.copy(p1);
            t.shr();
            PRIV.dp.set(e);
            PRIV.dp.invmodp(t);
            if (PRIV.dp.parity() === 0) {
                PRIV.dp.add(t);
            }
            PRIV.dp.norm();

            t.copy(q1);
            t.shr();
            PRIV.dq.set(e);
            PRIV.dq.invmodp(t);
            if (PRIV.dq.parity() === 0) {
                PRIV.dq.add(t);
            }
            PRIV.dq.norm();

            PRIV.c.copy(PRIV.p);
            PRIV.c.invmodp(PRIV.q);

            return;
        },



        /* destroy the Private Key structure */
        PRIVATE_KEY_KILL: function(PRIV) {
            PRIV.p.zero();
            PRIV.q.zero();
            PRIV.dp.zero();
            PRIV.dq.zero();
            PRIV.c.zero();
        },

        /* RSA encryption with the public key */
        ENCRYPT: function(PUB, F, G) {
            var n = PUB.n.getlen(),
                f = new ctx.FF(n);

            ctx.FF.fromBytes(f, F);

            f.power(PUB.e, PUB.n);

            f.toBytes(G);
        },

        /* RSA decryption with the private key */
        DECRYPT: function(PRIV, G, F) {
            var n = PRIV.p.getlen(),
                g = new ctx.FF(2 * n),
                jp, jq, t;

            ctx.FF.fromBytes(g, G);

            jp = g.dmod(PRIV.p);
            jq = g.dmod(PRIV.q);

            jp.skpow(PRIV.dp, PRIV.p);
            jq.skpow(PRIV.dq, PRIV.q);

            g.zero();
            g.dscopy(jp);
            jp.mod(PRIV.q);
            if (ctx.FF.comp(jp, jq) > 0) {
                jq.add(PRIV.q);
            }
            jq.sub(jp);
            jq.norm();

            t = ctx.FF.mul(PRIV.c, jq);
            jq = t.dmod(PRIV.q);

            t = ctx.FF.mul(jq, PRIV.p);
            g.add(t);
            g.norm();

            g.toBytes(F);
        }
    };

    return RSA;
};

rsa_private_key = function(ctx) {
    "use strict";

    var rsa_private_key = function(n) {
        this.p = new ctx.FF(n);
        this.q = new ctx.FF(n);
        this.dp = new ctx.FF(n);
        this.dq = new ctx.FF(n);
        this.c = new ctx.FF(n);
    };

    rsa_private_key.prototype = {
        set: function(p, q, dp, dq, c) {
            this.p = p;
            this.q = q;
            this.dp = dp;
            this.dq = dq;
            this.c = c;
        },

        set_p: function(p) {
            this.p = p;
        },

        set_q: function (q) {
            this.q = q;
        },

        set_dp: function (dp) {
            this.dp = dp;
        },

        set_dq: function (dq) {
            this.dq = dq;
        },

        set_c: function (c) {
            this.c = c;
        }
    };

    rsa_private_key.get_instance = function(p, q, dp, dq, c) {
        let priv = new rsa_private_key(ctx.FF.HFLEN);
        priv.set(p, q, dp, dq, c);
        return priv;
    }

    return rsa_private_key;
};

rsa_public_key = function(ctx) {
    "use strict";

    var rsa_public_key = function(m) {
        this.e = 0;
        this.n = new ctx.FF(m);
    };

    rsa_public_key.prototype = {
        set: function(n, e) {
            this.e = e;
            ctx.FF.fromBytes(this.n, n);
        },

        set_e: function(e) {
            this.e = e;
        },

        set_n: function(n) {
            ctx.FF.fromBytes(this.n, n);
        }
    };

    rsa_public_key.get_instance = function(n, e) {
        var pub = new rsa_public_key(ctx.FF.FFLEN);
        pub.set(n,e);
        return pub;
    };

    return rsa_public_key;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports = {
        RSA: RSA,
        rsa_private_key: rsa_private_key,
        rsa_public_key: rsa_public_key
    };
}

