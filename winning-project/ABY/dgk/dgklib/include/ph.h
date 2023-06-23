#include <assert.h>
#include <gmp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vec_
{
  mpz_t *el;
  int count;
} vec_t;

typedef struct bsgs_
{
  mpz_t modulus;
  mpz_t base;
  unsigned long order;
  unsigned long sqrt_order;
  mpz_t a_minus_m;
  vec_t bs_vals;
} bsgs_t;

typedef struct ph_
{
  mpz_t modulus;
  mpz_t generator;
  vec_t subgroup_generators;
  vec_t mpi;
  vec_t mpi_inv;
  bsgs_t *subgroup_bsgs;
  unsigned long *primes;
  unsigned n_primes;
} ph_t;

/***********************************************/

void init_vec (vec_t *v, int count);

void free_vec (vec_t *v);

void pp_vec (vec_t *v);

int cmp_vec (vec_t *a, vec_t *b);

int cmp_vec_ui (vec_t *a, unsigned long *b, int count);

/***********************************************/

void init_bsgs (bsgs_t *b, const mpz_t base, const mpz_t modulus,
                const unsigned long order);

void free_bsgs (bsgs_t *b);

int compute_dlog_bsgs (bsgs_t *b, unsigned long *rop, const mpz_t value);

void print_bsgs_t (bsgs_t *b);

void fprint_bsgs_t (FILE *f, bsgs_t *b);

/***********************************************/

unsigned lazy_sieve (unsigned n, unsigned long *primes);

/***********************************************/

void init_ph (ph_t *rop, const mpz_t generator,
              const unsigned primorial_order);

void free_ph (ph_t *op);

void compute_subgroup_dlogs (unsigned long *rop, ph_t *op, mpz_t value);

void compute_dlog_prime_power (unsigned long *rop, unsigned long p,
                               unsigned long e, bsgs_t *bsgs, mpz_t generator,
                               mpz_t modulus, mpz_t value);

void solve_congruences (mpz_t rop, ph_t *op, unsigned long *x_i);

void compute_dlog (mpz_t result, ph_t *ph, mpz_t value);

void print_ph (ph_t *op);

/***********************************************/
////Test functions
int test_sieve_basic (int verbose);
int test_sieve_return_length (int verbose);

int test_bsgs_basic (int verbose);
int test_bsgs_small (int verbose);

int test_ph_basic (int verbose);
int test_ph_crt (int verbose);
int test_ph_prime_power (int verbose);
int test_ph_dlog_small (int verbose);