#include "benchmark.h"

using namespace std;
using namespace seal;

int main(int argc, char *argv[]) {

  cmdline::parser cmd_parser;
  cmd_parser.add<string>("host", 'h', "ip of server", false, "127.0.0.1");
  cmd_parser.add<uint16_t>("port", 'p', "port of server", false, 51022,
                           cmdline::range(1, 65535));
  cmd_parser.add<uint64_t>("xb", 'x', "coordinate1 of server", false, 123456888,
                           cmdline::range(0ul, 1ul << 27)); // 134217728
  cmd_parser.add<uint64_t>("yb", 'y', "coordinate2 of server", false, 132456777,
                           cmdline::range(0ul, 1ul << 27)); // 134217728

  cmd_parser.add<uint64_t>("radius", 'r', "radius/thershold", false, 128,
                           cmdline::range(1, 8192));

  cmd_parser.parse_check(argc, argv);

  cmd_parser.add<uint64_t>("hkey", 'w', "hash key", false, 128,
                           cmdline::range(1, 8192));

  cmd_parser.add<uint64_t>("shift", 's', "shift", false, 128,
                           cmdline::range(1, 8192));

  cmd_parser.parse_check(argc, argv);

  string ip = cmd_parser.get<string>("host");
  uint16_t port = cmd_parser.get<uint16_t>("port");
  uint64_t xb = cmd_parser.get<uint64_t>("xb");
  uint64_t yb = cmd_parser.get<uint64_t>("yb");
  uint64_t radius = cmd_parser.get<uint64_t>("radius");

  uint64_t z = xb * xb + yb * yb;
  uint64_t sq_radius = radius * radius;

  int sockfd_client = connect_to_client(ip, port, AF_INET);
  if (sockfd_client < 0)
    return sockfd_client;

  pppt_printf("Test start...\n");

  // test leg
  radius = 16;
  for (size_t i = 0; i < 9; ++i) {
    auto raw_data = test_server_leg(sockfd_client, radius, ip, port, xb, yb);
    parse2csv_server_leg("./server_leg.csv", radius, i == 0, raw_data);
    radius <<= 1;
  }

  // test opt
  radius = 16;
  for (size_t i = 0; i < 9; ++i) { // radius 8, 16, 32, ... , 4096 (4 ... 12)
    auto raw_data = test_server_opt(sockfd_client, radius, ip, port, xb, yb);
    parse2csv_server_opt("./server_opt.csv", radius, i == 0, raw_data);
    radius <<= 1;
  }

  close(sockfd_client);
}

// server
static std::pair<TrafficLoad, DurationServer>
test_server_leg(int sockfd_client, uint64_t radius, //
                std::string ip, uint16_t port,      //
                uint64_t xb, uint64_t yb) {
  auto z = xb * xb + yb * yb;
  auto sq_radius = radius * radius;

  pppt_printf("Proximity test start...\n");
  pppt_printf("Server's coordinates:\t(%" PRIu64 ", %" PRIu64 ")\n", xb, yb);
  pppt_printf("Radius:\t\t\t\t%" PRIu64 "\n", radius);

  auto t_begin = chrono::high_resolution_clock::now();

  auto t_recvParms0 = chrono::high_resolution_clock::now();

  // receive the parms from the client
  ssize_t bytes = recv(sockfd_client, buf, sizeof(buf), 0);
  pppt_printf("Receive parms from the client, bytes: %zu\n", size_t(bytes));

  auto t_recvParms1 = chrono::high_resolution_clock::now();

  auto t_setParms0 = chrono::high_resolution_clock::now();

  // set the parms and the context
  EncryptionParameters parms;
  stringstream stream_parms;
  stream_parms << string((char *)buf, bytes);
  parms.load(stream_parms);
  SEALContext context(parms);

  auto t_setParms1 = chrono::high_resolution_clock::now();

  if (flag_log)
    print_parameters(context);
  pppt_printf("Parameter validation: %s\n", context.parameter_error_message());

  auto t_recvPk0 = chrono::high_resolution_clock::now();

  // receive the pk from the client
  stringstream stream_pk;
  bytes = recv_by_stream(sockfd_client, stream_pk);
  pppt_printf("Receive the public key from the client, bytes: %zu\n",
              size_t(bytes));
  PublicKey pk;
  pk.load(context, stream_pk);

  auto t_recvPk1 = chrono::high_resolution_clock::now();

  auto t_setBF0 = chrono::high_resolution_clock::now();

  // set the bloom filter
  bloom_parameters bf_parms;
  bf_parms.projected_element_count = sq_radius;
  bf_parms.false_positive_probability = 0.0001; // 1 in 10000
  bf_parms.random_seed = 0xA5A5A5A5;
  bf_parms.compute_optimal_parameters();
  bloom_filter bf(bf_parms);
  // generate the random number
  uint64_t r, s, w; // to fix
  random_bytes((byte *)&r, 4);
  random_bytes((byte *)&s, 4);
  random_bytes((byte *)&w, 2);
  int w_len = get_bitlen(w);
  for (uint64_t di = 0; di < sq_radius; ++di) {
    uint64_t bd = s * (di + r);
    bf.insert((bd << uint64_t(w_len)) | w);
  }

  auto t_setBF1 = chrono::high_resolution_clock::now();

  auto t_recvCips0 = chrono::high_resolution_clock::now();

  // receive the encrypted data
  vector<Ciphertext> lst_cipher(3);
  // for each ciphertext
  for (size_t id_cipher = 0; id_cipher < 3; id_cipher++) {
    stringstream stream_cipher;
    bytes = recv_by_stream(sockfd_client, stream_cipher);
    lst_cipher[id_cipher].load(context, stream_cipher);
    pppt_printf("Recv the ciphertext %zu, bytes: %zu\n", id_cipher,
                size_t(bytes));
  }

  auto t_recvCips1 = chrono::high_resolution_clock::now();

  auto t_homoCalc0 = chrono::high_resolution_clock::now();

  //  homomorphic evaluation
  Evaluator evaluator(context);
  Plaintext plain_z(uint64_to_hex_string(z));
  Plaintext plain_xb(uint64_to_hex_string(xb));
  Plaintext plain_yb(uint64_to_hex_string(yb));
  evaluator.add_plain_inplace(lst_cipher[0], plain_z);
  evaluator.multiply_plain_inplace(lst_cipher[1], plain_xb);
  evaluator.multiply_plain_inplace(lst_cipher[2], plain_yb);
  evaluator.add_inplace(lst_cipher[1], lst_cipher[2]);
  evaluator.sub_inplace(lst_cipher[0], lst_cipher[1]);
  evaluator.multiply_plain_inplace(lst_cipher[0],
                                   Plaintext(uint64_to_hex_string(s)));
  evaluator.add_plain_inplace(lst_cipher[0],
                              Plaintext(uint64_to_hex_string(s * r)));

  auto t_homoCalc1 = chrono::high_resolution_clock::now();

  auto t_sendBF0 = chrono::high_resolution_clock::now();

  // send the bloom filter and hash key
  bytes = sizeof(uint64_t) + bf.compute_serialization_size();
  bytes_to_send(sockfd_client, bytes);
  uint8_t *bf_buf = (uint8_t *)malloc(bytes);
  *(uint64_t *)bf_buf = w;
  bf.serialize(bf_buf + sizeof(uint64_t));
  bytes = send(sockfd_client, bf_buf, bytes, 0);
  pppt_printf("Send the BF and hash key, bytes sent: %zu\n", size_t(bytes));
  free(bf_buf);

  auto t_sendBF1 = chrono::high_resolution_clock::now();

  auto t_sendBD0 = chrono::high_resolution_clock::now();

  // send the encrypted blind distance
  stringstream stream_cipher;
  lst_cipher[0].save(stream_cipher);
  bytes_to_send(sockfd_client, stream_cipher.str().length());
  bytes = send(sockfd_client, stream_cipher.str().c_str(),
               stream_cipher.str().length(), 0);

  auto t_sendBD1 = chrono::high_resolution_clock::now();

  auto t_end = chrono::high_resolution_clock::now();

  pppt_printf("Send the encrypted blind distance, bytes sent: %zu\n", bytes);

  // parse the time points

  DurationServer du;

  du.d_recvParms = get_diff(t_recvParms1, t_recvParms0); //
  du.d_recvPk = get_diff(t_recvPk1, t_recvPk0);          //
  du.d_setParms = get_diff(t_setParms1, t_setParms0);    //

  du.d_setBF = get_diff(t_setBF1, t_setBF0);    //
  du.d_sendBF = get_diff(t_sendBF1, t_sendBF0); //

  du.d_recvCips = get_diff(t_recvCips1, t_recvCips0); //
  du.d_homoCalc = get_diff(t_homoCalc1, t_homoCalc0); //
  du.d_sendBD = get_diff(t_sendBD1, t_sendBD0);       //

  // du.d_stage1 = du.d_setParms + du.d_setBF; //
  // du.d_stage2 = du.d_homoCalc;              //
  // du.d_stage3 = 0.00;                       //

  du.d_stage1 = du.d_setParms;                              //
  du.d_stage2 = du.d_setBF;                                 //
  du.d_stage3 = du.d_homoCalc;                              //
  du.d_total = get_diff(t_end, t_begin);                    //
  du.d_totalTraffic = 0.0;                                  //
  du.d_totalCalc = du.d_stage1 + du.d_stage2 + du.d_stage3; //

  return {
      {}, //
      du  // dummy
  };
}

static std::pair<TrafficLoad, DurationServer>
test_server_opt(int sockfd_client, uint64_t radius, //
                std::string ip, uint16_t port,      //
                uint64_t xb, uint64_t yb) {

  auto z = xb * xb + yb * yb;
  auto sq_radius = radius * radius;

  pppt_printf("Proximity test start...\n");
  pppt_printf("Server's coordinates:\t(%" PRIu64 ", %" PRIu64 ")\n", xb, yb);
  pppt_printf("Radius:\t\t\t\t%" PRIu64 "\n", radius);

  auto t_begin = chrono::high_resolution_clock::now();

  auto t_recvParms0 = chrono::high_resolution_clock::now();

  // receive the parms from the client
  ssize_t bytes = recv(sockfd_client, buf, sizeof(buf), 0);
  pppt_printf("Receive parms from the client, bytes: %zu\n", size_t(bytes));

  auto t_recvParms1 = chrono::high_resolution_clock::now();

  auto t_setParms0 = chrono::high_resolution_clock::now();

  // set the parms and the context
  EncryptionParameters parms;
  stringstream stream_parms;
  stream_parms << string((char *)buf, bytes);
  parms.load(stream_parms);
  SEALContext context(parms);

  auto t_setParms1 = chrono::high_resolution_clock::now();

  if (flag_log)
    print_parameters(context);
  pppt_printf("Parameter validation: %s\n", context.parameter_error_message());

  auto t_setBF0 = chrono::high_resolution_clock::now();

  // set the bloom filter
  bloom_parameters bf_parms;
  bf_parms.projected_element_count = sq_radius;
  bf_parms.false_positive_probability = 0.0001; // 1 in 10000
  bf_parms.random_seed = 0xA5A5A5A5;
  bf_parms.compute_optimal_parameters();
  bloom_filter bf(bf_parms);
  // generate the random number
  uint64_t r, s, w; // to fix
  random_bytes((byte *)&r, 4);
  random_bytes((byte *)&s, 4);
  random_bytes((byte *)&w, 2);
  int w_len = get_bitlen(w);
  for (uint64_t di = 0; di < sq_radius; ++di) {
    uint64_t bd = s * (di + r);
    bf.insert((bd << uint64_t(w_len)) | w);
  }

  auto t_setBF1 = chrono::high_resolution_clock::now();

  auto t_recvCips0 = chrono::high_resolution_clock::now();

  // receive the encrypted data
  vector<Ciphertext> lst_cipher(3);
  // for each ciphertext
  for (size_t id_cipher = 0; id_cipher < 3; id_cipher++) {
    stringstream stream_cipher;
    bytes = recv_by_stream(sockfd_client, stream_cipher);
    lst_cipher[id_cipher].load(context, stream_cipher);
    pppt_printf("Recv the ciphertext %zu, bytes: %zu\n", id_cipher,
                size_t(bytes));
  }

  auto t_recvCips1 = chrono::high_resolution_clock::now();

  auto t_homoCalc0 = chrono::high_resolution_clock::now();

  //  homomorphic evaluation
  Evaluator evaluator(context);
  Plaintext plain_z(uint64_to_hex_string(z));
  Plaintext plain_xb(uint64_to_hex_string(xb));
  Plaintext plain_yb(uint64_to_hex_string(yb));
  evaluator.add_plain_inplace(lst_cipher[0], plain_z);
  evaluator.multiply_plain_inplace(lst_cipher[1], plain_xb);
  evaluator.multiply_plain_inplace(lst_cipher[2], plain_yb);
  evaluator.add_inplace(lst_cipher[1], lst_cipher[2]);
  evaluator.sub_inplace(lst_cipher[0], lst_cipher[1]);
  evaluator.multiply_plain_inplace(lst_cipher[0],
                                   Plaintext(uint64_to_hex_string(s)));
  evaluator.add_plain_inplace(lst_cipher[0],
                              Plaintext(uint64_to_hex_string(s * r)));

  auto t_homoCalc1 = chrono::high_resolution_clock::now();

  auto t_sendBF0 = chrono::high_resolution_clock::now();

  // send the bloom filter and hash key
  bytes = sizeof(uint64_t) + bf.compute_serialization_size();
  bytes_to_send(sockfd_client, bytes);
  uint8_t *bf_buf = (uint8_t *)malloc(bytes);
  *(uint64_t *)bf_buf = w;
  bf.serialize(bf_buf + sizeof(uint64_t));
  bytes = send(sockfd_client, bf_buf, bytes, 0);
  pppt_printf("Send the BF and hash key, bytes sent: %zu\n", size_t(bytes));
  free(bf_buf);

  auto t_sendBF1 = chrono::high_resolution_clock::now();

  auto t_sendBD0 = chrono::high_resolution_clock::now();

  // send the encrypted blind distance
  stringstream stream_cipher;
  lst_cipher[0].save(stream_cipher);
  bytes_to_send(sockfd_client, stream_cipher.str().length());
  bytes = send(sockfd_client, stream_cipher.str().c_str(),
               stream_cipher.str().length(), 0);

  auto t_sendBD1 = chrono::high_resolution_clock::now();

  auto t_end = chrono::high_resolution_clock::now();

  pppt_printf("Send the encrypted blind distance, bytes sent: %zu\n", bytes);

  // parse the time points

  DurationServer du;

  du.d_recvParms = get_diff(t_recvParms1, t_recvParms0); //
  du.d_setParms = get_diff(t_setParms1, t_setParms0);    //
  du.d_setBF = get_diff(t_setBF1, t_setBF0);             //
  du.d_recvCips = get_diff(t_recvCips1, t_recvCips0);    //

  du.d_homoCalc = get_diff(t_homoCalc1, t_homoCalc0); //
  du.d_sendBF = get_diff(t_sendBF1, t_sendBF0);       //
  du.d_sendBD = get_diff(t_sendBD1, t_sendBD0);       //

  du.d_stage1 = du.d_setParms + du.d_setBF; //
  du.d_stage2 = du.d_homoCalc;              //
  du.d_stage3 = 0.00;                       //

  du.d_total = get_diff(t_end, t_begin);                    //
  du.d_totalTraffic = 0.0;                                  //
  du.d_totalCalc = du.d_stage1 + du.d_stage2 + du.d_stage3; //

  return {
      {}, //
      du  // dummy
  };
}
