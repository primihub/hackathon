#include "seal/seal.h"

#include "bloomfilter.h"
#include "cmdline.h"
#include "csvwriter.h"
#include "examples.h" // print_parameter
#include "util.h"

#include <chrono>
#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

struct TrafficLoad {
  using tl_t = uint64_t;
  tl_t c_sendParms; //
  tl_t c_sendPk;    //
  tl_t c_sendCips;  //
  tl_t c_recvBF;    //
  tl_t c_recvBD;    //
  tl_t c_totalSend; //
  tl_t c_totalRecv; //
  tl_t c_total;     //
};

struct DurationClient {
  using dr_t = uint64_t;
  dr_t d_setParms;     //
  dr_t d_kGen;         //
  dr_t d_enc;          //
  dr_t d_sendPk;       //
  dr_t d_sendParms;    //
  dr_t d_sendCip;      //
  dr_t d_recvBF;       //
  dr_t d_recvBD;       //
  dr_t d_dec;          //
  dr_t d_stage1;       //
  dr_t d_stage2;       //
  dr_t d_stage3;       //
  dr_t d_total;        //
  dr_t d_totalCalc;    //
  dr_t d_totalTraffic; //
};

struct DurationServer {
  using dr_t = uint64_t;
  dr_t d_recvParms;    //
  dr_t d_recvPk;       //
  dr_t d_setParms;     //
  dr_t d_setBF;        //
  dr_t d_sendBF;       //
  dr_t d_recvCips;     //
  dr_t d_homoCalc;     //
  dr_t d_sendBD;       //
  dr_t d_stage1;       //
  dr_t d_stage2;       //
  dr_t d_stage3;       //
  dr_t d_total;        //
  dr_t d_totalTraffic; //
  dr_t d_totalCalc;    //
};

// get the diff of time
auto get_diff = [](std::chrono::system_clock::time_point t1,
                   std::chrono::system_clock::time_point t0) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
};

//// variable name
// t_xxx -- time point
// c_xxx -- traffic load

// client
static std::pair<TrafficLoad, DurationClient>
test_client_leg(int sockfd_server, uint64_t radius, //
                std::string ip, uint16_t port,      //
                uint64_t xa, uint64_t ya,           //
                uint64_t poly_modulus_degree_bits,  //
                uint64_t plain_modulus_bits);

static std::pair<TrafficLoad, DurationClient>
test_client_opt(int sockfd_server, uint64_t radius, //
                std::string ip, uint16_t port,      //
                uint64_t xa, uint64_t ya,           //
                uint64_t poly_modulus_degree_bits,  //
                uint64_t plain_modulus_bits);

// server
// dummy TrafficLoad -- pair use only for later Duraiton
static std::pair<TrafficLoad, DurationServer>
test_server_leg(int sockfd_client, uint64_t radius, //
                std::string ip, uint16_t port,      //
                uint64_t xb, uint64_t yb);

static std::pair<TrafficLoad, DurationServer>
test_server_opt(int sockfd_client, uint64_t radius, //
                std::string ip, uint16_t port,      //
                uint64_t xb, uint64_t yb);

// parser +++++++++++++

static void parse2csv_client_leg(const char *filename, uint64_t radius,
                                 bool makehead,
                                 std::pair<TrafficLoad, DurationClient> input) {

  CSVWriter csv(",");

  auto tf = input.first;
  auto du = input.second;
  if (makehead)
    csv.newRow() << "radius"
                 << "d_setParms"
                 << "d_kGen"
                 << "d_sendPk"
                 << "d_sendParms"
                 << "d_recvBF"
                 << "d_enc"
                 << "d_sendCip"
                 << "d_recvBD"
                 << "d_dec"
                 << "d_stage1"
                 << "d_stage2"
                 << "d_stage3"
                 << "d_total"
                 << "d_totalCalc"
                 << "d_totalTraffic" //
                 << "c_sendParms"
                 << "c_sendPk"
                 << "c_sendCips"
                 << "c_recvBF"
                 << "c_recvBD"
                 << "c_totalSend"
                 << "c_totalRecv"
                 << "c_total";

  csv.newRow() << radius            //
               << du.d_setParms     //
               << du.d_kGen         //
               << du.d_sendPk       //
               << du.d_sendParms    //
               << du.d_recvBF       //
               << du.d_enc          //
               << du.d_sendCip      //
               << du.d_recvBD       //
               << du.d_dec          //
               << du.d_stage1       //
               << du.d_stage2       //
               << du.d_stage3       //
               << du.d_total        //
               << du.d_totalCalc    //
               << du.d_totalTraffic //
               << tf.c_sendParms    //
               << tf.c_sendPk       //
               << tf.c_sendCips     //
               << tf.c_recvBF       //
               << tf.c_recvBD       //
               << tf.c_totalSend    //
               << tf.c_totalRecv    //
               << tf.c_total;       //

  // if makehead, trunc; else append
  csv.writeToFile(filename, !makehead);
}

static void parse2csv_server_leg(const char *filename, uint64_t radius,
                                 bool makehead,
                                 std::pair<TrafficLoad, DurationServer> input) {
  CSVWriter csv(",");

  auto du = input.second;

  if (makehead)
    csv.newRow() << "radius"
                 << "d_recvParms"
                 << "d_recvPk"
                 << "d_setParms"
                 << "d_setBF"
                 << "d_sendBF"
                 << "d_recvCips"
                 << "d_homoCalc"
                 << "d_sendBD"
                 << "d_stage1"
                 << "d_stage2"
                 << "d_stage3"
                 << "d_total"
                 << "d_totalTraffic"
                 << "d_totalCalc";

  csv.newRow() << radius            //
               << du.d_recvParms    //
               << du.d_recvPk       //
               << du.d_setParms     //
               << du.d_setBF        //
               << du.d_sendBF       //
               << du.d_recvCips     //
               << du.d_homoCalc     //
               << du.d_sendBD       //
               << du.d_stage1       //
               << du.d_stage2       //
               << du.d_stage3       //
               << du.d_total        //
               << du.d_totalTraffic //
               << du.d_totalCalc;   //

  csv.writeToFile(filename, !makehead);
}

static void parse2csv_client_opt(const char *filename, uint64_t radius,
                                 bool makehead,
                                 std::pair<TrafficLoad, DurationClient> input) {
  CSVWriter csv(",");

  auto tf = input.first;
  auto du = input.second;
  if (makehead)
    csv.newRow() << "radius"
                 << "d_setParms"
                 << "d_sendParms"
                 << "d_kGen"
                 << "d_enc"
                 << "d_sendCip"
                 << "d_recvBF"
                 << "d_recvBD"
                 << "d_dec"
                 << "d_stage1"
                 << "d_stage2"
                 << "d_stage3"
                 << "d_total"
                 << "d_totalCalc"
                 << "d_totalTraffic" //
                 << "c_sendParms"
                 << "c_sendPk"
                 << "c_sendCips"
                 << "c_recvBF"
                 << "c_recvBD"
                 << "c_totalSend"
                 << "c_totalRecv"
                 << "c_total";

  csv.newRow() << radius            //
               << du.d_setParms     //
               << du.d_sendParms    //
               << du.d_kGen         //
               << du.d_enc          //
               << du.d_sendCip      //
               << du.d_recvBF       //
               << du.d_recvBD       //
               << du.d_dec          //
               << du.d_stage1       //
               << du.d_stage2       //
               << du.d_stage3       //
               << du.d_total        //
               << du.d_totalCalc    //
               << du.d_totalTraffic //
               << tf.c_sendParms    //
               << tf.c_sendPk       //
               << tf.c_sendCips     //
               << tf.c_recvBF       //
               << tf.c_recvBD       //
               << tf.c_totalSend    //
               << tf.c_totalRecv    //
               << tf.c_total;       //

  // if makehead, trunc; else append
  csv.writeToFile(filename, !makehead);
}

static void parse2csv_server_opt(const char *filename, uint64_t radius,
                                 bool makehead,
                                 std::pair<TrafficLoad, DurationServer> input) {
  CSVWriter csv(",");

  auto du = input.second;

  if (makehead)
    csv.newRow() << "radius"
                 << "d_recvParms"
                 << "d_setParms"
                 << "d_setBF"
                 << "d_recvCips"
                 << "d_homoCalc"
                 << "d_sendBF"
                 << "d_sendBD"
                 << "d_stage1"
                 << "d_stage2"
                 << "d_stage3"
                 << "d_total"
                 << "d_totalTraffic"
                 << "d_totalCalc";

  csv.newRow() << radius            //
               << du.d_recvParms    //
               << du.d_setParms     //
               << du.d_setBF        //
               << du.d_recvCips     //
               << du.d_homoCalc     //
               << du.d_sendBF       //
               << du.d_sendBD       //
               << du.d_stage1       //
               << du.d_stage2       //
               << du.d_stage3       //
               << du.d_total        //
               << du.d_totalTraffic //
               << du.d_totalCalc;   //

  csv.writeToFile(filename, !makehead);
}