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

#include <gmp.h>
#include <stdlib.h>

#include "dgk_keygen.h"
#include "gdsa.h"
#include "maurer.h"
#include "random.h"

void dgk_gen_keys(dgk_priv_t *priv, dgk_pub_t *pub, unsigned int k,
                  unsigned int t, unsigned int l) {
  mpz_t uvp, uvq;
  mpz_t rp, rq;
  mpz_init(uvp);
  mpz_init(uvq);
  mpz_init(rp);
  mpz_init(rq);
  dgk_keys_init(priv, pub);
  mpz_init(uvp);
  mpz_init(uvq);

  // random t-bits prime
  maurer(priv->vp, t);
  maurer(priv->vq, t);
  mpz_mul(priv->vpq, priv->vp, priv->vq);

  // random l-bits prime
  maurer(priv->u, l);
  mpz_set(pub->u, priv->u);

  mpz_mul(uvp, pub->u, priv->vp);
  mpz_mul(uvq, pub->u, priv->vq);

  // find appropriate (k/2)-bits p, q
  gdsa_prime(priv->p, uvp, k / 2);
  gdsa_prime(priv->q, uvq, k / 2);

  mpz_mul(priv->n, priv->p, priv->q);
  mpz_set(pub->n, priv->n);

  mpz_mul(uvp, pub->u, priv->vp);
  mpz_mul(uvq, pub->u, priv->vq);

  // efficiently minus one
  priv->p->_mp_d[0] = priv->p->_mp_d[0] - 1;
  priv->q->_mp_d[0] = priv->q->_mp_d[0] - 1;
  mpz_divexact(rp, priv->p, uvp);
  mpz_divexact(rq, priv->q, uvq);
  priv->p->_mp_d[0] = priv->p->_mp_d[0] + 1;
  priv->q->_mp_d[0] = priv->q->_mp_d[0] + 1;

  find_elm_ord_v(pub->h, pub->u, rp, rq, pub->n);
  find_elm_ord_vu(pub->g, pub->u, priv->vp, priv->vq, rp, rq, pub->n);

  mpz_set(priv->g, pub->g);

  dgk_init_table(priv);

  return;
}

void

dgk_keys_init (dgk_priv_t *priv, dgk_pub_t *pub)
{
  mpz_init(priv->n);
  mpz_init(priv->g);
  mpz_init(priv->u);
  mpz_init(priv->p);
  mpz_init(priv->q);
  mpz_init(priv->vp);
  mpz_init(priv->vq);
  mpz_init(priv->vpq);

  mpz_init(pub->n);
  mpz_init(pub->g);
  mpz_init(pub->h);
  mpz_init(pub->u);

  return;
}

void dgk_keys_init_priv(dgk_priv_t *priv) {
  mpz_init(priv->n);
  mpz_init(priv->g);
  mpz_init(priv->u);
  mpz_init(priv->p);
  mpz_init(priv->q);
  mpz_init(priv->vp);
  mpz_init(priv->vq);
  mpz_init(priv->vpq);

  return;
}

void dgk_keys_init_pub(dgk_pub_t *pub) {
  mpz_init(pub->n);
  mpz_init(pub->g);
  mpz_init(pub->h);
  mpz_init(pub->u);

  return;
}

void dgk_init_table(dgk_priv_t *priv) {
  mpz_t gv, vpq;
  mpz_init(gv);
  mpz_init(vpq);

  int u = mpz_get_si(priv->u);
  int i = 0;

  mpz_mul(vpq, priv->vp, priv->vq);
  mpz_powm(gv, priv->g, vpq, priv->n);

  priv->rtab = (mpz_t *)malloc(sizeof(mpz_t) * u);

  mpz_array_init((priv->rtab[0]), u, mpz_sizeinbase(priv->n, 2));

  for (; i < u; i++) {
    mpz_powm_ui(priv->rtab[i], gv, i, priv->n);
  }
  return;
}

void find_elm_ord_v(mpz_t rop, mpz_t u, mpz_t rp, mpz_t rq, mpz_t n) {
  mpz_set_ui(rop, 0);
  mpz_t e;
  mpz_init(e);
  mpz_t r;
  mpz_init(r);
  mpz_t cd;
  mpz_init(cd);
  mpz_t sum;
  mpz_init(sum);

  /*  exponent e = rp*rq*u */
  mpz_mul(e, rp, rq);
  mpz_mul(e, e, u);

  do {
    dgk_get_random_num(r, mpz_sizeinbase(n, 2) - 2);
    mpz_powm(rop, r, e, n);
    mpz_gcd(cd, rop, n);
  } while (mpz_cmp_ui(rop, 1) == 0 || mpz_cmp_ui(cd, 1) != 0);

  return;
}

void find_elm_ord_vu(mpz_t rop, mpz_t u, mpz_t vp, mpz_t vq, mpz_t rp, mpz_t rq,
                     mpz_t n) {
  mpz_t e;
  mpz_init(e);
  mpz_t r;
  mpz_init(r);
  mpz_t cd;
  mpz_init(cd);
  mpz_t r1, r2, r3;
  mpz_t sum;
  mpz_init(sum);
  mpz_init(r1);
  mpz_init(r2);
  mpz_init(r3);

  /*  exponent e = rp*rq*u */
  mpz_mul(e, rp, rq);

  do {
    dgk_get_random_num(r, mpz_sizeinbase(n, 2) - 2);
    mpz_powm(rop, r, e, n);
    mpz_gcd(cd, rop, n);

    mpz_powm(r1, rop, u, n);
    mpz_powm(r2, rop, vp, n);
    mpz_powm(r3, rop, vq, n);
  } while (mpz_cmp_ui(rop, 1) == 0 || mpz_cmp_ui(r1, 1) == 0 ||
           mpz_cmp_ui(r2, 2) == 0 || mpz_cmp_ui(r3, 3) == 0 ||
           mpz_cmp_ui(cd, 1) != 0);

  return;
}
