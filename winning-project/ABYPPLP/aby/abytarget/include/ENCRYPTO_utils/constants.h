/**
 \file 		constants.h
 \author	michael.zohner@ec-spride.de, daniel.demmler@crisp-da.de
\copyright	ABY - A Framework for Efficient Mixed-protocol Secure Two-party Computation
			Copyright (C) 2019 ENCRYPTO Group, TU Darmstadt
			This program is free software: you can redistribute it and/or modify
            it under the terms of the GNU Lesser General Public License as published
            by the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.
            ABY is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
            GNU Lesser General Public License for more details.
            You should have received a copy of the GNU Lesser General Public License
            along with this program. If not, see <http://www.gnu.org/licenses/>.
 \brief		File containing all crypto and networking constants used throughout the source
 */

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "typedefs.h"
#include <cstdint>
#include "cmake_constants.h"

#define BATCH
//#define FIXED_KEY_AES_HASHING
//#define USE_PIPELINED_AES_NI
//#define SIMPLE_TRANSPOSE //activate the simple transpose, only required for benchmarking, not recommended

#define AES_KEY_BITS			128
#define AES_KEY_BYTES			16
#define AES_BITS				128
#define AES_BYTES				16
#define LOG2_AES_BITS			ceil_log2(AES_BITS)

#define SHA1_OUT_BYTES 20
#define SHA256_OUT_BYTES 32
#define SHA512_OUT_BYTES 64

#define MAX_NUM_COMM_CHANNELS 256
#define ADMIN_CHANNEL MAX_NUM_COMM_CHANNELS-1

enum field_type {P_FIELD, ECC_FIELD, FIELD_LAST};

static const seclvl ST = { 40, 80, 1024 };
static const seclvl MT = { 40, 112, 2048 };
static const seclvl LT = { 40, 128, 3072 };
static const seclvl XLT = { 40, 192, 7680 };
static const seclvl XXLT = { 40, 256, 15360 };

const uint8_t m_vFixedKeyAESSeed[AES_KEY_BYTES] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
/** \var m_vSeed
 \brief Static seed for various testing functionalities
 */
const uint8_t m_vSeed[AES_KEY_BYTES] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

inline const char* getFieldType(field_type ftype) {
	switch (ftype) {
	case P_FIELD: return "P_FIELD";
	case ECC_FIELD: return "ECC_FIELD";
	default: return "unknown field";
	}
}
#endif /* _CONSTANTS_H_ */


