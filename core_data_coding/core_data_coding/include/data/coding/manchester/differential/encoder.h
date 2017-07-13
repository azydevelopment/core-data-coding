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

#include <data/coding/common/coder.h>

template<typename ARRAY_INDEX_TYPE, typename DATA_PRIMITIVE_TYPE>
class CManchesterDifferentialEncoder :
	public IDataCoder<ARRAY_INDEX_TYPE, DATA_PRIMITIVE_TYPE, DATA_PRIMITIVE_TYPE> {
public:
	CManchesterDifferentialEncoder();
	virtual ~CManchesterDifferentialEncoder();

	// IDataDecoder

	virtual void Code(const DATA_PRIMITIVE_TYPE input[], ARRAY_INDEX_TYPE inLength, DATA_PRIMITIVE_TYPE output[]) const;
};
