

#ifndef __PPPT_H_
#define __PPPT_H_

#include <abycore/aby/abyparty.h>
#include <abycore/circuit/arithmeticcircuits.h>
#include <abycore/circuit/booleancircuits.h>
#include <abycore/circuit/circuit.h>
#include <cassert>
#include <math.h>

#define ALICE "ALICE"
#define BOB "BOB"

int32_t test_pppt_circuit (e_role role, const std::string &address,
                           uint16_t port, seclvl seclvl, uint32_t bitlen,
                           uint32_t nthreads, e_mt_gen_alg mt_alg,
                           e_sharing sharing, uint32_t thr);

share *BuildPPPTCircuit (share *x0, share *x1, share *y0, share *y1,
                         share *thr, Circuit *circA, Circuit *circY);

#endif /* __PPPT_H_ */
