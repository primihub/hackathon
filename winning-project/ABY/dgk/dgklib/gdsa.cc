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

#include "gdsa.h"
#include "maurer.h"
#include "random.h"
#include <gmp.h>

/* Fuction returns a random number which is invertible
 */
void
get_invertible_num (mpz_t rob, mpz_t mod)
{ // gen a random num coprime mod
  mpz_t t1, one;
  mpz_init (t1);

  mpz_init_set_ui (one, 1);

  do
    {
      dgk_get_random_number (rob, one, mod);
      mpz_gcd (t1, rob, mod);
    }
  while (mpz_cmp_ui (t1, 1) != 0);

  return;
}

void
gdsa_prime (mpz_t rob, mpz_t q, unsigned int numbits)
{
  mpz_t a, c;
  mpz_t q_inv, q_min; // q_min is -q mod P
  mpz_t pprod;
  mpz_t r, t1, t2;
  mpz_init (a);
  mpz_init (c);
  mpz_init (pprod);
  mpz_init (q_inv);
  mpz_init (r);
  mpz_init (t1);
  mpz_init (t2);
  mpz_init (q_min);

  int i = 0;
  // bitlen
  int q_size = mpz_sizeinbase (q, 2);
  prime_prod (pprod, numbits - q_size);

  // calculate prime product
  mpz_invert (q_inv, q, pprod);
  mpz_sub (q_min, pprod, q_inv);

  get_invertible_num (a, pprod);
  get_invertible_num (c, pprod);

  do
    {
      mpz_add (t1, q_min, c);
      mpz_mod (r, t1, pprod);
      mpz_mul (t2, r, q);
      mpz_add_ui (rob, t2, 1);

      // calc new c
      mpz_mul (t1, a, c);
      mpz_mod (c, t1, pprod);
      i++;
    }
  while (mpz_probab_prime_p (rob, 10) < 1);

  return;
}
