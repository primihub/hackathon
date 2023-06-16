
#include "pppt.h"
#include <abycore/circuit/booleancircuits.h>
#include <abycore/sharing/sharing.h>

int32_t
test_pppt_circuit (e_role role, const std::string &address, uint16_t port,
                   seclvl seclvl, uint32_t bitlen, uint32_t nthreads,
                   e_mt_gen_alg mt_alg, e_sharing sharing, uint32_t thr)
{

  // init circuit
  ABYParty *party
      = new ABYParty (role, address, port, seclvl, bitlen, nthreads, mt_alg);
  std::vector<Sharing *> &sharings = party->GetSharings ();
  Circuit *circ = sharings[sharing]->GetCircuitBuildRoutine ();

  // final
  share *s_out;
  uint32_t output;

  // num of edges
  constexpr size_t n = 4;
  
  
  int radius = thr;
  int32_t a[n]{ radius, -radius, -radius, radius };
  int32_t b[n]{ radius, radius, -radius, -radius };

  // int32_t a[n]{ 10, 20, 20, 10 };
  // int32_t b[n]{ 10, 10, 20, 20 };

  int32_t x[n], y[n], z[n];
  int32_t xp, yp;
  share *s_x[n], *s_y[n], *s_z[n], *s_thr;
  share *s_xp, *s_yp;

  for (size_t i = 0; i + 1 < n; ++i)
    {
      x[i] = b[i] - b[i + 1];
      y[i] = a[i + 1] - a[i];
      z[i] = a[i] * b[i + 1] - a[i + 1] * b[i];
    }

  x[n - 1] = b[n - 1] - b[0];
  y[n - 1] = a[0] - a[n - 1];
  z[n - 1] = a[n - 1] * b[0] - a[0] * b[n - 1];

  // max positive integer
  // thr = (1 << (bitlen - 1)) -1 ;
  thr = 0x7fffffff;

  xp = 100;
  yp = 111;

  // srand (time (NULL));
  // uint32_t blen = 15;
  // rand bitlen range
  // RAND_MAX = 2 ^ 32 = 21 4748 3647

  s_thr = circ->PutCONSGate (thr, bitlen);

  // CLINET r 1
  s_xp = circ->PutINGate ((uint32_t)xp, bitlen, CLIENT);
  s_yp = circ->PutINGate ((uint32_t)yp, bitlen, CLIENT);

  // SERVER r 0
  for (size_t i = 0; i < n; ++i)
    {
      s_x[i] = circ->PutINGate ((uint32_t)x[i], bitlen, SERVER);
      s_y[i] = circ->PutINGate ((uint32_t)y[i], bitlen, SERVER);
      s_z[i] = circ->PutINGate ((uint32_t)z[i], bitlen, SERVER);
    }

  s_out = BuildPPPTCircuit (n, s_x, s_y, s_z, //
                            s_xp, s_yp,       //
                            s_thr, (BooleanCircuit *)circ);

  s_out = circ->PutOUTGate (s_out, ALL);

  party->ExecCircuit ();

  output = s_out->get_clear_value<uint32_t> ();

  std::cout << "output " << output << std::endl;

  std::cout << "Vertices of Ploygon: " <<"(±" << radius<< ", ±" << radius <<")" <<  std::endl;
  std::cout << "Point: \t\t\t" <<"(" << xp << ", " << yp <<")" <<  std::endl;

  std::cout << "Testing PPPT in " << get_sharing_name (sharing)
            << " sharing: " << std::endl << std::endl;

  // std::cout << "x0, y0:\t" << x0 << ", " << y0 << std::endl;
  // std::cout << "x1, y1:\t" << x1 << ", " << y1 << std::endl;

  std::cout << "Circuit Result:\t" << (!output ? "CLOSE" : "FAR") << std::endl;

  int local_ret = 1;
  for (int i = 0; i < n; ++i)
    {
      if (x[i] * xp + y[i] * yp + z[i] < 0)
        {
          local_ret = 0;
          break;
        }
    }

  std::cout << "Local Verify Result: \t" << (local_ret ? "CLOSE" : "FAR")  << std::endl;
  PrintTimings ();
  delete party;
  return 0;
}

// just to del prefix s_ in var name
share *
BuildPPPTCircuit (size_t edges, share **x, share **y, share **z, share *xp,
                  share *yp, share *thr, BooleanCircuit *circ)
{

  share *px = circ->PutMULGate (x[0], xp);
  share *py = circ->PutMULGate (y[0], yp);

  share *orient = circ->PutADDGate (px, py);
  orient = circ->PutADDGate (orient, z[0]);

  share *res = circ->PutGTGate (orient, thr);

  for (size_t i = 1; i < edges; ++i)
    {
      px = circ->PutMULGate (x[i], xp);
      py = circ->PutMULGate (y[i], xp);

      orient = circ->PutADDGate (px, py);
      orient = circ->PutADDGate (orient, z[i]);

      res = circ->PutORGate (res, circ->PutGTGate (orient, thr));
    }

  return res;
  // return circ->PutCONSGate (1, 32);
}
