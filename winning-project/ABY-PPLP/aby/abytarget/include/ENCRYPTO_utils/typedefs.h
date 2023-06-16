/**
 \file 		typedefs.h
 \author 	michael.zohner@ec-spride.de
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
 \brief		Typedefs Implementation
 */

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <cstdint>

typedef int BOOL;
typedef unsigned char BYTE;

typedef uint64_t UGATE_T;
typedef uint64_t REGISTER_SIZE;

typedef struct SECURITYLEVELS {
	uint32_t statbits;
	uint32_t symbits;
	uint32_t ifcbits;
	//ECCLVL depends on CMAKE settings
} seclvl;

//ECCLVL depends on CMAKE settings

#define GATE_T_BITS (sizeof(UGATE_T) * 8)

typedef REGISTER_SIZE REGSIZE;
#define LOG2_REGISTER_SIZE		ceil_log2(sizeof(REGISTER_SIZE) << 3)

#define FILL_BYTES				AES_BYTES
#define FILL_BITS				AES_BITS

#define RETRY_CONNECT		1000
#define CONNECT_TIMEO_MILISEC	10000

#define SNDVALS 2

#define OTEXT_BLOCK_SIZE_BITS	AES_BITS
#define OTEXT_BLOCK_SIZE_BYTES	AES_BYTES

#define VECTOR_INTERNAL_SIZE 8

#define	SERVER_ID	0
#define	CLIENT_ID	1

template<class T>
T rem(T a, T b) {
	return ((a) > 0) ? (a) % (b) : (a) % (b) + ((b) > 0 ? (b) : (b) * -1);
}
template<class T>
T sub(T a, T b, T m) {
	return ((b) > (a)) ? (a) + (m) - (b) : (a) - (b);
}
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#define ZERO_BYTE 0
#define MAX_BYTE 0xFF

#endif //__TYPEDEFS_H__
