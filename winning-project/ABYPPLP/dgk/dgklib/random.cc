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
#include "random.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include <gmp.h>

void
dgk_get_random_num (mpz_t rob, unsigned long bits)
{
  struct timeval t;
  gettimeofday (&t, NULL);
  srandom (t.tv_sec * t.tv_usec);
  unsigned long int seed = random ();
  gmp_randstate_t r_state;
  gmp_randinit_default (r_state);
  gmp_randseed_ui (r_state, seed);
  mpz_urandomb (rob, r_state, bits);
}

void
dgk_get_random_number (mpz_t rob, mpz_t a, mpz_t b)
{
  size_t num_bits = 0;
  mpz_t ran, mod, t1;
  mpz_init (ran);
  mpz_init (mod);
  mpz_init (t1);

  mpz_sub (mod, b, a);

  num_bits = mpz_sizeinbase (b, 2);

  struct timeval t;
  gettimeofday (&t, NULL);
  srandom (t.tv_sec * t.tv_usec);
  unsigned long int seed = random ();
  gmp_randstate_t r_state;

  gmp_randinit_default (r_state);
  gmp_randseed_ui (r_state, seed);
  mpz_urandomb (ran, r_state, num_bits);

  mpz_mod (t1, ran, mod);
  mpz_add (rob, t1, a);

  mpz_clear (t1);
  mpz_clear (mod);
  mpz_clear (ran);
}

void
get_prime_number (mpz_t rob, unsigned int num_bits)
{
  mpz_t base, ran, tmp, two;
  struct timeval t;
  gettimeofday (&t, NULL);
  srandom (t.tv_sec * t.tv_usec);
  unsigned long int seed = random ();
  gmp_randstate_t r_state;

  mpz_init (base);
  mpz_init (ran);
  mpz_init (two);
  mpz_init (tmp);
  gmp_randinit_default (r_state);
  gmp_randseed_ui (r_state, seed);
  mpz_set_ui (two, 2);

  mpz_urandomb (ran, r_state, num_bits);
  mpz_pow_ui (base, two, num_bits);
  mpz_add (tmp, base, ran);
  mpz_nextprime (rob, tmp);

  gmp_randclear (r_state);
  mpz_clear (ran);
  mpz_clear (tmp);
  mpz_clear (base);
}
