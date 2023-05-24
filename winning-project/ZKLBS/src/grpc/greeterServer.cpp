#include "greeterServer.h"

GreeterServer::GreeterServer(std::shared_ptr<Channel> channel)
{
    svStub_ = SVGreeter::NewStub(channel);
}
std::string GreeterServer::server_to_verifier(const std::string h, const std::string randh)
{
    SVRequest request;
    request.set_h(h);
    request.set_randh(randh);

    SVReply reply;
    ClientContext context;

    Status status = svStub_->SVCommunication(&context, request, &reply);

    if (status.ok())
    {
        return "RPC SUCCESS";
    }
    else
    {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return "RPC failed";
    }
}

void GreeterServer::RunServer2Client()
{
    std::string server_address("0.0.0.0:50051");
    GreeterServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char **argv)
{
    GreeterServer greeterServer;
    greeterServer.RunServer2Client();
    return 0;
}