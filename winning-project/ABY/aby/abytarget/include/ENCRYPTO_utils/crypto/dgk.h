/**
 \file 		dgk.h
 \author 	Daniel Demmler
 \copyright Copyright (C) 2019 ENCRYPTO Group, TU Darmstadt

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  ABY is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

 \brief		 libdgk - v0.9
 A library implementing the DGK crypto system with full decryption
 Thanks to Marina Blanton for sharing her Miracl DGK implementation from
 M. Blanton and P. Gasti, "Secure and efficient protocols for iris and fingerprint identification" (ESORICS’11)
 with us. We used it as a template for this GMP version.

 The implementation structure was inspired by
 libpailler - A library implementing the Paillier crypto system. (http://hms.isi.jhu.edu/acsc/libpaillier/)
 */

#ifndef _DGK_H_
#define _DGK_H_
#include <gmp.h>

/*
 This represents a DGK public key.
 */
struct dgk_pubkey_t {
	unsigned int bits; /* key bits e.g., 1024 */
	unsigned int lbits; /* share (message) length e.g., 32 */
	mpz_t n; /* public modulus n = pq */
	mpz_t u; /* u = 2^lbits (uses 2^(2lbits+2) internally) */
	mpz_t g; /* generator g */
	mpz_t h; /* generator h */
};

/*
 This represents a DGK private key; it needs to be used with a
 dgk_pubkey_t to be meaningful.
 */
struct dgk_prvkey_t {
	mpz_t vp;
	mpz_t vq;
	mpz_t p;
	mpz_t q;
	mpz_t p_minusone;
	mpz_t q_minusone;
	mpz_t pinv;
	mpz_t qinv;
};

extern mpz_t* powtwo;
extern mpz_t* gvpvqp;

/**
 * create a DGK key pair. This will take some time, depending on the size (up to several minutes!)
 * modulusbits is the size of the modulus n, e.g. 1024 or 2048 bit
 * lbits is equal to the share length, e.g. 16 or 32 bit (We use 2*lbits+2 internally)
 * the parameter t is internally fixed to 160, as recommended in the paper
 * NOTE: this key generation is probabilistic and might create fauly keys!
 *       Generated keys *MUST* be validated by running multiple trial encryption/decryptions and
 *       checking the result for correctnes!
 *       An example can be found in createKeys() in dgk.cpp:559
 */
void dgk_keygen(unsigned int modulusbits, unsigned int lbits, dgk_pubkey_t** pub, dgk_prvkey_t** prv);

/**
 * encrypt with public key only and double-base encryption - unfortunately not efficient due to different sized exponents, therefore deactivated
 */
//void dgk_encrypt_db(mpz_t res, dgk_pubkey_t* pub, mpz_t pt);
/**
 * encrypt with public key only, fixed-base encryption (must be initialized before first use!)
 */
void dgk_encrypt_fb(mpz_t res, dgk_pubkey_t* pub, mpz_t pt);

/**
 * encrypt with public key only, no further optimization (slower than fixed-base encryption)
 */
void dgk_encrypt_plain(mpz_t res, dgk_pubkey_t* pub, mpz_t pt);

/**
 * encrypt using CRT if we have the private key for efficiency
 */
void dgk_encrypt_crt(mpz_t res, dgk_pubkey_t* pub, dgk_prvkey_t* prv, mpz_t pt);

/**
 * use CRT and double base combined - unfortunately not efficient due to different sized exponents, therefore deactivated
 */
// void dgk_encrypt_crt_db(mpz_t res, dgk_pubkey_t* pub, dgk_prvkey_t* prv, mpz_t pt);
/**
 * DGK decryption
 */
void dgk_decrypt(mpz_t res, dgk_pubkey_t* pub, dgk_prvkey_t* prv, mpz_t ct);

/**
 * stores a generated key pair to disc
 */
void dgk_storekey(unsigned int modulusbits, unsigned int lbits, dgk_pubkey_t* pub, dgk_prvkey_t* prv);

/**
 * reads a previously stored key pair from disc
 */
void dgk_readkey(unsigned int modulusbits, unsigned int lbits, dgk_pubkey_t** pub, dgk_prvkey_t** prv);

/*
 These free the structures allocated and returned by various
 functions within library and should be used when the structures are
 no longer needed.
 */
void dgk_freepubkey(dgk_pubkey_t* pub);
void dgk_freeprvkey(dgk_prvkey_t* prv);

/**
 * create the full public key struct type given only n, g and h (e.g. after key exchange)
 */
void dgk_complete_pubkey(unsigned int modulusbits, unsigned int lbits, dgk_pubkey_t** pub, mpz_t n, mpz_t g, mpz_t h);

/**
 * -------------------------------
 * the following are internal functions, that should not be called from the outside unless you really know what they do, hence commented out
 * -------------------------------
 */

/**
 * create a batch of different keys and check that they are valid, overwrites existing keys (if any)
 */
// void createKeys(){
/**
 * test correct encrypt/decrypt
 */
//void test_encdec()

/**
 * test correct sharing
 */
//void test_sharing(){
#endif
