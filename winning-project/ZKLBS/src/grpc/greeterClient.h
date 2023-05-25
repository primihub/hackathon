#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "Client2Server.grpc.pb.h"
#include "Client2Verifier.grpc.pb.h"
#include "../circuit/PoseidonClient.h"
#include "../circuit/PoseidonServer.h"
#include "../utils/TimeUtil.h"
#include "../utils/CoordinateUtil.h"
#include "../utils/TypeUtil.h"
#include "../utils/RandomUtil.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using Client2Server::CSGreeter;
using Client2Server::CSReply;
using Client2Server::CSRequest;

using Client2Verifier::CVGreeter;
using Client2Verifier::CVReply;
using Client2Verifier::CVRequest;

class GreeterClient
{
public:
    GreeterClient(){}
    GreeterClient(std::shared_ptr<Channel> channel);
    std::string client_to_server(const std::string pk,
                                 const std::string latitude,
                                 const std::string longitude);
    std::string client_to_verifier(const std::string vk, const std::string proof,
                                   const std::string randVk, const std::string randProof);
    std::string getsRand();
    std::string getsTime();
    void RunClient();

    std::vector<std::string> RunClient2(std::string pk, std::string lat, std::string lon);
private:
    std::unique_ptr<CSGreeter::Stub> stub_;
    std::unique_ptr<CVGreeter::Stub> cvStub_;

    std::string sRand = "";
    std::string sTime = "";
};