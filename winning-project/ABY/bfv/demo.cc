#include <sstream>
#include <sys/socket.h>
#define _CRT_SECURE_NO_WARNINGS

#include "bloomfilter.h"
#include "cmdline.h"
#include "examples.h" // print_parameter
#include "seal/seal.h"
#include "util.h"

#include <chrono>
#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;
using namespace seal;

int main(int argc, char *argv[]) {

  cmdline::parser cmd_parser;

  cmd_parser.add<uint64_t>("xa", 'x', "coordinate1 of client", false, 1234,
                           cmdline::range(0ul, 1ul << 27)); // 134217728
  cmd_parser.add<uint64_t>("ya", 'y', "coordinate2 of client", false, 1212,
                           cmdline::range(0ul, 1ul << 27)); // 134217728

  cmd_parser.add<uint64_t>("xb", 'u', "coordinate1 of server", false, 1000,
                           cmdline::range(0ul, 1ul << 27)); // 134217728
  cmd_parser.add<uint64_t>("yb", 'v', "coordinate2 of server", false, 1000,
                           cmdline::range(0ul, 1ul << 27)); // 134217728

  cmd_parser.add<uint64_t>("radius", 'r', "radius/thershold", false, 128,
                           cmdline::range(1, 8192));

  cmd_parser.add<size_t>("plain_modulus_bits", 'b',
                         "bit length of plain modulus", false, 56,
                         cmdline::range(1, 56));

  cmd_parser.add<size_t>("poly_modulus_degree", 'd',
                         "set degree of polynomial(2^d)", false, 13,
                         cmdline::range(12, 15));
  cmd_parser.add<int>("print_bf", 'g', "print bf", false, 0);
  // cmdline::range(0, 1000));
  cmd_parser.parse_check(argc, argv);

  cout << "sizeof(uint64_t)" << sizeof(uint64_t) << endl;
  // debug
  Plaintext pdbg;
  uint64_t xa = cmd_parser.get<uint64_t>("xa");
  uint64_t ya = cmd_parser.get<uint64_t>("ya");
  uint64_t xb = cmd_parser.get<uint64_t>("xb");
  uint64_t yb = cmd_parser.get<uint64_t>("yb");

  uint64_t radius = cmd_parser.get<uint64_t>("radius");
  uint64_t sq_radius = radius * radius;
  int print_bf = cmd_parser.get<int>("print_bf");
  pppt_printf("Client's coordinates:\t(%" PRIu64 ", %" PRIu64 ")\n", xa, ya);
  pppt_printf("Server's coordinates:\t(%" PRIu64 ", %" PRIu64 ")\n", xb, yb);
  pppt_printf("Radius(Threshold):\t\t\t%" PRIu64 "\n", radius);

  auto begin = std::chrono::high_resolution_clock::now();
  // A  KeyGen
  EncryptionParameters parms(scheme_type::bfv);
  size_t poly_modulus_degree_bits =
      cmd_parser.get<size_t>("poly_modulus_degree"); // 4096 * 8
  size_t plain_modulus_bits = cmd_parser.get<size_t>("plain_modulus_bits");
  size_t poly_modulus_degree = 1ull << poly_modulus_degree_bits;
  size_t plain_modulus = 1ull << plain_modulus_bits;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
  parms.set_plain_modulus(plain_modulus); // sq

  SEALContext context(parms);
  print_parameters(context);
  cout << "Parameter validation (success): "
       << context.parameter_error_message() << endl;

  KeyGenerator keygen(context);
  SecretKey sk = keygen.secret_key();
  PublicKey pk;

  keygen.create_public_key(pk);

  cout << "head!!!!!!!!!!!!!!!!!" << endl;
  stringstream stream_pk;
  pk.save(stream_pk);
  // auto a = ;
  // auto len = stream_pk.str().length();
  // stringstream stream_spk(string(stream_pk.str().c_str(), len));

  // int socks = connect_to_client("127.0.0.1", 12345);

  // int sockc = connect_to_server("127.0.0.1", 12345);

  // pk.load(context, stream_spk);
  // cout << "tail!!!!!!!!!!!!!!!!" << endl;
  // stream_cipher << pk << endl;

  Encryptor encryptor(context, pk);
  Evaluator evaluator(context);
  Decryptor decryptor(context, sk);

  // B initialize bloom filter
  bloom_parameters bf_parms;
  bf_parms.projected_element_count = sq_radius;
  bf_parms.false_positive_probability = 0.000000000001; // 1 in 10000
  bf_parms.random_seed = 0xA5A5A5A5;
  bf_parms.compute_optimal_parameters();
  bloom_filter bf(bf_parms);

  // insert key-hashed blind distance
  uint64_t r, s, w; // to fix
  random_bytes((byte *)&r, 4);
  random_bytes((byte *)&s, 4);
  random_bytes((byte *)&w, 2);
  int w_len = get_bitlen(w);
  for (uint64_t di = 0; di < sq_radius; ++di) {
    uint64_t bd = s * (di + r);
    bf.insert((bd << uint64_t(w_len)) | w);
    if (print_bf)
      cout << hex << bd << ' ';
  }
  cout << endl;

  cout << hex << r * s << dec << endl;

  // A ---------------
  uint64_t u = xa * xa + ya * ya;
  Ciphertext c1, c2, c3;
  // cout << 1 << endl;
  Plaintext p1(uint64_to_hex_string(u));
  Plaintext p2(uint64_to_hex_string(xa << 1));
  Plaintext p3(uint64_to_hex_string(ya << 1));

  encryptor.encrypt(p1, c1);
  encryptor.encrypt(p2, c2);
  encryptor.encrypt(p3, c3);

  cout << "stream_cipher head" << endl;
  stringstream stream_cipher;
  c1.save(stream_cipher);
  c1.load(context, stream_cipher);
  cout << "stream_cipher begin" << endl;

  // B ----------------
  uint64_t z = xb * xb + yb * yb;
  Plaintext plain_z(uint64_to_hex_string(z));
  Plaintext plain_xb(uint64_to_hex_string(xb));
  Plaintext plain_yb(uint64_to_hex_string(yb));

  evaluator.add_plain_inplace(c1, plain_z);
  evaluator.multiply_plain_inplace(c2, plain_xb);
  evaluator.multiply_plain_inplace(c3, plain_yb);
  evaluator.add_inplace(c2, c3);
  evaluator.sub_inplace(c1, c2);
  evaluator.multiply_plain_inplace(c1, Plaintext(uint64_to_hex_string(s)));
  evaluator.add_plain_inplace(c1, Plaintext(uint64_to_hex_string(s * r)));

  // A ----------------------
  Plaintext plain_blind_distance;
  decryptor.decrypt(c1, plain_blind_distance);

  cout << plain_blind_distance.to_string() << endl;
  uint64_t blind_distance =
      hex_string_to_uint(plain_blind_distance.to_string());
  cout << "blind_distance: " << hex << blind_distance << dec << endl;

  bool isNear = bf.contains((blind_distance << uint64_t(w_len)) | w);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

  cout << (isNear ? "near" : "far") << endl;
  printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);
}
