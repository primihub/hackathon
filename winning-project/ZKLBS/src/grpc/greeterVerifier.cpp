#include "greeterVerifier.h"

void GreeterVerifier::RunVerifier2Client()
{
    std::string server_address("0.0.0.0:50052");
    GreeterServiceImpl service;
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Verifier listening to Client on " << server_address << std::endl;
    server->Wait();
}

void GreeterVerifier::RunVerifier2Server()
{
    std::string verifier_address("0.0.0.0:50053");
    GreeterVerifierImpl service;
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;

    builder.AddListeningPort(verifier_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Verifier listening to Server on " << verifier_address << std::endl;
    server->Wait();
}

int main()
{
    GreeterVerifier greeterVerifier;
    std::thread v2c(greeterVerifier.RunVerifier2Client);
    v2c.detach();
    std::thread v2s(greeterVerifier.RunVerifier2Server);
    v2s.detach();
    getchar();
    return 0;
}