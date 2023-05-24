#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <Client2Verifier.grpc.pb.h>
#include <Server2Verifier.grpc.pb.h>
#include "../circuit/PoseidonServer.cpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using Client2Verifier::CVGreeter;
using Client2Verifier::CVReply;
using Client2Verifier::CVRequest;

using Server2Verifier::SVGreeter;
using Server2Verifier::SVReply;
using Server2Verifier::SVRequest;

std::string h;
std::string randh;
bool inUse = false;

class GreeterServiceImpl final : public CVGreeter::Service
{
    Status CVCommunication(ServerContext *context, const CVRequest *request,
                           CVReply *reply) override
    {
        if(inUse)
        {
            PoseidonServer poseidonServer2;
            auto rand_verify = poseidonServer2.verify(request->randvk(), request->randproof(), randh);
            std::cout << "From verifier rand_verify: " << rand_verify << std::endl;
            if (!rand_verify)
            {
                reply->set_answer("Rand Verify Fail, Please Contact Service");
            }
            else
            {
                std::cout << "From verifier VerifyRand: Rand verify Success" << std::endl;
            }

            std::cout << "From client proof: " << request->proof() << std::endl;
            std::cout << "From client vk: " << request->vk() << std::endl;
            PoseidonServer poseidonServer;
            std::cout << "From verifier h: " << h << std::endl;
            auto if_verify = poseidonServer.verify(request->vk(), request->proof(), h);
            std::cout << "From verifier if_verify: " << if_verify << std::endl;
            if (if_verify)
                reply->set_answer("Verify Success");
            else
                reply->set_answer("Verify Fail");
            inUse = false;
            return Status::OK;
        }
        else
        {
            return Status::OK;
        }
    }
};

class GreeterVerifierImpl final : public SVGreeter::Service
{
    Status SVCommunication(ServerContext *context, const SVRequest *request,
                           SVReply *reply) override
    {
        if(inUse)
        {
            return Status::OK;
        }
        else
        {
            std::cout << "From server h: " << request->h() << std::endl;
            h = request->h();
            randh = request->randh();
            inUse = true;
            return Status::OK;
        }
    }
};

class GreeterVerifier
{
public:
    static void RunVerifier2Client();
    static void RunVerifier2Server();
};
