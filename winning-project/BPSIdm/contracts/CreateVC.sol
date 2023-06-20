// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract CreateVC {
    struct VerifiableCredential {
        string id;
        string issuer;
        string subject;
        uint256 issuedDate;
        mapping (string => string) claims;
        bytes32[] proof;
    }

    mapping (string => VerifiableCredential) public verifiableCredentials;

    function createVerifiableCredential(
        string memory id,
        string memory issuer,
        string memory subject,
        uint256 issuedDate,
        string[] memory claimKeys,
        string[] memory claimValues
    ) public {
        require(verifiableCredentials[id].issuedDate == 0, "Verifiable Credential already exists");

        VerifiableCredential storage vc = verifiableCredentials[id];
        vc.id = id;
        vc.issuer = issuer;
        vc.subject = subject;
        vc.issuedDate = issuedDate;

        for (uint i = 0; i < claimKeys.length; i++) {
            vc.claims[claimKeys[i]] = claimValues[i];
        }

        // Generate and store proof (e.g., cryptographic signatures) for the verifiable credential
        bytes32 proof = generateProof(id);
        vc.proof.push(proof);
    }

    function generateProof(string memory id) private pure returns (bytes32) {
        // Perform cryptographic operations to generate proof for the verifiable credential
        // This could include signing the credential with a private key or using other secure methods
        bytes32 proof = keccak256(abi.encodePacked(id));
        return proof;
    }
}

