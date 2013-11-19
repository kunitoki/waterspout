/*
 * waterspout
 *
 *   - simd abstraction library for audio/image manipulation -
 *
 * Copyright (c) 2013 Lucio Asnaghi
 *
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_AVX_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_AVX_H__


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific AVX math class elaborating on __m256 buffers
 */

class math_avx : public math_sse42
{
public:
    math_avx()
    {
        //assertfalse; // not implemented !
    }

    const char* name() { return "AVX"; }
    
    void copy_buffer(float* srcBuffer, float* dstBuffer, uint32_t size)
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)srcBuffer & 0x0F);

        if (size < WATERSPOUT_MIN_SSE_SAMPLES ||
              ((ptrdiff_t)dstBuffer & 0x0F) != align_bytes)
        {
            for (int i = 0; i < size; ++i)
            {
              dstBuffer[i] = srcBuffer[i];
            }
        } 
        else 
        { 
            assert(size >= 8);

            // Copy unaligned head
            switch (align_bytes >> 3)
            {
            case 1: --size; *dstBuffer++ = *srcBuffer++;
            case 2: --size; *dstBuffer++ = *srcBuffer++;
            case 3: --size; *dstBuffer++ = *srcBuffer++;
            case 4: --size; *dstBuffer++ = *srcBuffer++;
            case 5: --size; *dstBuffer++ = *srcBuffer++;
            case 6: --size; *dstBuffer++ = *srcBuffer++;
            case 7: --size; *dstBuffer++ = *srcBuffer++;
            }  

            // Copy with simd
            __m256* sourceVector = (__m256*)srcBuffer;
            __m256* destVector = (__m256*)dstBuffer;

            int vector_count = size >> 3;
            while (vector_count--)
            {
                *destVector = *sourceVector;

                ++destVector;
                ++sourceVector;
            }

            // Handle unaligned leftovers
            dstBuffer = (float*)destVector;
            srcBuffer = (float*)sourceVector;

            switch (size & 7)
            {
            case 7: *dstBuffer++ = *srcBuffer++;
            case 6: *dstBuffer++ = *srcBuffer++;
            case 5: *dstBuffer++ = *srcBuffer++;
            case 4: *dstBuffer++ = *srcBuffer++;
            case 3: *dstBuffer++ = *srcBuffer++;
            case 2: *dstBuffer++ = *srcBuffer++;
            case 1: *dstBuffer++ = *srcBuffer++;
            }  
        }
    }

};


#endif
