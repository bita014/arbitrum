/*
 * Copyright 2020, Offchain Labs, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef test_ecops_hpp
#define test_ecops_hpp

#include <gmpxx.h>
#include <avm/machinestate/ecops.hpp>
#include <boost/algorithm/hex.hpp>
#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>

using namespace libff;

inline std::vector<unsigned char> hexToVec(const std::string& hexstr) {
    std::vector<unsigned char> bytes;
    bytes.resize(hexstr.size() / 2);
    boost::algorithm::unhex(hexstr.begin(), hexstr.end(), bytes.begin());
    return bytes;
}

inline uint256_t hexToInt(const std::string& hexstr) {
    auto bytes = hexToVec(hexstr);
    return intx::be::unsafe::load<uint256_t>(bytes.data());
}

struct PairingTestCase {
    G1Point a;
    G2Point b;
    G1Point c;
    G2Point d;

    PairingTestCase(const G1<alt_bn128_pp>& a_,
                    const G2<alt_bn128_pp>& b_,
                    const G1<alt_bn128_pp>& c_,
                    const G2<alt_bn128_pp>& d_)
        : a(toArbPoint(a_)),
          b(toArbPoint(b_)),
          c(toArbPoint(c_)),
          d(toArbPoint(d_)) {}

    PairingTestCase(const std::string& data);
};

std::vector<PairingTestCase> preparePairingCases();

struct ECAddTestCase {
    G1Point a;
    G1Point b;
    G1Point res;
};

std::vector<ECAddTestCase> prepareECAddCases();

struct ECMulTestCase {
    G1Point a;
    uint256_t k;
    G1Point res;
};

inline std::vector<ECMulTestCase> prepareECMulCases() {
    alt_bn128_pp::init_public_params();

    G1<alt_bn128_pp> Pff =
        (Fr<alt_bn128_pp>::random_element()) * G1<alt_bn128_pp>::one();
    uint256_t sui = hexToInt(
        "b7abaaf2f45b6d1c1b23afb835719050a28b98cea191d94bff8feb3025ddbfc8");

    uint8_t sbytes[32];
    intx::be::store(sbytes, sui);
    mpz_t smpz;
    mpz_init(smpz);
    mpz_import(smpz, 32, 1, 1, 1, 0, sbytes);
    bigint<BIG_INT_FOR_UINT256> s(smpz);

    auto P = toArbPoint(Pff);

    G1Point prod = toArbPoint(s * Pff);
    return {{P, sui, prod}};
}

#endif /* test_ecops_hpp */
