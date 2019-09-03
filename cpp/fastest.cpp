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
#include <stdio.h>
#include "fp_YYY.h"

#ifdef COMBA

int main()
{
    int i, j, k, N;

    N = NLEN_XXX;

    printf("Insert this code in file fp_YYY.cpp\n\n");

    printf("void YYY::FP_modmul(BIG r,BIG a,BIG b)\n");

    printf("{\n");
    printf("\tdchunk t,c,s;\n");
    printf("\tdchunk d[%d],dd[%d];\n", N, N);
    printf("\tchunk v[%d],md[%d];\n", N, N);
    printf("\tchunk MC=MConst;\n");

    printf("\tBIG_rcopy(md,Modulus);\n");

    for (i = 0; i < N; i++)
        printf("\td[%d]=(dchunk)a[%d]*b[%d];\n", i, i, i);

    printf("\n\ts=d[0];  t=s; v[0]=((chunk)t*MC)&BMASK_XXX; t+=(dchunk)v[0]*md[0]; c=(t>>BASEBITS_XXX);\n\n");

    for (k = 1; k < N; k++)
    {
        printf("\ts+=d[%d]; t=s ", k);
        for (i = k; i >= 1 + k / 2; i--)
            printf("+(dchunk)(a[%d]-a[%d])*(b[%d]-b[%d])", i, k - i, k - i, i);
        printf("; t+=c+(dchunk)v[0]*md[%d]", k);
        for (i = k - 1; i > k / 2; i--) printf("+(dchunk)(v[%d]-v[%d])*(md[%d]-md[%d])", k - i, i, i, k - i);
        printf("; v[%d]=((chunk)t*MC)&BMASK_XXX; t+=(dchunk)v[%d]*md[0]; ", k, k);
        printf(" dd[%d]=(dchunk)v[%d]*md[%d]; s+=dd[%d]; c=(t>>BASEBITS_XXX); \n", k, k, k, k);
    }
    printf("\n");
    for (k = N; k < 2 * N - 1; k++)
    {
        printf("\ts-=d[%d]; t=s ", k - N);
        for (i = N - 1; i >= 1 + k / 2; i--)
            printf("+(dchunk)(a[%d]-a[%d])*(b[%d]-b[%d])", i, k - i, k - i, i);
        printf("; t+=c");
        for (i = N - 1; i >= 1 + k / 2; i--) printf("+(dchunk)(v[%d]-v[%d])*(md[%d]-md[%d])", k - i, i, i, k - i);
        printf("; r[%d]=(chunk)t&BMASK_XXX;  s-=dd[%d]; c=(t>>BASEBITS_XXX); \n", k - N, k - N + 1);
    }

    printf("\tr[%d]=(chunk)c&BMASK_XXX;\n", N - 1);

    printf("}\n");

}

#endif


