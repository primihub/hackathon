
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

  Circuit *circA = sharings[S_ARITH]->GetCircuitBuildRoutine ();
  Circuit *circY = sharings[S_YAO]->GetCircuitBuildRoutine ();

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

  // // SERVER r 0
  s_x0 = circA->PutINGate (x0, blen, SERVER);
  s_y0 = circA->PutINGate (y0, blen, SERVER);

  // // CLIENT r 1
  s_x1 = circA->PutINGate (x1, blen, CLIENT);
  s_y1 = circA->PutINGate (y1, blen, CLIENT);

  // thr must be publicly known
  thr = thr * thr;

  std::cout << thr << std::endl;

  s_thr = circY->PutCONSGate (thr, 2 * blen);

  s_out = BuildPPPTCircuit (s_x0, s_x1, s_y0, s_y1, s_thr, circA, circY);

  s_out = circY->PutOUTGate (s_out, ALL);

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
                  Circuit *circA, Circuit *circY)
{

  share *x00 = circA->PutMULGate (x0, x0);
  share *x11 = circA->PutMULGate (x1, x1);
  share *x01 = circA->PutMULGate (x0, x1);

  share *y00 = circA->PutMULGate (y0, y0);
  share *y11 = circA->PutMULGate (y1, y1);
  share *y01 = circA->PutMULGate (y0, y1);

  share *x = circA->PutADDGate (x00, x11);
  x = circA->PutSUBGate (x, x01);
  x = circA->PutSUBGate (x, x01);

  share *y = circA->PutADDGate (y00, y11);
  y = circA->PutSUBGate (y, y01);
  y = circA->PutSUBGate (y, y01);

  share *dist = circA->PutADDGate (x, y);
  dist = circY->PutA2YGate (dist);

  share *ret = circY->PutGTGate (thr, dist);

  return ret;
}
