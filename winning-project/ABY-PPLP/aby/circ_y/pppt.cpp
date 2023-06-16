
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

  // share *s_alice_money, *s_bob_money;
  // uint32_t alice_money, bob_money;

  // s_alice_money = circ->PutINGate(alice_money, bitlen, CLIENT);
  // s_bob_money = circ->PutINGate(bob_money, bitlen, SERVER);

  // final
  share *s_out;
  uint32_t output;

  share *s_x0, *s_x1, *s_y0, *s_y1, *s_thr;
  uint32_t x0, x1, y0, y1;
  //  thr;

  srand (time (NULL));

  // rand bitlen range
  uint32_t blen = 15;
  // RAND_MAX = 2 ^ 32 = 21 4748 3647
  x0 = rand () % (1 << blen);
  y0 = rand () % (1 << blen);
  
  x1 = rand () % (1 << blen);
  y1 = rand () % (1 << blen);

  // x0 = 1;
  // y0 = 1;
  // x1 = 10;
  // y1 = 10;
  // thr = 10;

  // thr %= (1 << blen);

  // if (role == SERVER)
  //   {
  //     s_x0 = circ->PutINGate (x0, blen, SERVER);
  //     s_y0 = circ->PutINGate (y0, blen, SERVER);
  //     s_x1 = circ->PutDummyINGate (blen);
  //     s_y1 = circ->PutDummyINGate (blen);
  //   }
  // else // role == CLIENT
  //   {
  //     s_x0 = circ->PutDummyINGate (blen);
  //     s_y0 = circ->PutDummyINGate (blen);
  //     s_x1 = circ->PutINGate (x1, blen, CLIENT);
  //     s_y1 = circ->PutINGate (y1, blen, CLIENT);
  //   }

  // CLINET r 1
  s_x0 = circ->PutINGate (x0, blen, CLIENT);
  s_y0 = circ->PutINGate (y0, blen, CLIENT);

  // SERVER r 0
  s_x1 = circ->PutINGate (x1, blen, SERVER);
  s_y1 = circ->PutINGate (y1, blen, SERVER);

  // thr must be publicly known
  thr = thr * thr;
  std::cout << thr << std::endl;

  s_thr = circ->PutCONSGate (thr, 2 * blen);

  s_out = BuildPPPTCircuit (s_x0, s_x1, s_y0, s_y1, s_thr, circ);

  s_out = circ->PutOUTGate (s_out, ALL);

  party->ExecCircuit ();

  output = s_out->get_clear_value<uint32_t> ();

  std::cout << "output " << output << std::endl;

  std::cout << "Testing PPPT in " << get_sharing_name (sharing)
            << " sharing: " << std::endl
            << std::endl;

  std::cout << "x0, y0:\t" << x0 << ", " << y0 << std::endl;
  std::cout << "x1, y1:\t" << x1 << ", " << y1 << std::endl;

  std::cout << "Circuit Result:\t" << (output ? "CLOSE" : "FAR") << std::endl;
  std::cout << "Local Verify Result: \t"
            << (((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1) < thr) ? "CLOSE"
                                                                      : "FAR")
            << std::endl;
  PrintTimings ();
  delete party;
  return 0;
}

// just to del prefix s_ in var name
share *
BuildPPPTCircuit (share *x0, share *x1, share *y0, share *y1, share *thr,
                  Circuit *circ)
{

  share *gtX = circ->PutGTGate (x0, x1);
  share **swappedX = circ->PutCondSwapGate (x0, x1, gtX, 0);
  share *xmax = swappedX[0];
  share *xmin = swappedX[1];

  share *gtY = circ->PutGTGate (y0, y1);
  share **swappedY = circ->PutCondSwapGate (y0, y1, gtY, 0);
  share *ymax = swappedX[0];
  share *ymin = swappedX[1];

  share *factorX = circ->PutSUBGate (xmax, xmin);
  share *factorY = circ->PutSUBGate (ymax, ymin);

  share *squareX = circ->PutMULGate (factorX, factorX);
  share *squareY = circ->PutMULGate (factorY, factorY);

  // dist
  share *dist = circ->PutADDGate (squareX, squareY);
  share *ret = circ->PutGTGate (thr, dist);

  // share *ret = bc->PutMUXGate (bc->PutCONSGate ((uint64_t)0, 1),
  //                              bc->PutCONSGate ((uint64_t)1, 1), gt);

  return ret;
}
