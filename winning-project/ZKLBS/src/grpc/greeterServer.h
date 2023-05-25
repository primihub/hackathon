#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <Client2Server.grpc.pb.h>
#include <Server2Verifier.grpc.pb.h>
#include "../circuit/PoseidonClient.h"
#include "../circuit/PoseidonServer.h"
#include "../utils/TimeUtil.h"
#include "../utils/RandomUtil.h"
#include "../utils/TypeUtil.h"

using grpc::Channel;
using grpc::ClientContext;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using Client2Server::CSGreeter;
using Client2Server::CSReply;
using Client2Server::CSRequest;

using Server2Verifier::SVGreeter;
using Server2Verifier::SVReply;
using Server2Verifier::SVRequest;

class GreeterServer
{
public:
    GreeterServer() {}
    GreeterServer(std::shared_ptr<Channel> channel);
    std::string server_to_verifier(const std::string h, const std::string randh);
    void RunServer2Client();

private:
    std::unique_ptr<SVGreeter::Stub> svStub_;
};

class GreeterServiceImpl final : public CSGreeter::Service
{
    void RunServer2Verifier(std::string proof, std::string randProof)
    {
        std::string verifier_address = "localhost:50053";
        GreeterServer greeter(
            grpc::CreateChannel(verifier_address, grpc::InsecureChannelCredentials()));
        std::string h(proof);
        std::string randh(randProof);
        std::cout << "From server h:" << h << std::endl;
        std::string reply = greeter.server_to_verifier(h, randh);
        std::cout << "From verifier pk:" << reply << std::endl;
    }
    Status CSCommunication(ServerContext *context, const CSRequest *request,
                           CSReply *reply) override
    {
        std::cout << "From client pk: " << request->pk() << std::endl;
        std::cout << "From client latitude: " << request->latitude() << std::endl;
        std::cout << "From client longitude: " << request->longitude() << std::endl;

        RandomUtil randomUtil;
        TimeUtil timeUtil;
        timeUtil.init();
        TypeUtil typeUtil;
        std::string sRand = randomUtil.generateRandomIntString(8);

        int iPk = typeUtil.String2Int(request->pk());
        int iLatitude = typeUtil.String2Int(request->latitude());
        int iLongitude = typeUtil.String2Int(request->longitude());
        std::pair<long int, long int> coordinate = std::make_pair(iLatitude, iLongitude);
        int iRand = typeUtil.String2Int(sRand);
        int iTime = timeUtil.getHour();
        reply->set_rand(sRand);
        reply->set_time(typeUtil.Int2String(iTime));
        PoseidonClient poseidonClient;
        PoseidonClient poseidonClient2;

        std::cout << "From server iPk: " << iPk << std::endl;
        std::cout << "From server iLatitude: " << iLatitude << std::endl;
        std::cout << "From server iLongitude: " << iLongitude << std::endl;
        std::cout << "From server iRand: " << iRand << std::endl;
        std::cout << "From server iTime: " << iTime << std::endl;
        poseidonClient.prove(coordinate, iTime, iPk, iRand, 2);
        poseidonClient2.randProve(iRand);
        RunServer2Verifier(poseidonClient.getProofJson(), poseidonClient2.getProofJson());
        return Status::OK;
    }
};