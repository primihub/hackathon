/**
 \file 		cbitvector.h
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
 \brief		CBitVector Implementation
 */

#ifndef CBITVECTOR_H_
#define CBITVECTOR_H_

#include "typedefs.h"
#include <cassert>
#include <cstddef>

// forward declarations
class crypto;

/** Class which defines the functionality of storing C-based Bits in vector type format.*/
class CBitVector {
public:

	//Constructor code begins here...

	/** Constructor which initializes the member variables bit pointer and size to NULL and zero respectively. */
	CBitVector();

	/**
	 	 Overloaded constructor of class \link CBitVector \endlink which calls internally \link Create(std::size_t bits) \endlink
	 	 \param  bits	 - It is the number of bits which will be used to allocate the CBitVector with. For more info on how these bits are allocated refer to \link Create(std::size_t bits) \endlink
	 */
	CBitVector(std::size_t bits);

	/**
	 	Overloaded constructor of class \link CBitVector \endlink which calls internally \link Create(std::size_t bits,crypto* crypt) \endlink
	 	\param  bits	 - It is the number of bits which will be used to allocate the CBitVector with. For more info on how these bits are allocated refer to \link Create(std::size_t bits,crypto* crypt) \endlink
	 	\param  crypt 	 - This object from crypto class is used to generate pseudo random values for the cbitvector.
	 */
	CBitVector(std::size_t bits, crypto* crypt);

	//Constructor code ends here...

	//Basic Primitive function of allocation and deallocation begins here.
	/**
	 	 Function which gets called initially when the cbitvector object is created. This method is mostly called from constructor of CBitVector class.
	 	 The method sets bit pointer and size to NULL and zero respectively.
	*/
	void Init();

	/**
			Destructor which internally calls the delCBitVector for deallocating the space. This method internally calls
			\link delCBitVector() \endlink.
	*/
	~CBitVector();

	/**
		This method is used to deallocate the bit pointer and size explicitly. This method needs to be called by the programmer explicitly.
	*/
	void delCBitVector();
	//Basic Primitive function of allocation and deallocation ends here.


	//Create function supported by CBitVector starts here...
	/**
		This method generates random values and assigns it to the bitvector using crypto object. If the bits provided in the params are greater
		than the bit size of the bitvector, then the bit vector is recreated with new bit size and filled in with random values.

		\param  bits	 - It is the number of bits which will be used to allocate and assign random values of the CBitVector with. For more info on how these bits are allocated refer to \link Create(std::size_t bits) \endlink
		\param	crypt	 - It is the crypto class object which is used to generate random values for the bit size.
	*/
	void FillRand(std::size_t bits, crypto* crypt);



	/* Create in bits and bytes */
	
	/**
		This method is used to create the CBitVector with the provided bits. The method creates a bit vector of exactly ceil_divide(bits) size.
		For example, if bit size provided is 3 after this method is called it will be 8 bits = 1 byte.

		\param  bits	 - It is the number of bits which will be used to allocate the CBitVector with.
	*/
	void CreateExact(std::size_t bits);

	/**
		This method is used to create the CBitVector with the provided bits. The method creates a bit vector with a size close to AES Bitsize.
		For example, if bit size provided is 110. After this method is called it will be 128 bits. It will perform a ceil of provided_bit_size
		to AES bit size and multiply that ceiled value with AES bits size. (For reference, AES Bit size is taken as 128 bits)

		\param  bits	 - It is the number of bits which will be used to allocate the CBitVector with.
	*/
	void Create(std::size_t bits);


	/**
		This method is used to create the CBitVector with the provided byte size. The method creates a bit vector with a size close to AES Bytesize.
		For example, if byte size provided is 9. After this method is called it will be 16 bytes. It will perform a ceil of provided_byte_size
		to AES byte size and multiply that ceiled value with AES byte size. (For reference, AES Byte size is taken as 16 bytes). Internally, this method
		calls \link Create(std::size_t bits) \endlink. Therefore, for further info please refer to the internal method provided.

		\param  bytes	 - It is the number of bytes which will be used to allocate the CBitVector with.
	*/
	void CreateBytes(std::size_t bytes);

	/**
		This method is used to create the CBitVector with the provided byte size and fills it with random data from the crypt object. The method creates a
		bit vector with a size close to AES Bytesize. For example, if byte size provided is 9. After this method is called it will be 16 bytes. It will perform a ceil of provided_byte_size
		to AES byte size and multiply that ceiled value with AES byte size. (For reference, AES Byte size is taken as 16 bytes). Internally, this method
		calls \link Create(std::size_t bits, crypto* crypt) \endlink. Therefore, for further info please refer to the internal method provided.

		\param  bytes	 - It is the number of bytes which will be used to allocate the CBitVector with.
		\param  crypt	 - Reference to a crypto object from which fresh randomness is sampled
	*/
	void CreateBytes(std::size_t bytes, crypto* crypt);

	/**
		This method is used to create the CBitVector with the provided bits and set them to value zero. The method creates a bit vector with a size close to AES Bitsize.
		And performs an assignment of zero to each bit being allocated. Internally, this method calls \link Create(std::size_t bits) \endlink. Therefore, for further info
		please refer to the internal method provided.

		\param  bits	 - It is the number of bits which will be used to allocate and assign zero values of the CBitVector with.
	*/
	void CreateZeros(std::size_t bits);

	/**
		This method is used to create the CBitVector with the provided bits and set them to some random values. The method creates a bit vector with a size close to AES Bitsize.
		And performs an assignment of random values to each bit being allocated. Internally, this method calls \link Create(std::size_t bits) \endlink and
		\link FillRand(std::size_t bits, crypto* crypt) \endlink. Therefore, for further info please refer to the internal method provided.

		\param  bits	 - It is the number of bits which will be used to allocate and assign random values of the CBitVector with.
		\param	crypt	 - It is the crypto class object which is used to generate random values for the bit size.
	*/
	void Create(std::size_t bits, crypto* crypt);


	/**
		This method is used create the CBitVector with the provided number of elements and element length. This method basically creates a 1-dimensional array/vector with the provided
		element size and number of elements. This method internally calls \link Create(std::size_t bits) \endlink with arguments as elementlength*numelements.
		\param numelements		- The number of elements in the 1-dimensional array/vector which gets created.
		\param elementlength	- The size of element in the provided cbitvector.
	*/
	void Create(std::size_t numelements, std::size_t elementlength);

	/**
		This method is used create the CBitVector with the provided number of elements and element length and then assign random values to them. This method basically creates
		a 1-dimensional array/vector with the provided element size and number of elements and assign some random values based on crypt object provided. This method internally
		calls \link Create(std::size_t bits, crypto* crypt) \endlink for creation of 1-d vector.
		\param numelements		- The number of elements in the 1-dimensional array/vector which gets created.
		\param elementlength	- The size of element in the provided cbitvector.
		\param crypt			- It is the crypto class object which is used to generate random values for the provided bit size.
	*/
	void Create(std::size_t numelements, std::size_t elementlength, crypto* crypt);

	/**
		This method is used create the CBitVector with the provided number of elements of 2 dimensions and element length. This method basically creates a 2-dimensional array/vector
		with the provided element size and number of elements in two dimensions. This method internally calls \link Create(std::size_t bits) \endlink with arguments as
		elementlength*numelementsDimA*numelementsDimB.
		\param numelementsDimA		- The number of elements in the 1st-dimension of the 2d array/vector which gets created.
		\param numelementsDimB		- The number of elements in the 2nd-dimension of the 2d array/vector which gets created.
		\param elementlength		- The size of element in the provided cbitvector.
	*/
	void Create(std::size_t numelementsDimA, std::size_t numelementsDimB, std::size_t elementlength);
	/**
		This method is used create the CBitVector with the provided number of elements of 2 dimensions and element length, and then assign random values to them. This method basically
		creates a 2-dimensional array/vector with the provided element size and number of elements in two dimensions  and assign some random values based on crypt object provided.
		This method internally calls \link Create(std::size_t bits, crypto* crypt) \endlink.
		\param numelementsDimA		- The number of elements in the 1st-dimension of the 2d array/vector which gets created.
		\param numelementsDimB		- The number of elements in the 2nd-dimension of the 2d array/vector which gets created.
		\param elementlength		- The size of element in the provided cbitvector.
		\param crypt				- It is the crypto class object which is used to generate random values for the provided bit size.
	*/
	void Create(std::size_t numelementsDimA, std::size_t numelementsDimB, std::size_t elementlength, crypto* crypt);
	//Create function supported by CBitVector ends here...



	/*
	 * Management operations
	 */

	/**
		This method is used to resize the bytes allocated to CBitVector with newly provided size. And also accommodate the data from previous allocation to new one.
		\param newSizeBytes		-	This variable provides the new size to which the cbitvector needs to be modified to user's needs.
	*/
	void ResizeinBytes(std::size_t newSizeBytes);

	/**
		This method is used to reset the values in the given CBitVector. This method sets all bit values to zeros. This is a slight variant of the method
		\link CreateZeros(std::size_t bits) \endlink. The create method mentioned above allocates and sets value to zero. Whereas the provided method only
		sets the value to zero.
	*/
	void Reset();

	/**
		This method is used to reset the values in the given CBitVector for specific byte range.
		\param 	frombyte	-	The source byte position from which the values needs to be reset.
		\param 	tobyte		-	The destination byte position until which the values needs to be reset to.
	*/
	void ResetFromTo(std::size_t frombyte, std::size_t tobyte);

	/**
		This method sets all bit position values in a CBitVector to One.
	*/
	void SetToOne();

	/**
		This method sets all bits in the CBitVector to the inverse
	*/
	void Invert();


	/**
		This is a getter method which returns the size of the CBitVector in bytes.
		\return the byte size of CBitVector.
	*/
	std::size_t GetSize() const;

	/**
		This method checks if two CBitVectors are equal or not.
		\param	vec		-		Vector to be checked with current one for the case of equality.
		\return	boolean value which says whether it is equal or not.
	*/
	BOOL IsEqual(const CBitVector& vec) const;

	/**
		This method checks if two CBitVectors are equal or not for a given range of bit positions.
		\param	vec		-		Vector to be checked with current one for the case of equality.
		\param  from	-		Bit Position from which the vectors need to be checked for equality.
		\param	to	 	-		Bit Position until which the vectors need to be checked for equality.
		\return	boolean value which says whether the vectors are equal or not in the provided range of bits.
	*/
	BOOL IsEqual(const CBitVector& vec, std::size_t from, std::size_t to) const;

	/**
		This method sets the element length of the CBitVector. It can be used to modify the object size in a CBitVector when
		around with the multi dimensional arrays/vectors.
		\param	elelen	-		New element length which can be used to set the object size in a CBitVector.
	*/
	void SetElementLength(std::size_t elelen);


	/**
		This method gets the element length of the CBitVector.
		\return element length of the elements in CBitVector.
	*/
	std::size_t GetElementLength() const;

	/*
	 * Copy operations
	 */

	/**
		This method is used to copy the provided CBitVector to itself. It internally calls
		\link Copy(BYTE* p, int pos, int len) \endlink for copying bytewise.
		\param	vec		- 		The vector from which the copying needs to be performed.
	*/
	void Copy(const CBitVector& vec);

	/**
		This method is used to copy the provided CBitVector to itself for a given range. It internally calls \link Copy(BYTE* p, int pos, int len) \endlink
		for copying bytewise. Copying is done in a slightly different way. Here the range is pos and len. The offset is defined for the base vector and not
		for the copying vector. So if the method is called as B.Copy(A,5,10) then, values of vector A will be copied from first index location for length 10
		to the vector B from position 5 for length 10. Unlike copying values from 5 position in vector A to vector B for length 10.
		\param	vec		- 		The vector from which the copying needs to be performed.
		\param	pos		- 		The positional offset for copying into current vector.
		\param	len		-		Length or amount of values to be copied to the current vector from provided vector.
	*/
	void Copy(const CBitVector& vec, std::size_t pos, std::size_t len);

	/**
		This method is used to copy the current CBitVector with some ByteLocation with positional shift and length. This method is the base method for methods
		\link Copy(CBitVector& vec, int pos, int len) \endlink and \link Copy(CBitVector& vec) \endlink.
		\param	p		-		Pointer to the byte location to be copied to the CBitVector.
		\param	pos		-		Positional offset for copying into current CBitVector.
		\param	len		-  		Length or amount of values to be copied to the current vector from provided byte location.
	*/
	void Copy(const BYTE* p, std::size_t pos, std::size_t len);

	/**
		This method performs OR operation bytewise with the current CBitVector at the provided byte position with another Byte object.
		\param	pos		- 		Byte position in the CBitVector which is used to perform OR operation with.
		\param	p		-		Byte with which the OR operation is performed to get the result.

	*/
	void ORByte(std::size_t pos, BYTE p);

	/*
	 * Bitwise operations
	 */

	/**
		This method gets the bit in the provided index by using the maskbits. The maskbits brings the concept of
		endianness in the vector. In this method MASK_BIT is used to  extract the bits which are assumed to be
		organized in Little Endian form.
		\param	idx		-		Bit Index which needs to be fetched from the CBitVector.
		\return The byte which has got just the bit in it.
	*/
	BYTE GetBit(std::size_t idx) const;

	/**
		This method sets the bit in the provided index by using the maskbits and the provided bit. The maskbits brings the concept of
		endianness in the vector. In this method C_MASK_BIT is used to figure out the bits which are assumed to be
		organized in Little Endian form.
		\param	idx		-		Bit Index which needs to be written to in the CBitVector.
		\param	b		-		The bit which being written in the provided index.
	*/
	void SetBit(std::size_t idx, BYTE b);

	/**
		This method gets the bit in the provided index without using the maskbits. The maskbits brings the concept of
		endianness in the vector. In this method mask bits are not used so the vector is treated in Big Endian form.
		\param	idx		-		Bit Index which needs to be fetched from the CBitVector.
		\return The byte which has got just the bit in it.
	*/
	BYTE GetBitNoMask(std::size_t idx) const;

	/**
		This method sets the bit in the provided index without using the maskbits. The maskbits brings the concept of
		endianness in the vector. In this method mask bits are not used so the vector is treated in Big Endian form.
		\param	idx		-		Bit Index which needs to be written to in the CBitVector.
		\param	b		-		The bit which being written in the provided index.
	*/
	void SetBitNoMask(std::size_t idx, BYTE b);

	/**
		This method XORs the bit in the provided index without using the maskbits. The maskbits brings the concept of
		endianness in the vector. In this method mask bits are not used so the vector is treated in Big Endian form.
		\param	idx		-		Bit Index which needs to be XORed to in the CBitVector.
		\param	b		-		The bit which being XORed in the provided index.
	*/
	void XORBitNoMask(std::size_t idx, BYTE b);

	/*
	 * Single byte operations
	 */

	/**
		This method sets a byte in a given index of the CBitVector with the provided Byte.
		\param	idx		-	Index where the byte needs to be set.
		\param	p		-	Byte which needs to be copied to.
	*/
	void SetByte(std::size_t idx, BYTE p);

	/**
		This method returns the byte at the given index in the CBitVector. Here the index is w.r.t bytes.
		\param	idx		-	Index of the byte which needs to be returned from the CBitVector.
		\return Byte is returned from CBitVector at the given index.
	*/
	BYTE GetByte(std::size_t idx) const;

	/**
		Not Used Currently in Framework.
		This method performs XOR operation at the given index in the CBitVector with a provided Byte.
		\param	idx		-	Index of the byte which needs to be XORed inside the CBitVector.
		\param	b		- 	Byte to be XORed with the CBitVector.
	*/
	void XORByte(std::size_t idx, BYTE b);

	/**
		This method performs AND operation at the given index in the CBitVector with a provided Byte.
		\param	idx		-	Index of the byte which needs to be ANDed inside the CBitVector.
		\param	b		- 	Byte to be ANDed with the CBitVector.
	*/
	void ANDByte(std::size_t idx, BYTE b);

	/*
	 * Get Operations
	 */

	/**
		This method gets elements from the CBitVector bitwise from a given offset for a given length. And stores the result
		in the provided byte pointer. This method is used by the generic method \link Get(int pos, int len) \endlink
		\param	p		-	The resulting bits for the given range in the CBitVector is stored in the byte pointer p.
		\param	pos		-	The positional offset in the CBitVector from which the data needs to obtained.
		\param	len		- 	The range limit of obtaining the data from the CBitVector.
	*/
	void GetBits(BYTE* p, std::size_t pos, std::size_t len) const;

	/**
		This method gets elements from the CBitVector bytewise from a given offset for a given length. And stores the result
		in the provided byte pointer.
		\param	p		-	The resulting bits for the given range in the CBitVector is stored in the byte pointer p.
		\param	pos		-	The positional offset in the CBitVector from which the data needs to obtained.
		\param	len		- 	The range limit of obtaining the data from the CBitVector.
	*/
	void GetBytes(BYTE* p, std::size_t pos, std::size_t len) const;

	/**
		Generic method which performs the operation of getting values from a CBitVector for a given bit position and length.
		This method internally calls \link GetBits(BYTE* p, int pos, int len) \endlink.
		\param	pos		-	The positional offset in the CBitVector from which the data needs to obtained.
		\param	len		- 	The range limit of obtaining the data from the CBitVector.
		\return	returns the value/values for the provided range.
	*/
	template<class T> T Get(std::size_t pos, std::size_t len) const {
		assert(len <= sizeof(T) * 8);
		T val = 0;
		GetBits((BYTE*) &val, pos, len);
		return val;
	}

	/*
	 * Set Operations
	 */
	/**
		The method for setting CBitVector for a given bit range with offset and length in unsigned 64bit integer format. This method
		is called from \link SetBits(BYTE* p, int pos, int len) \endlink and \link Set(T val, int pos, int len) \endlink.
		\param	p		-	Byte array passed to be set to the current CBitVector.
		\param	pos		-	Positional offset in the CBitVector, where data will be set from the provided byte array.
		\param	len		-   The range limit of obtaining the data from the CBitVector.
	*/
	void SetBits(const BYTE* p, std::size_t pos, std::size_t len);

	/**
		The method for setting CBitVector for a given bit range with offsets and length with another Byte Array.
		\param	p		-	Byte array passed to be set with the current CBitVector.
		\param	ppos	-	Positional offset in the Byte Array.
		\param	pos		-	Positional offset in the CBitVector, where data will be set from the provided byte array.
		\param	len		-	The range limit of obtaining the data from the CBitVector.
	 */
	void SetBitsPosOffset(const BYTE* p, std::size_t ppos, std::size_t pos, std::size_t len);

	/**
		The method for setting CBitVector for a given byte range with offset and length. This method internally calls the method
		\link SetBytes(T* dst, T* src, T* lim) \endlink.
		\param	src		-	Byte array passed to be set to the current CBitVector.
		\param	pos		-	Byte position offset in the CBitVector, where data will be set from the provided byte array.
		\param	len		-   The number of bytes to be set.
	*/
	void SetBytes(const BYTE* src, std::size_t pos, std::size_t len);

	/**
		This method sets the values in a given byte range to Zero in the current CBitVector.
		\param	bytepos		-	Byte Positional offset in the CBitVector.
		\param	bytelen		-	Byte Length in the CBitVector until which the value needs to be set to zero.
	*/
	void SetBytesToZero(std::size_t bytepos, std::size_t bytelen);

	/**
		Generic method which performs the operation of setting values to a CBitVector for a given bit position and length.
		This method internally calls \link SetBits(BYTE* p, std::size_t pos, std::size_t len) \endlink.
		\param	pos		-	The positional offset in the CBitVector from which the data needs to obtained.
		\param	len		- 	The range limit of obtaining the data from the CBitVector.
	*/
	template<class T> void Set(T val, std::size_t pos, std::size_t len) {
		assert(len <= sizeof(T) * 8);
		SetBits((BYTE*) &val, pos, len);
	}

	/**
		This method sets the values in a given bit range to Zero in the current CBitVector.
		\param	bitpos		-	Bit Positional offset in the CBitVector.
		\param	bitlen		-	Bit Length in the CBitVector until which the value needs to be set to zero.
	*/
	void SetBitsToZero(std::size_t bitpos, std::size_t bitlen);

	/*
	 * XOR Operations
	 */

	/**
		This method performs XOR operation from a given position in the CBitVector with a provided Byte Array with a length.
		This method is called from \link XORBytes(BYTE* p, int len) \endlink. This method internally calls \link XORBytes(T* dst, T* src, T* lim) \endlink.
		\param	p		- 		Byte Array to be XORed with the CBitVector range.
		\param	pos		-		Positional offset for XORing into current CBitVector.
		\param	len		-  		Length or amount of values to be XORed to the current vector from provided byte location.
	*/
	void XORBytes(const BYTE* p, std::size_t pos, std::size_t len);
	/**
		This method performs XOR operation for a given length in the CBitVector with a provided Byte Array.	This method internally calls
		\link XORBytes(BYTE* p, int pos, int len) \endlink.
		\param	p		- 		Byte Array to be XORed with the CBitVector range.
		\param	len		-  		Length or amount of values to be XORed to the current vector from provided byte location.
	*/
	void XORBytes(const BYTE* p, std::size_t len);

	/**
	 	Not Used in the Framework.
		This method performs XOR operation from a given position in the CBitVector with another CBitVector with a length.
		This method internally calls \link XORBytes(BYTE* p, int pos, int len) \endlink.
		\param	vec		- 		Provided Array to be XORed with the CBitVector.
		\param	pos		-		Positional offset for XORing into current CBitVector.
		\param	len		-  		Length or amount of values to be XORed to the current vector from provided byte location.
	*/
	void XORVector(const CBitVector &vec, std::size_t pos, std::size_t len);

	/**
		Generic method which is used to XOR bit wise the CBitVector. This method internally calls
		\link XORBits(BYTE* p, int pos, int len) \endlink.
	*/
	template<class T> void XOR(T val, std::size_t pos, std::size_t len) {
		assert(len <= sizeof(T) * 8);
		XORBits((BYTE*) &val, pos, len);
	}

	/**
		The method for XORing CBitVector for a given bit range with offset and length. This method is called from
		\link XOR(T val, int pos, int len) \endlink.
		\param	p		-	Byte array passed to be XORed with the current CBitVector.
		\param	pos		-	Positional offset in the CBitVector, where data will be XORed from the provided byte array.
		\param	len		-   The range limit of obtaining the data from the CBitVector.
	*/
	void XORBits(const BYTE* p, std::size_t pos, std::size_t len);

	/**
		The method for XORing CBitVector for a given bit range with offsets and length with another Byte Array.
		\param	p		-	Byte array passed to be XORed with the current CBitVector.
		\param	ppos	-	Positional offset in the Byte Array.
		\param	pos		-	Positional offset in the CBitVector, where data will be XORed from the provided byte array.
		\param	len		-   The range limit of obtaining the data from the CBitVector.
	*/
	void XORBitsPosOffset(const BYTE* p, std::size_t ppos, std::size_t pos, std::size_t len);

	/**
		Set the value of this CBitVector to this XOR b
		\param	b		-	Pointer to a CBitVector which is XORed on this CBitVector
	*/
	void XOR(const CBitVector* b);

	/**
		This method performs XOR operation from a given position in the CBitVector with a provided Byte Array with a length.
		The XORing is performed in a slightly different way. The byte array is reversed before it is XORed with the CBitVector.
		This method is called from \link XORBytes(BYTE* p, int len) \endlink. This method internally calls \link XORBytes(T* dst, T* src, T* lim) \endlink.
		\param	p		- 		Byte Array to be XORed with the CBitVector range.
		\param	pos		-		Positional offset for XORing into current CBitVector.
		\param	len		-  		Length or amount of values to be XORed to the current vector from provided byte location.
	*/
	void XORBytesReverse(const BYTE* p, std::size_t pos, std::size_t len);

	/*
	 * AND Operations
	 */

	/**
		This method performs AND operation from a given position in the CBitVector with a provided Byte Array with a length.
		This method internally calls \link ANDBytes(T* dst, T* src, T* lim) \endlink.
		\param	p		- 		Byte Array to be ANDed with the CBitVector range.
		\param	pos		-		Positional offset for ANDing into current CBitVector.
		\param	len		-  		Length or amount of values to be ANDed to the current vector from provided byte location.
	*/
	void ANDBytes(const BYTE* p, std::size_t pos, std::size_t len);

	/*
	 * Set operations
	 */
	/**
		This method is used to set and XOR a CBitVector with a byte array and then XOR it with another byte array
		for a given range. This method internally calls \link Copy(BYTE* p, int pos, int len) \endlink and
		\link XORBytes(BYTE* p, int pos, int len) \endlink.
		\param	p		-		Pointer to the byte location to be copied to the CBitVector.
		\param 	q		-		Pointer to the byte location with which the CBitVector is XORed with.
		\param	pos		-		Positional offset for copying and XORing into current CBitVector.
		\param	len		-  		Length or amount of values to be copied and XORed to the current vector from provided byte location.
	*/
	void SetXOR(const BYTE* p, const BYTE* q, std::size_t pos, std::size_t len);

	/**
		This method is used to set and AND a CBitVector with a byte array and then AND it with another byte array
		for a given range. This method internally calls \link Copy(BYTE* p, int pos, int len) \endlink and
		\link ANDBytes(BYTE* p, int pos, int len) \endlink.
		\param	p		-		Pointer to the byte location to be copied to the CBitVector.
		\param 	q		-		Pointer to the byte location with which the CBitVector is ANDed with.
		\param	pos		-		Positional offset for copying and ANDing into current CBitVector.
		\param	len		-  		Length or amount of values to be copied and ANDed to the current vector from provided byte location.
	*/
	void SetAND(const BYTE* p, const BYTE* q, std::size_t pos, std::size_t len);

	/**
		Set the value of this CBitVector to this AND b
		\param	b		-	Pointer to a CBitVector which is ANDed on this CBitVector
	*/
	void AND(const CBitVector* b);

	/**
		Cyclic shift left by pos positions
		\param	pos		-	the left shift value
	*/
	void CLShift(std::size_t pos);


	/*
	 * Buffer access operations
	 */

	/**
		This method returns CBitVector in byte array format. This is very widely used method.
	*/
	BYTE* GetArr();
	const BYTE* GetArr() const;

	/**
		This method is used to attach a new buffer into the CBitVector provided as arguments to this method.
		\param	p		-		Pointer to the byte location to be attached to the CBitVector.
		\param  size	-		Number of bytes attached from the provided buffer.
	*/
	void AttachBuf(BYTE* p, std::size_t size = 0);

	/**
		This method is used to detach the buffer from the CBitVector. */
	void DetachBuf();

	/*
	 * Print Operations
	 */

	/**
		This method prints the CBitVector bitwise for provided bit range. This method internally calls \link  GetBitNoMask(int idx) \endlink.
		This method is called from \link PrintBinary() \endlink.
		\param	fromBit			-		The bit from which the printing starts in a CBitVector.
		\param	toBit			-		The bit until which the printing in a CBitVector is done.
	*/
	void Print(std::size_t fromBit, std::size_t toBit);

	/**
		This method prints the CBitVector in Hexadecimal format.
	*/
	void PrintHex(bool linebreak = true);

	/**
		This method prints the CBitVector in Hexadecimal format for the provided byte range.
		\param	fromByte		-		The byte from which the printing of CBitVector begins.
		\param	toByte			-		The byte until which the printing of CBitVector is done.
	*/
	void PrintHex(std::size_t fromByte, std::size_t toByte, bool linebreak = true);

	/**
		This method prints the CBitVector in Binary format. This method internally calls \link Print(int fromBit, int toBit) \endlink.
	*/
	void PrintBinary();

	/**
		This method is a more abstract printing method which is used to print the CBitVector even if the vector is a simple 1 bit based
		vector or 1-d array/vector or even a 2-d vector/array. This method internally calls methods \link Get(int i) \endlink and
		\link Get2D(int i, int j) \endlink.
	*/
	void PrintContent();

	/**
		This method prints the CBitVector bitwise for provided bit range with mask. This method internally calls \link  GetBit(int idx) \endlink.
		\param	fromBit			-		The bit from which the printing starts in a CBitVector.
		\param	toBit			-		The bit until which the printing in a CBitVector is done.
	*/
	void PrintBinaryMasked(std::size_t from, std::size_t to);

	/*
	 * If the cbitvector is abstracted to an array of elements with m_nElementLength bits size, these methods can be used for easier access
	 */

	/**
		Generic method which provides more abstraction for getting elements in the CBitVector. It is mainly used for getting values which are
		1-dimensional in nature. This method internally calls \link Get(int pos, int len) \endlink.
		\param	i		-		Index from which data needs to be fetched.
	*/
	template<class T> T Get(std::size_t i) const {
		return Get<T>(i * m_nElementLength, m_nElementLength);
	}
	/**
		Generic method which provides more abstraction for setting elements in the CBitVector. It is mainly used for getting values which are
		1-dimensional in nature. This method internally calls \link Set(int pos, int len) \endlink.
		\param	val		-		Value which needs to be written to the given location.
		\param	i		-		Index to which data needs to be written to.
	*/
	template<class T> void Set(T val, std::size_t i) {
		Set<T>(val, i * m_nElementLength, m_nElementLength);
	}
	/*
	 * The same as the above methods only for two-dimensional access
	 */
	/**
		Generic method which provides more abstraction for getting elements in the CBitVector. It is mainly used for getting values which are
		2-dimensional in nature. This method internally calls \link Get(int pos, int len) \endlink.
		\param	i		-		Row index from which the data needs to be read.
		\param	j		-		Column index from which the data needs to be read.
	*/
	template<class T> T Get2D(std::size_t i, std::size_t j) const {
		return Get<T>((i * m_nNumElementsDimB + j) * m_nElementLength, m_nElementLength);
	}

	/**
		Generic method which provides more abstraction for setting elements in the CBitVector. It is mainly used for getting values which are
		2-dimensional in nature. This method internally calls \link Set(int pos, int len) \endlink.
		\param	val		-		Value which needs to be written to the given location.
		\param	i		-		Row index from which the data needs to be written.
		\param	j		-		Column index from which the data needs to be written.
	*/
	template<class T> void Set2D(T val, std::size_t i, std::size_t j) {
		Set<T>(val, (i * m_nNumElementsDimB + j) * m_nElementLength, m_nElementLength);
	}
	//useful when accessing elements using an index

	//View the cbitvector as a rows x columns matrix and transpose
	void Transpose(std::size_t rows, std::size_t columns);
	void SimpleTranspose(std::size_t rows, std::size_t columns);
	void EklundhBitTranspose(std::size_t rows, std::size_t columns);

private:
	BYTE* m_pBits;	/** Byte pointer which stores the CBitVector as simple byte array. */
	std::size_t m_nByteSize; /** Byte size variable which stores the size of CBitVector in bytes. */
	std::size_t m_nBits; //The exact number of bits
	std::size_t m_nElementLength; /** Size of elements in the CBitVector. By default, it is set to 1. It is used
	 	 	 	 	 	 	 	   differently when it is used as 1-d or 2-d custom vector/array. */
	std::size_t m_nNumElements;  /** Number elements in the first dimension in the CBitVector. */
	std::size_t m_nNumElementsDimB;/** Number elements in the second dimension in the CBitVector. */
};

#endif /* BITVECTOR_H_ */
