#include "PoseidonServer.h"

bool PoseidonServer::verify(std::string vk_json, std::string proof_json, std::string server_proof_json)
{
    ppT::init_public_params();
    std::stringstream vk_stream;
    vk_stream << vk_json;
    auto vk = vk_from_json(vk_stream);

    std::stringstream proof_stream;
    proof_stream << proof_json;
    auto proof_pair = proof_from_json(proof_stream);

    std::stringstream server_proof_stream;
    server_proof_stream << server_proof_json;
    auto server_proof_pair = proof_from_json(server_proof_stream);

    return libsnark::r1cs_gg_ppzksnark_zok_verifier_strong_IC<ppT>(vk, server_proof_pair.first,
                                                                   proof_pair.second);
}
