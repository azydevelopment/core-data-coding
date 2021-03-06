/* MIT License
 * Copyright(c) 2017 Andrew Yeung <azy.development@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#include <data/coding/manchester/differential/decoder.h>

#include <util/binary.h>

/* PUBLIC */

template<typename ARRAY_INDEX_TYPE, typename DATA_PRIMITIVE_TYPE>
CManchesterDifferentialDecoder<ARRAY_INDEX_TYPE, DATA_PRIMITIVE_TYPE>::CManchesterDifferentialDecoder() :
IDataCoder<ARRAY_INDEX_TYPE, DATA_PRIMITIVE_TYPE, DATA_PRIMITIVE_TYPE>() {}

template<typename ARRAY_INDEX_TYPE, typename DATA_PRIMITIVE_TYPE>
CManchesterDifferentialDecoder<ARRAY_INDEX_TYPE, DATA_PRIMITIVE_TYPE>::~CManchesterDifferentialDecoder() {}

// IDataDecoder

template<typename ARRAY_INDEX_TYPE, typename DATA_PRIMITIVE_TYPE>
void CManchesterDifferentialDecoder<ARRAY_INDEX_TYPE, DATA_PRIMITIVE_TYPE>::Code(const DATA_PRIMITIVE_TYPE input[], ARRAY_INDEX_TYPE inLength, DATA_PRIMITIVE_TYPE output[]) const {
	// TODO ERROR_HANDLING: Make sure output is big enough somehow

	// TODO HACK: Assuming no datatype larger than 256 bits
	uint8_t numDataPrimitiveBits = sizeof(DATA_PRIMITIVE_TYPE) * 8;
	uint8_t maxBitIndex = numDataPrimitiveBits - 1;

	// encode the data
	for (ARRAY_INDEX_TYPE i = 0; i < inLength; i++) {
		// iterate over every two bits since this is decoded using edges (requires before and after information)
		// TODO HACK: Assuming no datatype larger than 256 bits
		for (uint8_t j = 0; j < numDataPrimitiveBits; j += 2) {
			uint8_t inputBit = numDataPrimitiveBits - 2 - j;

			ARRAY_INDEX_TYPE outputPrimitiveIndex = i / 2;
			uint8_t outputBit = maxBitIndex - ((i % 2) * numDataPrimitiveBits + j) / 2;

			// TODO IMPLEMENT: Ability to change polarity of the decoder
			_BS(
				output[outputPrimitiveIndex],
				outputBit,
				_BC(input[i], inputBit + 1) != _BC(input[i], inputBit));
		}
	}
}

/* FORWARD DECLARED TEMPLATE COMBOS */
template class CManchesterDifferentialDecoder<uint8_t, uint8_t>;