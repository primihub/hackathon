#include "bf.h"
#include "dgk.h"
#include "dgk_keygen.h"
#include "random.h"
#include <chrono>
#include <iostream>

#include "csvwriter.h"

using namespace std;

// get the diff of time
auto get_diff = [](std::chrono::system_clock::time_point t1,
                   std::chrono::system_clock::time_point t0) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
};

int dgk_example() {
  mpz_t vp, vq, p, q;
  mpz_t r, rp, rq, uvp, uvq;
  mpz_t n, g, h, u;
  mpz_inits(vp, vq, p, q, n, g, h, u, NULL);
  mpz_inits(r, rp, rq, uvp, uvq, NULL);
  dgk_pub_t *pub = (dgk_pub_t *)malloc(sizeof(dgk_pub_t));
  dgk_priv_t *priv = (dgk_priv_t *)malloc(sizeof(dgk_priv_t));

  /*                         k   t    l */
  dgk_gen_keys(priv, pub, 2048, 320, 16);

  gmp_printf("q : %Zd\n", priv->q);
  gmp_printf("p : %Zd\n", priv->p);
  gmp_printf("u : %Zd\n", pub->u);
  gmp_printf("vp: %Zd\n", priv->vp);
  gmp_printf("vq: %Zd\n", priv->vq);
  gmp_printf("g : %Zd\n", pub->g);
  gmp_printf("h : %Zd\n", pub->h);

  mpz_t m, vpq, c;
  mpz_init_set_ui(m, 3373);
  mpz_init(vpq);
  mpz_init(c);

  mpz_mul(vpq, priv->vp, priv->vq);
  dgk_get_random_num(r, 320 * 2.5);

  dgk_encrypt(c, m, pub->g, pub->n, pub->h, r);
  gmp_printf("c %Zd\n", c);

  mpz_t bm;
  mpz_init(bm);
  dgk_decrypt(bm, c, priv->vpq, priv->u, priv->n, priv->rtab);
  gmp_printf("m %Zd\n", bm);
  return EXIT_SUCCESS;
} /* ----------  end of function dgk_example  ---------- */

int pppt(const char *filename, uint64_t radius, int makehead) {
  uint64_t xa = 123123;
  uint64_t ya = 123456;
  uint64_t xb = 123321;
  uint64_t yb = 123654;
  uint64_t sq_radius = radius * radius;

  cout << "radius(threshold):\t" << radius << endl;
  mpz_t tmp1, tmp2;
  mpz_inits(tmp1, tmp2, NULL);

  cout << "A's horizontal coordinates:\t" << xa << endl;
  cout << "A's vertical coordinates:\t" << ya << endl;
  cout << "B's horizontal coordinates:\t" << xb << endl;
  cout << "B's vertical coordinates:\t" << yb << endl;

  // auto t_begin = chrono::high_resolution_clock::now();

  auto t_AkGen0 = chrono::high_resolution_clock::now();

  // A -- KGen
  dgk_pub_t *pub = (dgk_pub_t *)malloc(sizeof(dgk_pub_t));
  dgk_priv_t *priv = (dgk_priv_t *)malloc(sizeof(dgk_priv_t));
  int k = 2048, t = 320, l = 16;
  dgk_gen_keys(priv, pub, k, t, l);

  auto t_AkGen1 = chrono::high_resolution_clock::now();

  // gmp_printf ("q : %Zd\n", priv->q);
  // gmp_printf ("p : %Zd\n", priv->p);
  // gmp_printf ("u : %Zd\n", pub->u);
  // gmp_printf ("vp: %Zd\n", priv->vp);
  // gmp_printf ("vq: %Zd\n", priv->vq);
  // gmp_printf ("g : %Zd\n", pub->g);
  // gmp_printf ("h : %Zd\n", pub->h);

  auto t_ApreCalc0 = chrono::high_resolution_clock::now();

  // A -- precomputation
  mpz_t r1, r2, r3;
  mpz_t t1, t2, t3;
  mpz_inits(r1, r2, r3, t1, t2, t3, NULL);
  dgk_get_random_num(r1, 2.5 * t);
  dgk_get_random_num(r2, 2.5 * t);
  dgk_get_random_num(r3, 2.5 * t);
  mpz_powm(t1, pub->h, r1, pub->n);
  mpz_powm(t2, pub->h, r2, pub->n);
  mpz_powm(t3, pub->h, r3, pub->n);

  auto t_ApreCalc1 = chrono::high_resolution_clock::now();

  auto t_BsetBF0 = chrono::high_resolution_clock::now();

  // B -- initialize bloom filter
  bloom_parameters bf_parms;
  bf_parms.projected_element_count = sq_radius;
  bf_parms.false_positive_probability = 0.0001; // 1 in 10000
  bf_parms.random_seed = 0xA5A5A5A5;
  bf_parms.compute_optimal_parameters();
  bloom_filter bf(bf_parms);

  mpz_t r, s, w, cr; // rr,
  mpz_inits(r, s, w, cr, NULL);
  dgk_get_random_num(r, l);
  dgk_get_random_num(s, l);
  dgk_get_random_num(w, l);
  // dgk_get_random_num(rr, 2.5 * t);

  for (uint64_t di = 0; di < sq_radius; ++di) { // s(r+di)
    mpz_add_ui(tmp1, r, di);
    mpz_mul(tmp2, tmp1, s);
    mpz_mod(tmp2, tmp2, pub->u);
    // gmp_printf ("%Zd ", tmp2);
    mpz_mul_2exp(tmp1, tmp2, l);
    mpz_ior(tmp2, tmp1, w);
    bf.insert(mpz_get_ui(tmp2));
    // cout << mpz_get_ui (tmp2) << ' ';
  }

  auto t_BsetBF1 = chrono::high_resolution_clock::now();

  auto t_BencCr0 = chrono::high_resolution_clock::now();

  mpz_mul(tmp1, r, s);
  dgk_encrypt(cr, tmp1, pub->g, pub->n, pub->h, r);

  auto t_BencCr1 = chrono::high_resolution_clock::now();

  // cout << endl;
  // A

  auto t_Aenc0 = chrono::high_resolution_clock::now();

  uint64_t uu = xa * xa + ya * ya;
  mpz_t c1, c2, c3;
  mpz_inits(c1, c2, c3, NULL);
  mpz_powm_ui(c1, pub->g, uu, pub->n);
  mpz_powm_ui(c2, pub->g, xa << 1, pub->n);
  mpz_powm_ui(c3, pub->g, ya << 1, pub->n);
  mpz_invert(c2, c2, pub->n);
  mpz_invert(c3, c3, pub->n);

  mpz_mul(c1, c1, t1);
  mpz_mul(c2, c2, t2);
  mpz_mul(c3, c3, t3);

  auto t_Aenc1 = chrono::high_resolution_clock::now();

  // gmp_printf ("c1: %Zd\n", c1);
  // gmp_printf ("c2: %Zd\n", c2);
  // gmp_printf ("c3: %Zd\n", c3);

  // B
  auto t_BencCz0 = chrono::high_resolution_clock::now();

  uint64_t z = xb * xb + yb * yb;
  mpz_t cz;
  mpz_init_set_ui(cz, z);
  mpz_mul(cz, cz, s);
  dgk_get_random_num(r, l);
  dgk_encrypt(cz, cz, pub->g, pub->n, pub->h, r);

  auto t_BencCz1 = chrono::high_resolution_clock::now();

  auto t_BhomoCalc0 = chrono::high_resolution_clock::now();

  mpz_powm_ui(c2, c2, xb, pub->n);
  mpz_powm_ui(c3, c3, yb, pub->n);
  mpz_mul(c2, c2, c3);
  mpz_mul(c1, c1, c2);
  mpz_powm(c1, c1, s, pub->n);
  mpz_mul(c1, c1, cz);
  mpz_mul(c1, c1, cr);

  auto t_BhomoCalc1 = chrono::high_resolution_clock::now();

  auto t_Adec0 = chrono::high_resolution_clock::now();

  // pplp_printf("Time measured: %.3f seconds.\n\n", elapsed.count() * 1e-9);

  // A
  mpz_t bd;
  mpz_init(bd);
  dgk_decrypt(bd, c1, priv->vpq, priv->u, priv->n, priv->rtab);
  // gmp_printf ("blind distance:\t%Zd\n", bd);
  mpz_mul_2exp(bd, bd, l);
  mpz_ior(bd, bd, w);
  // gmp_printf ("%Zd\n", bd);

  int isNear = bf.contains(mpz_get_ui(bd));

  auto t_Adec1 = chrono::high_resolution_clock::now();
  // auto end = chrono::high_resolution_clock::now();

  double d_AkGen = get_diff(t_AkGen1, t_AkGen0);
  double d_ApreClac = get_diff(t_ApreCalc1, t_ApreCalc0);
  double d_Aenc = get_diff(t_Aenc1, t_Aenc0);
  double d_Adec = get_diff(t_Adec1, t_Adec0);
  double d_BsetBF = get_diff(t_BsetBF1, t_BsetBF0);
  double d_BencCr = get_diff(t_BencCr1, t_BencCr0);
  double d_BencCz = get_diff(t_BencCz1, t_BencCz0);
  double d_BhomoCalc = get_diff(t_BhomoCalc1, t_BhomoCalc0);

  double d_A1 = d_AkGen;
  double d_A2 = d_ApreClac;
  double d_A3 = d_Aenc + d_Adec;
  double d_B1 = 0.00;
  double d_B2 = d_BsetBF + d_BencCr;
  double d_B3 = d_BencCz + d_BhomoCalc;
  double d_Atotal = d_A1 + d_A2 + d_A3;
  double d_Btotal = d_B1 + d_B2 + d_B3;

  cout << "d_AkGen\t\t\t" << d_AkGen * 1e-6 << "ms" << endl;
  cout << "d_ApreClac\t\t" << d_ApreClac * 1e-6 << "ms" << endl;
  cout << "d_Aenc\t\t\t" << d_Aenc * 1e-6 << "ms" << endl;
  cout << "d_Adec\t\t\t" << d_Adec * 1e-6 << "ms" << endl;
  cout << "d_BsetBF\t\t" << d_BsetBF * 1e-6 << "ms" << endl;
  cout << "d_BencCr\t\t" << d_BencCr * 1e-6 << "ms" << endl;
  cout << "d_BencCz\t\t" << d_BencCz * 1e-6 << "ms" << endl;
  cout << "d_BhomoCalc\t\t" << d_BhomoCalc * 1e-6 << "ms" << endl;
  cout << (isNear ? "near" : "far") << endl;

  CSVWriter csv(",");
  if (makehead)
    csv.newRow() << "radius "
                 << "d_AkGen"
                 << "d_ApreClac"
                 << "d_Aenc"
                 << "d_Adec"
                 << "d_BsetBF"
                 << "d_BencCr"
                 << "d_BencCz"
                 << "d_BhomoCalc"
                 << "d_A1"
                 << "d_A2"
                 << "d_A3"
                 << "d_B1"
                 << "d_B2"
                 << "d_B3"
                 << "d_Atotal"
                 << "d_Btotal";

  csv.newRow() << radius      //
               << d_AkGen     //
               << d_ApreClac  //
               << d_Aenc      //
               << d_Adec      //
               << d_BsetBF    //
               << d_BencCr    //
               << d_BencCz    //
               << d_BhomoCalc //
               << d_A1        //
               << d_A2        //
               << d_A3        //
               << d_B1        //
               << d_B2        //
               << d_B3        //
               << d_Atotal    //
               << d_Btotal;   //

  csv.writeToFile(filename, !makehead);

  return 1;
}

int main() {

  // int a[1000];
  // for (size_t j = 0; j < 10000; j++) {
  //   for (size_t i = 0; i < 1000; i++) {
  //     a[i] = i * i + j;
  //   }
  // }

  //
  uint64_t radius_lst[9]{16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

  for (size_t i = 0; i < 9; ++i) {
    pppt("./dgk_measure.csv", radius_lst[i], i == 0);
  }

  // dgk_example ();
}
