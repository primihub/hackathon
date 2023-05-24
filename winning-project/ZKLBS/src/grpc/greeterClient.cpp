#include "greeterClient.h"

GreeterClient::GreeterClient(std::shared_ptr<Channel> channel)
{
    stub_ = CSGreeter::NewStub(channel);
    cvStub_ = CVGreeter::NewStub(channel);
}
std::string GreeterClient::client_to_server(const std::string pk,
                                            const std::string latitude,
                                            const std::string longitude)
{
    CSRequest request;
    request.set_pk(pk);
    request.set_latitude(latitude);
    request.set_longitude(longitude);

    CSReply reply;
    ClientContext context;

    Status status = stub_->CSCommunication(&context, request, &reply);

    if (status.ok())
    {
        std::cout << "From server rand: " << reply.rand() << std::endl;
        std::cout << "From server time: " << reply.time() << std::endl;
        sRand = reply.rand();
        sTime = reply.time();
        std::cout << "From client sRand:" << sRand << std::endl;
        return "RPC SUCCESS";
    }
    else
    {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return "RPC failed";
    }
}
std::string GreeterClient::client_to_verifier(const std::string vk, const std::string proof,
                                              const std::string randVk, const std::string randProof)
{
    CVRequest request;
    request.set_vk(vk);
    request.set_proof(proof);
    request.set_randvk(randVk);
    request.set_randproof(randProof);

    CVReply reply;
    ClientContext context;

    Status status = cvStub_->CVCommunication(&context, request, &reply);

    if (status.ok())
    {
        std::cout << "From verifier answer: " << reply.answer() << std::endl;
        return "RPC SUCCESS";
    }
    else
    {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return "RPC failed";
    }
}

std::string GreeterClient::getsRand()
{
    return sRand;
}

std::string GreeterClient::getsTime()
{
    return sTime;
}


void GreeterClient::RunClient()
{
    std::string server_address = "localhost:50051";
    GreeterClient greeter(
        grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

    CoordinateUtil coordinateUtil;
    std::pair<long int, long int> coordinate = coordinateUtil.getCoordinate();
    std::cout << "From client coordinate:" << coordinate.first << " " << coordinate.second << std::endl;
    TypeUtil typeUtil;
    RandomUtil randomUtil;

    std::string pk(randomUtil.generateRandomIntString(5));
    std::string lat(typeUtil.Int2String(coordinate.first));
    std::string lon(typeUtil.Int2String(coordinate.second));

    std::string reply = greeter.client_to_server(pk, lat, lon);
    std::cout << "From server reply:" << reply << std::endl;

    int iPk = typeUtil.String2Int(pk);

    int iRand = typeUtil.String2Int(greeter.getsRand());
    int iTime = typeUtil.String2Int(greeter.getsTime());
    std::cout << "From client iPk: " << iPk << std::endl;
    std::cout << "From client iLatitude: " << coordinate.first << std::endl;
    std::cout << "From client iLongitude: " << coordinate.second << std::endl;
    std::cout << "From client iRand: " << iRand << std::endl;
    std::cout << "From client iTime: " << iTime << std::endl;

    PoseidonClient poseidonClient;
    poseidonClient.prove(coordinate, iTime, iPk, iRand, 2);

    PoseidonClient poseidonClient2;
    poseidonClient2.randProve(iRand);

    std::string verifier_address = "localhost:50052";
    GreeterClient cvGreeter(
        grpc::CreateChannel(verifier_address, grpc::InsecureChannelCredentials()));
    std::string vk(poseidonClient.getVkJson());
    std::string proof(poseidonClient.getProofJson());
    std::string randvk(poseidonClient2.getVkJson());
    std::string randproof(poseidonClient2.getProofJson());
    std::string vReply = cvGreeter.client_to_verifier(vk, proof, randvk, randproof);
    std::cout << "From verifier reply:" << vReply << std::endl;
}

std::vector<std::string> GreeterClient::RunClient2(std::string pk, std::string lat, std::string lon)
{
    std::string server_address = "localhost:50051";
    GreeterClient greeter(
        grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));
    
    std::string reply = greeter.client_to_server(pk, lat, lon);

    TypeUtil typeUtil;
    int iPk = typeUtil.String2Int(pk);
    int iRand = typeUtil.String2Int(greeter.getsRand());
    int iTime = typeUtil.String2Int(greeter.getsTime());
    int ilat = typeUtil.String2Int(lat);
    int ilon = typeUtil.String2Int(lon);
    std::pair<long int, long int> coordinate = std::make_pair(ilat, ilon);

    PoseidonClient poseidonClient;
    poseidonClient.prove(coordinate, iTime, iPk, iRand, 2);

    PoseidonClient poseidonClient2;
    poseidonClient2.randProve(iRand);

    std::string verifier_address = "localhost:50052";
    GreeterClient cvGreeter(
        grpc::CreateChannel(verifier_address, grpc::InsecureChannelCredentials()));
    std::string vk(poseidonClient.getVkJson());
    std::string proof(poseidonClient.getProofJson());
    std::string randvk(poseidonClient2.getVkJson());
    std::string randproof(poseidonClient2.getProofJson());
    std::string vReply = cvGreeter.client_to_verifier(vk, proof, randvk, randproof);
    std::cout << "From verifier reply:" << vReply << std::endl;
    
    std::vector<std::string> result;
    result.push_back(vReply);
    result.push_back(greeter.getsRand());
    result.push_back(greeter.getsTime());
    return result;
}

// int main()
// {
//     GreeterClient greeterClient;
//     greeterClient.RunClient();
//     return 0;
// }