# core-data-coding
TLDR: Library containing snippets/classes for encoding/decoding/various data transformations. Eg. [Manchester code](https://en.wikipedia.org/wiki/Manchester_code).

## The context
Data transformation is required in many areas, encryption, RF transmission, etc.

## The problem
Having code for specific types of encoding/decoding is sometimes needed for certain types of projects like those that use RF transmission where some coding schemes have properties suited for the expected transmission medium. An example being a low frequency RF data transmission (125khz) over the air that requires a [self clocking](https://en.wikipedia.org/wiki/Self-clocking_signal) data stream. A single frequency means you can't send a clock and a data line at the same time; your clock and data have to be encoded in the same stream. The fact that the transmission medium is very 'analog' (waves flying through the air), the scheme also has to be robust against environmental factors (timing inaccuracies, etc). In one implementation, I used [Differential Manchester](https://en.wikipedia.org/wiki/Differential_Manchester_encoding) code. More details in the example section below.

## The solution
A library with a collection of encoding/decoding classes conforming to a simple templated base class.

## The details
The templated base class takes these template parameters:
```c++
template<typename ARRAY_INDEX_TYPE, typename INPUT_PRIMITIVE_TYPE, typename OUTPUT_PRIMITIVE_TYPE>
```
ARRAY_INDEX_TYPE allows you to select the max array size, especially useful in embedded environments where you don't need a stream of a billion input primitives. The input and output primitive types are self explanatory; they let you define what goes in and what comes out of the encoder/decoder. In fact, an encoder and decoder are same from that perspective; something goes in, something comes out. As a result, the base class is just called IDataCoder. No distinction between encoding and decoding:
```c++
template<typename ARRAY_INDEX_TYPE, typename INPUT_PRIMITIVE_TYPE, typename OUTPUT_PRIMITIVE_TYPE>
class IDataCoder {
public:
  virtual ~IDataCoder() {}

  virtual void Code(const INPUT_PRIMITIVE_TYPE input[], ARRAY_INDEX_TYPE inLength, OUTPUT_PRIMITIVE_TYPE output[]) const = 0;
};
```

## The example
I really like [Differential Manchester](https://en.wikipedia.org/wiki/Differential_Manchester_encoding) code. It's a nice simple code with some very interesting properties. Similar to [Manchester code](https://en.wikipedia.org/wiki/Manchester_code), it:
* Is [self clocking](https://en.wikipedia.org/wiki/Self-clocking_signal) (meaning it doesn't need an independent data and clock line)
* Has a DC bias of zero which is important for communications systems (also see [line codes](https://en.wikipedia.org/wiki/Line_code))

One of the properties about Differential Manchester that I liked over regular Manchester is that the first edge is always a non-data carrying edge. The gist of the scheme is that every period boundary must have an edge and in the middle of each period, a data bit can be encoded as either the existence or non-existence of an edge. Here's an example:
```
Data            :    1   0   1   1   0   1   0   1
Line signal     :  |_|¯|___|¯|_|¯|_|¯¯¯|_|¯|___|¯|_|
Period edges    :  ^   ^   ^   ^   ^   ^   ^   ^   ^
Diff Manchester :  0 1 0 0 1 0 1 0 1 1 0 1 0 0 1 0
```
Notice how every period edge is just that, an edge. The data bits are encoded as the existence or the non-existence of an edge.  In this case, a 1 is an edge and a 0 is the lack thereof. Since the first edge of a Differential Manchester stream is a period edge, it can be used as the kickoff interrupt for a microcontroller to start decoding. With Manchester, the first edge may be a period edge or a data edge.

I won't go through it here, but if you work through the state machine of a Differential Manchester graph, you will always start in the same place when you get the first edge. For a regular Manchester decoding session, you don't know up front which state to start in (which by definition means you need more states to represent when you're in that unknown-ness).

Here's the code that decodes the Differential Manchester code:

include/data/coding/manchester/differential/decoder.h
```c++
#pragma once

#include <data/coding/common/coder.h>

template<typename ARRAY_INDEX_TYPE, typename DATA_PRIMITIVE_TYPE>
class CManchesterDifferentialDecoder :
	public IDataCoder<ARRAY_INDEX_TYPE, DATA_PRIMITIVE_TYPE, DATA_PRIMITIVE_TYPE> {
public:
	CManchesterDifferentialDecoder();
	virtual ~CManchesterDifferentialDecoder();

	// IDataDecoder

	virtual void Code(const DATA_PRIMITIVE_TYPE input[], ARRAY_INDEX_TYPE inLength, DATA_PRIMITIVE_TYPE output[]) const;
};
```

src/data/coding/manchester/differential/decoder.cpp
```c++
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
			// _BS sets a bit in a value. First param is the value, second param the bit index, third param the bit value
			_BS(
				output[outputPrimitiveIndex],
				outputBit,
				_BC(input[i], inputBit + 1) != _BC(input[i], inputBit));
		}
	}
}

/* FORWARD DECLARED TEMPLATE COMBOS */
template class CManchesterDifferentialDecoder<uint8_t, uint8_t>;
```

Note that the class templates were propogated down from the parent class because we don't want to mandate the max length of the data stream and we also want to make maximum use of the optimally performant primitive for a particular architecture (eg. AVR8 vs x86). Disclaimer: I haven't actually done any tests yet on, say, if using 8-bit primitives like uint8_t on an x86 machine is inefficient compated its CPU 32-bit bitwidth primitive like uint32_t. Could end up being the same for all I know and only thorough testing with someone who understands the nuances of the architecture plus compiler specifics will be able to provide a complete answer.
