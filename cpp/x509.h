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

/* CORE x509 header file */

/**
 * @file x509.h
 * @author Mike Scott and Kealan McCusker
 * @date 19th May 2015
 * @brief X509 function Header File
 *
 * defines structures
 * declares functions
 *
 */

#ifndef X509_H
#define X509_H

using namespace core;

/**
 * @brief Public key type
 */
typedef struct
{
    int type;  /**< signature type (ECC or RSA) */
    int hash;  /**< hash type */
    int curve; /**< elliptic curve used or RSA key length in bits  */
} pktype;


/* X.509 functions */
/** @brief Extract certificate signature
 *
	@param c an X.509 certificate
	@param s the extracted signature
	@return 0 on failure, or indicator of signature type (ECC or RSA)

*/
extern pktype X509_extract_cert_sig(octet *c, octet *s);
/** @brief
 *
	@param sc a signed certificate
	@param c the extracted certificate
	@return 0 on failure
*/
extern int X509_extract_cert(octet *sc, octet *c);
/** @brief
 *
	@param c an X.509 certificate
	@param k the extracted key
	@return 0 on failure, or indicator of public key type (ECC or RSA)
*/
extern pktype X509_extract_public_key(octet *c, octet *k);
/** @brief
 *
	@param c an X.509 certificate
	@return 0 on failure, or pointer to issuer field in cert
*/
extern int X509_find_issuer(octet *c);
/** @brief
 *
	@param c an X.509 certificate
	@return 0 on failure, or pointer to validity field in cert
*/
extern int X509_find_validity(octet *c);
/** @brief
 *
	@param c an X.509 certificate
	@return 0 on failure, or pointer to subject field in cert
*/
extern int X509_find_subject(octet *c);
/** @brief
 *
	@param c an X.509 certificate
	@param S is OID of property we are looking for
	@param s is a pointer to the section of interest in the cert
	@param f is pointer to the length of the property
	@return 0 on failure, or pointer to the property
*/
extern int X509_find_entity_property(octet *c, octet *S, int s, int *f);
/** @brief
 *
	@param c an X.509 certificate
	@param s is a pointer to the start of the validity field
	@return 0 on failure, or pointer to the start date
*/
extern int X509_find_start_date(octet *c, int s);
/** @brief
 *
	@param c an X.509 certificate
	@param s is a pointer to the start of the validity field
	@return 0 on failure, or pointer to the expiry date
*/
extern int X509_find_expiry_date(octet *c, int s);


#endif
