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

#pragma once

// TODO HACK: Remove all references to double underscores or identifiers starting with an underscore followed by an uppercase letter

#include <stdint.h>

// one hot bit value
#define _OH(bit) (1 << (bit))

// create bit mask
#define _BM(offset, length) (((1 << length) - 1) << offset)

// create bit range
#define _BR(lsbOffset, length, value) ((value << lsbOffset) & _BM(lsbOffset, length))

// check bit
#define _BC(target, bit) ((target & _OH(bit)) >> (bit))

// check bit range
#define _BCR(target, lsbOffset, length) ((target & (_BM(lsbOffset, length))) >> lsbOffset)

// set bit
#define _BS(target, bit, setHigh) (setHigh ? (target |= (1 << (bit))) : (target &= ~(1 << (bit))))

// set bit range
#define _BSR(target, lsbOffset, length, value) (target = ((target & ~_BM(lsbOffset, length)) | _BR(lsbOffset, length, value)))


// binary related types

template <typename POINTER_TYPE>
struct BIT_POINTER {
	volatile POINTER_TYPE* address;
	uint8_t offset;
};

template <typename POINTER_TYPE>
struct BIT_RANGE {
	volatile POINTER_TYPE* address;
	uint8_t lsb_offset;
	uint8_t length;
};

// helper functions

template <typename TYPE>
uint8_t __binary_parity_even(TYPE data) {
	uint8_t parityEven = 0;
	for (uint8_t i = 0; i < (sizeof(data) * 8); i++) {
		parityEven ^= _BC(data, i);
	}
	return parityEven;
}

template <typename TYPE>
uint8_t __binary_parity_odd(TYPE data) {
	return !__binary_parity_even(data);
}