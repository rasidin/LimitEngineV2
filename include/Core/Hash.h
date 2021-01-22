/*********************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file Hash.h
@brief Hash utils
@author minseob
**********************************************************************/
#ifndef LIMITENGINEV2_CORE_HASH_H_
#define LIMITENGINEV2_CORE_HASH_H_

#ifndef USE_SSE_CRC32
#define USE_SSE_CRC32 0
#endif // USE_SSE_CRC32

#if USE_SSE_CRC32
#include <nmmintrin.h>
#pragma intrinsic(_mm_crc32_u64)
#else // Use FNV-1
static constexpr uint64 FNV_OFFSET_BASIS_64 = 14695981039346656037U;
static constexpr uint64 FNV_PRIME_64 = 1099511628211LLU;
#endif

namespace LimitEngine {
class Hash { public:
    static uint64 GenerateHash(const uint64* Data, size_t Size)
    {
        uint64 output = 0u;
#if USE_SSE_CRC32
        // Alignment check
        LEASSERT(reinterpret_cast<uint64>(Data) % 8ull == 0ull && Size % 8ull == 0ull);
        for (uint64 idx = 0; idx < Size / 8ull; idx++) {
            output = _mm_crc32_u64(output, Data[idx]);
        }
#else
        output = FNV_OFFSET_BASIS_64;
        const uint8* data8 = reinterpret_cast<const uint8*>(Data);
        for (size_t dtidx = 0; dtidx < Size; dtidx++, data8++) {
            output = (FNV_PRIME_64 * output) ^ (*data8);
        }
#endif
        return output;
    }
};
}

#endif // LIMITENGINEV2_CORE_HASH_H_