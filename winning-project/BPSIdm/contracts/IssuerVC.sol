// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract IssuerVC {
    struct Share {
        uint8 index;
        uint256 value;
    }

    struct Secret {
        uint8 threshold;
        uint8 numShares;
        uint256 secret;
        mapping(uint8 => Share) shares;
    }

    mapping(bytes32 => Secret) private secrets;

    event SecretShared(bytes32 id, uint8 threshold, uint8 numShares);

    function createSecret(bytes32 id, uint256 secret, uint8 n, uint8 k) public {
        require(secrets[id].secret == 0, "Secret already created");
        require(n >= k, "Invalid number of shares or threshold");

        Secret storage secretObj = secrets[id];
        secretObj.threshold = k;
        secretObj.numShares = n;
        secretObj.secret = secret;

        emit SecretShared(id, k, n);
    }

    function getShare(bytes32 id, uint8 index) public view returns (uint8, uint256) {
        Secret storage secret = secrets[id];
        require(secret.secret != 0, "Secret not found");
        require(index >= 1 && index <= secret.numShares, "Invalid share index");
        Share storage share = secret.shares[index];
        return (share.index, share.value);
    }

    function distributeShares(bytes32 id, Share[] memory shares) public {
        Secret storage secret = secrets[id];
        require(secret.secret != 0, "Secret not found");
        require(shares.length == secret.numShares, "Invalid number of shares");

        for (uint8 i = 0; i < shares.length; i++) {
            require(shares[i].index >= 1 && shares[i].index <= secret.numShares, "Invalid share index");
            secret.shares[shares[i].index] = Share(shares[i].index, shares[i].value);
        }
    }

    function reconstructSecret(bytes32 id, uint8[] memory shareIndexes) public view returns (uint256) {
        Secret storage secret = secrets[id];
        require(secret.secret != 0, "Secret not found");
        require(shareIndexes.length >= secret.threshold, "Insufficient shares provided");

        uint8 k = secret.threshold;
        uint256 result = 0;

        for (uint8 i = 0; i < k; i++) {
            require(shareIndexes[i] >= 1 && shareIndexes[i] <= secret.numShares, "Invalid share index");
            result += secret.shares[shareIndexes[i]].value;
        }

        return result;
    }
}


