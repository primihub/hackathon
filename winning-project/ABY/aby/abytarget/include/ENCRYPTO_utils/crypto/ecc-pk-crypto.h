/**
 \file 		ecc-pk-crypto.h
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
 \brief		Class with ECC operations
 */

#ifndef ECC_PK_CRYPTO_H_
#define ECC_PK_CRYPTO_H_

#include "pk-crypto.h"

extern "C"
{
	#include <relic.h>
}

#include <memory>
#include <vector>
#include <mutex>

class ecc_num;
class ecc_fe;
class ecc_brickexp;

static std::mutex relic_mutex;

class ecc_field: public pk_crypto {
public:
	ecc_field(seclvl sp, uint8_t* seed) :
			pk_crypto(sp) {
		init(sp, seed);
	};
	~ecc_field();

	num* get_num();
	num* get_rnd_num(uint32_t bitlen = 0);
	fe* get_fe();
	fe* get_rnd_fe();
	fe* get_generator();
	fe* get_rnd_generator();
	uint32_t get_size();
	//fe* sample_fe_from_bytes(uint8_t* buf, uint32_t bytelen);
	num* get_order();
	uint32_t num_byte_size();
	uint32_t get_field_size();

	brickexp* get_brick(fe* gen);

	ctx_t* get_context();

protected:
	void init(seclvl sp, uint8_t* seed);
private:
	fe* sample_random_point();
	ecc_fe* generator;
	ctx_t* context;
};

class ecc_num: public num {

public:
	ecc_num(ecc_field* fld);
	ecc_num(ecc_field* fld, bn_t src);
	~ecc_num();
	void set(num* src);
	void set_si(int32_t src);
	void set_add(num* a, num* b);
	void set_sub(num* a, num* b);
	void set_mul(num* a, num* b);
	void mod(num* mod);
	void set_mul_mod(num* a, num* b, num* modulus) ;

	void get_val(bn_t res);

	void export_to_bytes(uint8_t* buf, uint32_t field_size_bytes);
	void import_from_bytes(uint8_t* buf, uint32_t field_size_bytes);
	//void set_rnd(uint32_t bits); Seems useless since not implemented
	void print();

private:
	void shallow_copy(bn_t to, bn_t from);

	bn_t val;
	ecc_field* field;
	ctx_t* context;
};

class ecc_fe: public fe {
public:
	ecc_fe(ecc_field* fld);
	ecc_fe(ecc_field* fld, eb_t src);
	~ecc_fe();
	void set(fe* src);
	void get_val(eb_t res);
	void set_mul(fe* a, fe* b);

	void set_pow(fe* b, num* e);
	void set_div(fe* a, fe* b);
	void set_double_pow_mul(fe* b1, num* e1, fe* b2, num* e2);
	void export_to_bytes(uint8_t* buf);
	void import_from_bytes(uint8_t* buf);
	void sample_fe_from_bytes(uint8_t* buf, uint32_t bytelen);
	bool eq(fe* a);

	void print();

private:
	void init();
	void shallow_copy(eb_t to, eb_t from);
	eb_t val;
	ecc_field* field;
	ctx_t* context;
};

class ecc_brickexp: public brickexp {
public:
	ecc_brickexp(fe* generator, ecc_field* field);
	~ecc_brickexp();

	void pow(fe* res, num* e);
private:
	uint32_t eb_pre_size;
	eb_t* eb_table;
	ecc_field* field;
	ctx_t* context;
};


#endif /* ECC_PK_CRYPTO_H_ */
