// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

library Pairing {
    struct G1Point {
        uint X;
        uint Y;
    }
    // Encoding of field elements is: X[0] * z + X[1]
    struct G2Point {
        uint[2] X;
        uint[2] Y;
    }
    /// @return the generator of G1
    function P1() pure internal returns (G1Point memory) {
        return G1Point(1, 2);
    }
    /// @return the generator of G2
    function P2() pure internal returns (G2Point memory) {
        return G2Point(
            [10857046999023057135944570762232829481370756359578518086990519993285655852781,
             11559732032986387107991004021392285783925812861821192530917403151452391805634],
            [8495653923123431417604973247489272438418190587263600148770280649306958101930,
             4082367875863433681332203403145435568316851327593401208105741076214120093531]
        );
    }
    /// @return the negation of p, i.e. p.addition(p.negate()) should be zero.
    function negate(G1Point memory p) pure internal returns (G1Point memory) {
        // The prime q in the base field F_q for G1
        uint q = 21888242871839275222246405745257275088696311157297823662689037894645226208583;
        if (p.X == 0 && p.Y == 0)
            return G1Point(0, 0);
        return G1Point(p.X, q - (p.Y % q));
    }
    /// @return r the sum of two points of G1
    function addition(G1Point memory p1, G1Point memory p2) internal view returns (G1Point memory r) {
        uint[4] memory input;
        input[0] = p1.X;
        input[1] = p1.Y;
        input[2] = p2.X;
        input[3] = p2.Y;
        bool success;
        assembly {
            success := staticcall(sub(gas(), 2000), 6, input, 0xc0, r, 0x60)
            // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        require(success);
    }


    /// @return r the product of a point on G1 and a scalar, i.e.
    /// p == p.scalar_mul(1) and p.addition(p) == p.scalar_mul(2) for all points p.
    function scalar_mul(G1Point memory p, uint s) internal view returns (G1Point memory r) {
        uint[3] memory input;
        input[0] = p.X;
        input[1] = p.Y;
        input[2] = s;
        bool success;
        assembly {
            success := staticcall(sub(gas(), 2000), 7, input, 0x80, r, 0x60)
            // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        require (success);
    }
    /// @return the result of computing the pairing check
    /// e(p1[0], p2[0]) *  .... * e(p1[n], p2[n]) == 1
    /// For example pairing([P1(), P1().negate()], [P2(), P2()]) should
    /// return true.
    function pairing(G1Point[] memory p1, G2Point[] memory p2) internal view returns (bool) {
        require(p1.length == p2.length);
        uint elements = p1.length;
        uint inputSize = elements * 6;
        uint[] memory input = new uint[](inputSize);
        for (uint i = 0; i < elements; i++)
        {
            input[i * 6 + 0] = p1[i].X;
            input[i * 6 + 1] = p1[i].Y;
            input[i * 6 + 2] = p2[i].X[1];
            input[i * 6 + 3] = p2[i].X[0];
            input[i * 6 + 4] = p2[i].Y[1];
            input[i * 6 + 5] = p2[i].Y[0];
        }
        uint[1] memory out;
        bool success;
        assembly {
            success := staticcall(sub(gas(), 2000), 8, add(input, 0x20), mul(inputSize, 0x20), out, 0x20)
            // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        require(success);
        return out[0] != 0;
    }
    /// Convenience method for a pairing check for two pairs.
    function pairingProd2(G1Point memory a1, G2Point memory a2, G1Point memory b1, G2Point memory b2) internal view returns (bool) {
        G1Point[] memory p1 = new G1Point[](2);
        G2Point[] memory p2 = new G2Point[](2);
        p1[0] = a1;
        p1[1] = b1;
        p2[0] = a2;
        p2[1] = b2;
        return pairing(p1, p2);
    }
    /// Convenience method for a pairing check for three pairs.
    function pairingProd3(
            G1Point memory a1, G2Point memory a2,
            G1Point memory b1, G2Point memory b2,
            G1Point memory c1, G2Point memory c2
    ) internal view returns (bool) {
        G1Point[] memory p1 = new G1Point[](3);
        G2Point[] memory p2 = new G2Point[](3);
        p1[0] = a1;
        p1[1] = b1;
        p1[2] = c1;
        p2[0] = a2;
        p2[1] = b2;
        p2[2] = c2;
        return pairing(p1, p2);
    }
    /// Convenience method for a pairing check for four pairs.
    function pairingProd4(
            G1Point memory a1, G2Point memory a2,
            G1Point memory b1, G2Point memory b2,
            G1Point memory c1, G2Point memory c2,
            G1Point memory d1, G2Point memory d2
    ) internal view returns (bool) {
        G1Point[] memory p1 = new G1Point[](4);
        G2Point[] memory p2 = new G2Point[](4);
        p1[0] = a1;
        p1[1] = b1;
        p1[2] = c1;
        p1[3] = d1;
        p2[0] = a2;
        p2[1] = b2;
        p2[2] = c2;
        p2[3] = d2;
        return pairing(p1, p2);
    }
}

contract ZKPVerifier {
    using Pairing for *;
    struct VerifyingKey {
        Pairing.G1Point alpha;
        Pairing.G2Point beta;
        Pairing.G2Point gamma;
        Pairing.G2Point delta;
        Pairing.G1Point[] gamma_abc;
    }
    struct Proof {
        Pairing.G1Point a;
        Pairing.G2Point b;
        Pairing.G1Point c;
    }
    function verifyingKey() pure internal returns (VerifyingKey memory vk) {
        vk.alpha = Pairing.G1Point(uint256(0x19eff1557b79f23ff6ee607d3815647194285898f7938e441c1b86935a68b4dd), uint256(0x24f0973f016bc1692ee245992f15dd98fc495df55cb16e82f9c04d856a4d8efc));
        vk.beta = Pairing.G2Point([uint256(0x09ff943f4551e8297c37edd2ae22f88369e74bf0dd1f790f2862018d40e0981d), uint256(0x07fb809c50fec215d6951ffbf529f0e6df1acab6f5d6881b25307f89bbecf3cf)], [uint256(0x1dc588be9ca14b595e85bc5223b6fcd7d0c38dc59fbbdd5b54718061fc2e691e), uint256(0x04a0afd1586a94b4624d471fb458ba484057041a2c819ed8a50e19b4be1b4189)]);
        vk.gamma = Pairing.G2Point([uint256(0x0ce502386934cde6d6382820ddb8f8fd057951a13fa2345f854a5a87d8285b61), uint256(0x1dac6157668604f329182660e7bcc4b5456b72766e694a564132a965ea6ecffd)], [uint256(0x10f4b397fff44a05e07f7c2c1238d59ddcde5e3b62e275792b05dc23e6a76487), uint256(0x0e474bd8465e7dbde48f0c8d1023fecf3a4d356eae32284f22e35ab273cbec4c)]);
        vk.delta = Pairing.G2Point([uint256(0x242c3a08ee0654e80c55aaaedbdcc357b373089ba98d8d01c37412292dfc4873), uint256(0x2ce8bdb2ac1eb1c37a2eaf0a7a53c3cf8ba379990302cc4bb4fdb1605ebaac17)], [uint256(0x0ea0953a4663f22b9795d632c823b85c35e8cf678cf64f66b5a4f74c5b75104e), uint256(0x0bbc4467271b4930ea6cbf2246ca5ffbd6f283bf0dc14425c74c90c04e546d69)]);
        vk.gamma_abc = new Pairing.G1Point[](8);
        vk.gamma_abc[0] = Pairing.G1Point(uint256(0x2f5992dbf9d446be2a041308bb8c603ef02b3e93914ca931b62718e5441703f0), uint256(0x1dbe23a2d7419b416f30ced3f41751dfe78dc0d3f64381935d7dc7785e21dd19));
        vk.gamma_abc[1] = Pairing.G1Point(uint256(0x068209c31898ddd2af5bd6746c461e11c74ad329e053a0ea9af32443085b417e), uint256(0x1a1a52d89f55a7cacd6a13784dfd6f16882739ef5b2c6d58aadb6a01e68c2432));
        vk.gamma_abc[2] = Pairing.G1Point(uint256(0x299cd66c8955d063e587d58e42ebc6a906c1975e303771c466ae1db79cad6d16), uint256(0x061c69024e7e2883e08d5d525731559a7f3003628727f98eaa15eb7ace170bdd));
        vk.gamma_abc[3] = Pairing.G1Point(uint256(0x07caf84720ff297d42eadde6e24a6ef03cc5a61055a0b38d79f6f49091b8b3cd), uint256(0x27e9d13ef13d6c886c5be48f1dde408d0deac8634b144bc9129155d35e5f0cc7));
        vk.gamma_abc[4] = Pairing.G1Point(uint256(0x0c9cfad153411f842efa954d50f26571733873c728d8c0270a4cb6d90f93a5cc), uint256(0x14c993adfd1e0dc7f1abc2bab67fd22f071dfd1cebca1ae6824d05b6385f4034));
        vk.gamma_abc[5] = Pairing.G1Point(uint256(0x0532d5717ccb18f2c0d7c9b0717317608019df0821da47905e43aaf9d83665fc), uint256(0x0f64ee4de840143f0c0f6e8619048a77419aaf7786612d1b64748aea9b01c462));
        vk.gamma_abc[6] = Pairing.G1Point(uint256(0x0a66af4081123d5828fcfd2ab119eb7bf0af988e644b4ea47ddeaefe0c4ac55f), uint256(0x1b65051be178dbf1c86e84e5c02b27be7dc217b155b7069b6155eac62228e889));
        vk.gamma_abc[7] = Pairing.G1Point(uint256(0x3048ca67a5d1d22e32935b9f823d1fe602261734b64c8a4b11a285bf2d36bfb2), uint256(0x24d5053595d154a075ced75b196782663b2421270e6eb83792adce06d6374626));
    }
    function verify(uint[] memory input, Proof memory proof) internal view returns (uint) {
        uint256 snark_scalar_field = 21888242871839275222246405745257275088548364400416034343698204186575808495617;
        VerifyingKey memory vk = verifyingKey();
        require(input.length + 1 == vk.gamma_abc.length);
        // Compute the linear combination vk_x
        Pairing.G1Point memory vk_x = Pairing.G1Point(0, 0);
        for (uint i = 0; i < input.length; i++) {
            require(input[i] < snark_scalar_field);
            vk_x = Pairing.addition(vk_x, Pairing.scalar_mul(vk.gamma_abc[i + 1], input[i]));
        }
        vk_x = Pairing.addition(vk_x, vk.gamma_abc[0]);
        if(!Pairing.pairingProd4(
             proof.a, proof.b,
             Pairing.negate(vk_x), vk.gamma,
             Pairing.negate(proof.c), vk.delta,
             Pairing.negate(vk.alpha), vk.beta)) return 1;
        return 0;
    }
    function verifyTx(
            Proof memory proof, uint[7] memory input
        ) public view returns (bool r) {
        uint[] memory inputValues = new uint[](7);
        
        for(uint i = 0; i < input.length; i++){
            inputValues[i] = input[i];
        }
        if (verify(inputValues, proof) == 0) {
            return true;
        } else {
            return false;
        }
    }
}

