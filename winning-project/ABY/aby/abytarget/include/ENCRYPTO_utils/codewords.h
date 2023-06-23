/**
 \file 		codewords.h
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
 \brief		A hard-coded Welsh-Hadamard code for up to 8-bit words and resulting 256-bit codewords
 */

#ifndef __CODEWORDS_H_
#define __CODEWORDS_H_

#include <cstdint>
#include <cstdlib>

const uint32_t m_nCodewords = 256;
const uint32_t m_nCWIntlen = 8;

const uint32_t m_nCodeWordBits = 256;
const uint32_t m_nCodeWordBytes = m_nCodeWordBits/8;

extern const uint32_t CODE_MATRIX[m_nCodewords][m_nCWIntlen];

void readCodeWords(uint64_t** codewords);
void InitAndReadCodeWord(uint64_t*** codewords);

#endif //CODEWORDS_H_
