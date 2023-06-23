/*   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *
 *
 * Copyright 2017, Marc X. Makkes (_@makkes.mx)
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */
#ifndef __DGK_KEYGEN_H__
#define __DGK_KEYGEN_H__

typedef struct _dgk_pub
{
  mpz_t n;
  mpz_t g;
  mpz_t h;
  mpz_t u;
  unsigned int t;
} dgk_pub_t;

typedef struct _dgk_priv
{
  mpz_t n;
  mpz_t g;
  mpz_t u;
  mpz_t p;
  mpz_t q;
  mpz_t vp;
  mpz_t vq;
  mpz_t vpq;
  mpz_t *rtab;
} dgk_priv_t;

void find_elm_ord_v (mpz_t rop, mpz_t v, mpz_t p, mpz_t q, mpz_t n);
void find_elm_ord_vu (mpz_t rop, mpz_t u, mpz_t vp, mpz_t vq, mpz_t rp,
                      mpz_t rq, mpz_t n);

void dgk_gen_keys (dgk_priv_t *priv, dgk_pub_t *pub, unsigned int k,
                   unsigned int t, unsigned int l);
void dgk_keys_init (dgk_priv_t *priv, dgk_pub_t *pub);
void dgk_init_table (dgk_priv_t *priv);

void dgk_keys_init_priv (dgk_priv_t *priv);
void dgk_keys_init_pub (dgk_pub_t *pub);

#endif
