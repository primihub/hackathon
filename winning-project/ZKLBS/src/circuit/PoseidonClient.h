#include "ethsnarks.hpp"
#include "utils.hpp"
#include "gadgets/poseidon.hpp"
#include "stubs.hpp"
#include "import.hpp"

#include <iostream>
#include <utility>

using ethsnarks::make_var_array;
using ethsnarks::Poseidon128;
using ethsnarks::ppT;
using ethsnarks::ProtoboardT;
using ethsnarks::vk2json;

class PoseidonClient
{
public:
    std::string getVkJson();
    std::string getProofJson();
    void prove(std::pair<long int, long int> coordinate,
               int time, int pk, int rand, int n);
    void randProve(int rand);

private:
    int time;
    std::pair<long int, long int> coordinate;

    std::string vk_json;
    std::string proof_json;
};