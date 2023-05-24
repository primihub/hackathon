#include "../circuit/PoseidonClient.cpp"
#include "../circuit/PoseidonServer.cpp"

void test_prove(PoseidonClient &poseidonClient)
{
    poseidonClient.randProve(1);
    std::cout << "From test vkjson: " << poseidonClient.getVkJson() << std::endl;
    std::cout << "From test proofjson: " << poseidonClient.getProofJson() << std::endl;
}

void test_verify(PoseidonServer &poseidonServer, std::string vk, std::string proof, std::string server_proof)
{
    auto if_verify = poseidonServer.verify(2, vk, proof, server_proof);
    std::cout << if_verify << std::endl;
}

int main()
{
    PoseidonServer poseidonServer;
    PoseidonClient poseidonClient;
    PoseidonClient poseidonClient2;
    test_prove(poseidonClient);
    test_prove(poseidonClient2);
    std::cout << poseidonClient.getVkJson() << std::endl;
    std::cout << poseidonClient2.getProofJson() << std::endl;
    test_verify(poseidonServer, poseidonClient.getVkJson(), poseidonClient.getProofJson(), poseidonClient2.getProofJson());
    return 0;
}