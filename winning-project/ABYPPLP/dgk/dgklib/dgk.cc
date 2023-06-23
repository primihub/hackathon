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

#include "dgk.h"
#include "dgk_keygen.h"

void
dgk_encrypt (mpz_t rop, mpz_t m, mpz_t g, mpz_t n, mpz_t h, mpz_t r)
{
  mpz_t t1, t2;
  mpz_t t3;
  mpz_init (t3);
  mpz_init (t1);
  mpz_init (t2);

  mpz_powm (t1, g, m, n);
  mpz_powm (t2, h, r, n);
  mpz_mul (t3, t1, t2);
  mpz_mod (rop, t3, n);

  mpz_clear (t1);
  mpz_clear (t2);
  mpz_clear (t3);

  return;
} /* -----  end of function dgk_encrypt  ----- */

void
dgk_decrypt (mpz_t rop, mpz_t c, mpz_t v, mpz_t u, mpz_t n, mpz_t *atb)
{
  mpz_t cv;
  mpz_init (cv);
  int ui = mpz_get_si (u);
  int i = 0;

  mpz_powm (cv, c, v, n);

  for (; i < ui; i++)
    {
      if (atb[i]->_mp_d[0] == cv->_mp_d[0])
        {
          if (mpz_cmp (atb[i], cv) == 0)
            {
              mpz_set_ui (rop, i);
              return;
            }
        }
    }
}
