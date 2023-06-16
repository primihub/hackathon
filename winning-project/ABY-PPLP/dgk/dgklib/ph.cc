#include "ph.h"

void
init_ph (ph_t *rop, const mpz_t generator, const unsigned primorial_order)
{
  rop->primes = malloc (primorial_order * sizeof (unsigned long));
  assert (lazy_sieve (primorial_order, rop->primes) == primorial_order);
  rop->n_primes = primorial_order;

  // Modulus = \prod p_i
  mpz_init_set_ui (rop->modulus, rop->primes[0]);
  for (unsigned i = 1; i < primorial_order; i++)
    {
      mpz_mul_ui (rop->modulus, rop->modulus, rop->primes[i]);
    }

  mpz_init_set (rop->generator, generator);

  init_vec (&(rop->subgroup_generators), rop->n_primes);
  init_vec (&(rop->mpi), rop->n_primes);

  // mpi = M / p_i
  for (unsigned i = 0; i < rop->n_primes; i++)
    {
      mpz_tdiv_q_ui (rop->mpi.el[i], rop->modulus, rop->primes[i]);
    }

  // precompute mpi^{-1} mod p_i for CRT
  init_vec (&(rop->mpi_inv), rop->n_primes);
  mpz_t p;
  mpz_init (p);
  for (unsigned i = 0; i < rop->n_primes; i++)
    {
      mpz_set_ui (p, rop->primes[i]);
      mpz_invert (rop->mpi_inv.el[i], rop->mpi.el[i], p);
    }

  // compute g_i = g^(M/p_i)
  for (unsigned i = 0; i < rop->n_primes; i++)
    {
      mpz_powm (rop->subgroup_generators.el[i], generator, rop->mpi.el[i],
                rop->modulus);
    }

  // setup bsgs algorithm for each subgroup
  rop->subgroup_bsgs = malloc (rop->n_primes * sizeof (bsgs_t));
  for (unsigned i = 0; i < rop->n_primes; i++)
    {
      init_bsgs (&(rop->subgroup_bsgs[i]), rop->subgroup_generators.el[i],
                 rop->modulus, rop->primes[i]);
    }
}

void
free_ph (ph_t *op)
{
  mpz_clear (op->modulus);
  mpz_clear (op->generator);
  free_vec (&(op->subgroup_generators));
  free_vec (&(op->mpi));
  free_vec (&(op->mpi_inv));

  for (unsigned i = 0; i < op->n_primes; i++)
    free_bsgs (&(op->subgroup_bsgs[i]));

  free (op->primes);
  memset (op, 0, sizeof (ph_t));
}

// Assumes rop and op are properly initialized
void
compute_subgroup_dlogs (unsigned long *rop, ph_t *op, mpz_t value)
{
  vec_t vals;
  init_vec (&vals, op->n_primes);

  // h_i = h^{M/p_i} mod M
  for (unsigned i = 0; i < op->n_primes; i++)
    {
      mpz_powm (vals.el[i], value, op->mpi.el[i], op->modulus);
    }

  // solve g_i^{x_i} = h_i mod M
  for (unsigned i = 0; i < op->n_primes; i++)
    {
      compute_dlog_bsgs (&(op->subgroup_bsgs[i]), &(rop[i]), vals.el[i]);

      gmp_printf ("%Zd^%lu = %Zd\n", op->subgroup_generators.el[i], rop[i],
                  vals.el[i]);
    }

  free_vec (&vals);
}

// g^x = h mod N, where |<g>| = p^e
//  <g> is a subgroup of order p^e where p is prime
//  and e \geq 1
void
compute_dlog_prime_power (unsigned long *rop, unsigned long p, unsigned long e,
                          bsgs_t *bsgs, // base = gamma = g^p^(e-1)
                          mpz_t generator, mpz_t modulus, mpz_t value)
{
  // Validate that we have the right bsgs struct
  assert (p == bsgs->order);

  // Must have \gamma = g^p^(e-1) as base
  mpz_t test;
  mpz_init (test);
  mpz_powm_ui (test, generator, p, modulus);
  mpz_powm_ui (test, test, e - 1, modulus);
  assert (!mpz_cmp (test, bsgs->base));
  mpz_clear (test);

  mpz_t g_inv;
  mpz_init (g_inv);
  mpz_invert (g_inv, generator, modulus);

  unsigned long last_x = 0;
  unsigned long x = 0;

  mpz_t h;
  mpz_init (h);
  unsigned long pi = 1;
  unsigned long pe = 1;
  for (unsigned i = 0; i < e - 1; i++)
    pe *= 3;
  *rop = 0;

  for (unsigned i = 0; i < e; i++)
    {
      mpz_powm_ui (h, g_inv, last_x, modulus); // h[i] = g^{-x[i])
      mpz_mul (h, h, value);                   // h[i] = h * g^(-x[i])
      mpz_powm_ui (h, h, pe, modulus);         // h[i] = (h[i])^p

      compute_dlog_bsgs (bsgs, &x, h);

      *rop += x * pi;
      pi *= 3;
      pe /= 3;
      last_x = x;
    }

  mpz_clear (g_inv);
  mpz_clear (h);
}

// Assumes rop is initialized
void
solve_congruences (mpz_t rop, ph_t *op, unsigned long *x_i)
{
  mpz_t tmp;
  mpz_init (tmp);
  mpz_set_ui (rop, 0);

  // sum x_i * mpi * mpi_inv
  for (unsigned i = 0; i < op->n_primes; i++)
    {
      mpz_mul (tmp, op->mpi.el[i], op->mpi_inv.el[i]);
      mpz_mul_ui (tmp, tmp, x_i[i]);
      mpz_mod (tmp, tmp, op->modulus);
      mpz_add (rop, rop, tmp);
    }

  mpz_mod (rop, rop, op->modulus);
}

// Everything must be initialized
void
compute_dlog (mpz_t result, ph_t *ph, mpz_t value)
{
  unsigned long *subgroup_x = malloc (ph->n_primes * sizeof (unsigned long));
  compute_subgroup_dlogs (subgroup_x, ph, value);
  solve_congruences (result, ph, subgroup_x);
}

/****************************************************************************/

void
print_ph (ph_t *op)
{
  printf ("-----------ph_t----------\n");
  gmp_printf ("Modulus:\t\t%Zd\n", op->modulus);
  gmp_printf ("Generator:\t\t%Zd\n", op->generator);
  printf ("Subgroup generators:\n");
  pp_vec (&(op->subgroup_generators));
  printf ("Mpi:\n");
  pp_vec (&(op->mpi));
  printf ("mpi_inv:\n");
  pp_vec (&(op->mpi_inv));

  printf ("\nbsgs_t's:\n");
  for (unsigned i = 0; i < op->n_primes; i++)
    {
      printf ("(%u, %lu):\n", i, op->primes[i]);
      print_bsgs_t (&(op->subgroup_bsgs[i]));
      printf ("\n");
    }
}

/****************************************************************************/

int
test_ph_basic (int verbose)
{
  int ret_val = 0;

  unsigned long order = 4;

  unsigned long exp_sg[] = { 153, 39, 99, 99 };
  unsigned long exp_mpi[] = { 105, 70, 42, 30 };
  unsigned long exp_mpi_inv[] = { 1, 1, 3, 4 };

  mpz_t generator;
  mpz_init_set_ui (generator, 3);

  ph_t ph;

  init_ph (&ph, generator, order);

  if (verbose)
    print_ph (&ph);

  if (mpz_cmp_ui (ph.modulus, 210))
    ret_val = 1;

  if (cmp_vec_ui (&ph.subgroup_generators, exp_sg, 4))
    ret_val = 2;

  if (cmp_vec_ui (&ph.mpi, exp_mpi, 4))
    ret_val = 3;

  if (cmp_vec_ui (&ph.mpi_inv, exp_mpi_inv, 4))
    ret_val = 4;

  free_ph (&ph);

  return ret_val;
}

int
test_ph_crt (int verbose)
{
  int ret_val = 0;

  unsigned long order = 4;

  unsigned long xi[] = { 1, 0, 2, 1 };

  mpz_t generator;
  mpz_init_set_ui (generator, 3);

  ph_t ph;

  init_ph (&ph, generator, order);

  mpz_t res;
  mpz_init (res);

  solve_congruences (res, &ph, xi);

  if (mpz_cmp_ui (res, 57))
    {
      if (verbose)
        gmp_printf ("Expected 57, got %Zd\n", res);
      ret_val = 1;
    }

  free_ph (&ph);

  return ret_val;
}

int
test_ph_prime_power (int verbose)
{
  //<16> \in Z_37 has order 3^2

  unsigned long result;
  mpz_t generator;
  mpz_init_set_ui (generator, 16);
  mpz_t modulus;
  mpz_init_set_ui (modulus, 37);
  mpz_t value;
  mpz_init_set_ui (value, 12);

  mpz_t bsgs_base;
  mpz_init (bsgs_base);
  mpz_powm_ui (bsgs_base, generator, 3, modulus);

  bsgs_t bsgs;
  init_bsgs (&bsgs, bsgs_base, modulus, 3);

  compute_dlog_prime_power (&result, 3, 2, &bsgs, generator, modulus, value);

  free_bsgs (&bsgs);
  mpz_clear (generator);
  mpz_clear (modulus);
  mpz_clear (value);
  mpz_clear (bsgs_base);

  if (result == 7)
    return 0;
  else
    {
      if (verbose)
        printf ("Expected 7, got %lu\n", result);
      return 1;
    }
}

int
test_ph_dlog_small (int verbose)
{
  int ret_val = 0;

  unsigned long order = 4;

  mpz_t generator;
  mpz_init_set_ui (generator, 3);

  ph_t ph;

  init_ph (&ph, generator, order);

  mpz_t res;
  mpz_t value;
  mpz_init (res);
  mpz_init_set_ui (value, 117);

  compute_dlog (res, &ph, value);

  if (mpz_cmp_ui (res, 11))
    {
      if (verbose)
        gmp_printf ("Expected 11, got %Zd\n", res);
      ret_val = 1;
    }

  free_ph (&ph);

  return ret_val;
}