#include "ethsnarks.hpp"
#include "utils.hpp"
#include "stubs.hpp"
#include "gadgets/poseidon.hpp"
#include "import.hpp"
#include "export.hpp"

#include <utility>
#include <sstream>

using ethsnarks::make_var_array;
using ethsnarks::Poseidon128;
using ethsnarks::ppT;
using ethsnarks::proof_from_json;
using ethsnarks::ProtoboardT;
using ethsnarks::vk_from_json;

class PoseidonServer
{
public:
    bool verify(std::string vk_json, std::string proof_json, std::string server_proof_json);
};