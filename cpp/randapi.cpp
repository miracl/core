#include "randapi.h"

using namespace amcl;

/* Initialise a Cryptographically Strong Random Number Generator from
   an octet of raw random data */

void amcl::CREATE_CSPRNG(csprng *RNG,octet *RAW)
{
    RAND_seed(RNG,RAW->len,RAW->val);
}

void amcl::KILL_CSPRNG(csprng *RNG)
{
    RAND_clean(RNG);
}

