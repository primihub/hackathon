#include "seal/seal.h"

#include "bloomfilter.h"
#include "cmdline.h"
#include "examples.h" // print_parameter
#include "util.h"

#include <chrono>
#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <ios>
#include <iostream>
#include <vector>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
using namespace seal;

int main(int argc, char *argv[]) {

  cmdline::parser cmd_parser;
  cmd_parser.add<string>("host", 'h', "ip of server", false, "127.0.0.1");
  cmd_parser.add<uint16_t>("port", 'p', "port of server", false, 51022,
                           cmdline::range(1, 65535));

  cmd_parser.add<uint64_t>("xa", 'x', "coordinate1 of client", false, 123456789,
                           cmdline::range(0ul, 1ul << 27)); // 134217728
  cmd_parser.add<uint64_t>("ya", 'y', "coordinate2 of client", false, 132456888,
                           cmdline::range(0ul, 1ul << 27)); // 134217728

  cmd_parser.add<size_t>("plain_modulus_bits", 'b',
                         "bit length of plain modulus", false, 56,
                         cmdline::range(1, 56));

  cmd_parser.add<uint64_t>("radius", 'r', "radius/thershold", false, 128,
                           cmdline::range(1, 8192));

  cmd_parser.add<size_t>("poly_modulus_degree", 'd',
                         "set degree of polynomial(2^d)", false, 13,
                         cmdline::range(12, 15));

  // cmd_parser.add("ipv4", '4', "ipv4");
  cmd_parser.add("ipv6", '6', "ipv6", 0, 0);

  cmd_parser.parse_check(argc, argv);

  // radius
  string ip = cmd_parser.get<string>("host");
  uint16_t port = cmd_parser.get<uint16_t>("port");
  // int domain = AF_INET6; // cmd_parser.exist("ipv4") ? AF_INET : AF_INET6;

  int domain = cmd_parser.exist("ipv6") ? AF_INET6 : AF_INET;
  // cout << boolalpha << cmd_parser.exist("ipv6") << endl;
  uint64_t xa = cmd_parser.get<uint64_t>("xa");
  uint64_t ya = cmd_parser.get<uint64_t>("ya");
  uint64_t radius = cmd_parser.get<uint64_t>("radius");
  // uint64_t sq_radius = radius * radius;

  uint64_t u = xa * xa + ya * ya;

  size_t poly_modulus_degree_bits =
      cmd_parser.get<size_t>("poly_modulus_degree");
  size_t plain_modulus_bits = cmd_parser.get<size_t>("plain_modulus_bits");

  // Connecting............
  int sockfd_server = connect_to_server(ip, port, domain);
  if (sockfd_server < 0) // fail
    return 1;

  pppt_printf("Connected to the server,  proximity test start...\n");
  pppt_printf("Client's coordinates:\t(%" PRIu64 ", %" PRIu64 ")\n", xa, ya);
  pppt_printf("Radius:\t\t\t\t%" PRIu64 "\n", radius);

  auto begin = chrono::high_resolution_clock::now();

  // set the parms
  EncryptionParameters parms(scheme_type::bfv);
  size_t poly_modulus_degree = 1ull << poly_modulus_degree_bits;
  size_t plain_modulus = 1ull << plain_modulus_bits;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
  parms.set_plain_modulus(plain_modulus); // sq
  // set the context
  SEALContext context(parms);

  // send the parms
  stringstream stream_parms;
  parms.save(stream_parms);
  auto bytes = send(sockfd_server, stream_parms.str().c_str(),
                    stream_parms.str().length(), 0);
  pppt_printf("Send parms(context), bytes: %zu \n", size_t(bytes));

  if (flag_log)
    print_parameters(context);
  pppt_printf("Parameter validation: %s\n", context.parameter_error_message());

  // generate sk and pk
  KeyGenerator keygen(context);
  SecretKey sk = keygen.secret_key();
  PublicKey pk;
  keygen.create_public_key(pk);

  // encrypt the data
  Encryptor encryptor(context, pk);
  Ciphertext c1, c2, c3; // x^2+y^2, 2x, 2y
  encryptor.encrypt(Plaintext(uint64_to_hex_string(u)), c1);
  encryptor.encrypt(Plaintext(uint64_to_hex_string(xa << 1)), c2);
  encryptor.encrypt(Plaintext(uint64_to_hex_string(ya << 1)), c3);

  // send the ciphertext
  vector<Ciphertext> lst_cipher{c1, c2, c3};
  for (size_t id_cipher = 0; id_cipher < 3; id_cipher++) {
    stringstream stream_cipher;
    lst_cipher[id_cipher].save(stream_cipher);
    bytes = send_by_stream(sockfd_server, stream_cipher);
    pppt_printf("Send the ciphertext %zu, bytes: %zu\n", id_cipher,
                size_t(bytes));
  }

  // receive the bloom filter (w || BF)
  bytes = bytes_to_receive(sockfd_server);
  //
  uint8_t *bf_buf = (uint8_t *)malloc(bytes);
  ssize_t remain_bytes = bytes;
  for (uint8_t *ptr = bf_buf; remain_bytes != 0;) {
    ssize_t cur_bytes = recv(sockfd_server, ptr, remain_bytes, 0);
    ptr += cur_bytes;
    remain_bytes -= cur_bytes;
  }
  uint64_t w = *(uint64_t *)bf_buf;
  bloom_filter bf(bf_buf + sizeof(uint64_t));
  pppt_printf("Recv the BF and hash key, bytes: %zu\n", size_t(bytes));
  //
  free(bf_buf);

  // receive the encrypted blind distance
  stringstream stream_cipher;
  bytes = recv_by_stream(sockfd_server, stream_cipher);
  Ciphertext cipher_blind_distance;
  cipher_blind_distance.load(context, stream_cipher);
  pppt_printf("Recv the encrypted blind distance, bytes: %zu\n", size_t(bytes));

  // decrypt the result to get the blind distance
  Decryptor decryptor(context, sk);
  Plaintext plain_blind_distance;
  decryptor.decrypt(cipher_blind_distance, plain_blind_distance);

  uint64_t blind_distance =
      hex_string_to_uint(plain_blind_distance.to_string());

  pppt_printf("blind_distance: %" PRIu64 "\n", blind_distance);

  bool isNear = bf.contains((blind_distance << get_bitlen(w)) | w);
  auto end = chrono::high_resolution_clock::now();
  auto elapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin);

  close(sockfd_server);

  //
  cout << "Result of proximity test: " << (isNear ? "near" : "far") << endl;
  printf("Time measured: %.3f seconds\n", elapsed.count() * 1e-9);
  return 0;
}