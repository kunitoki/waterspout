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
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_AVX_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_AVX_H__


//==============================================================================

//------------------------------------------------------------------------------

#define avx_unroll_head(s) \
    switch (align_bytes >> 3) \
    { \
    case 1: s; \
    case 2: s; \
    case 3: s; \
    case 4: s; \
    case 5: s; \
    case 6: s; \
    case 7: s; \
    }

#define avx_unroll_tail(s) \
    switch (size & 7) \
    { \
    case 7: s; \
    case 6: s; \
    case 5: s; \
    case 4: s; \
    case 3: s; \
    case 2: s; \
    case 1: s; \
    }


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific AVX math class elaborating on __m256 buffers
 */

class math_avx : public math_sse42
{
public:

    //--------------------------------------------------------------------------

    const char* name() const { return "AVX"; }


    //--------------------------------------------------------------------------

    enum AVXMathDefines
    {
        AVX_MIN_SIZE    = 8,
        AVX_MIN_SAMPLES = 32,
        AVX_ALIGN       = 0x0F
    };


    //--------------------------------------------------------------------------

    math_avx()
    {
        //assertfalse; // not implemented !
    }

    
    //--------------------------------------------------------------------------

    void clear_buffer_float(
        float* src_buffer,
        uint32 size) const
    {
        if (size < AVX_MIN_SAMPLES)
        {
            math_sse42::clear_buffer_float(src_buffer, dst_buffer, size);
        } 
        else 
        { 
            assert(size >= AVX_MIN_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & AVX_ALIGN);

            // Copy unaligned head
            avx_unroll_head(
                --size;
                *src_buffer++ = 0.0f;
            );

            // Copy with simd
            const __m256 vvalue = _mm256_setzero_ps();
            __m256* source_vector = (__m256*)src_buffer;

            uint32 vector_count = size >> 3;
            while (vector_count--)
            {
                *source_vector = vvalue;

                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (float*)source_vector;

            avx_unroll_tail(
                *src_buffer++ = 0.0f;
            );
        }
    }


    //--------------------------------------------------------------------------

    void set_buffer_float(
        float* src_buffer,
        uint32 size,
        float value) const
    {
        if (size < AVX_MIN_SAMPLES)
        {
            math_sse42::set_buffer_float(src_buffer, size, value);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & AVX_ALIGN);

            // Copy unaligned head
            avx_unroll_head(
                --size;
                *src_buffer++ = value;
            );

            // Copy with simd
            const __m256 vvalue = _mm256_set1_ps(value);
            __m256* source_vector = (__m256*)src_buffer;

            uint32 vector_count = size >> 3;
            while (vector_count--)
            {
                *source_vector = vvalue;

                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (float*)source_vector;

            avx_unroll_tail(
                *src_buffer++ = value;
            );
        }
    }


    //--------------------------------------------------------------------------

    void copy_buffer_float(
        float* src_buffer,
        float* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & AVX_ALIGN);

        if (size < AVX_MIN_SAMPLES ||
              ((ptrdiff_t)dst_buffer & AVX_ALIGN) != align_bytes)
        {
            math_sse42::copy_buffer_float(src_buffer, dst_buffer, size);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            // Copy unaligned head
            avx_unroll_head(
                --size;
                *dst_buffer++ = *src_buffer++;
            );

            // Copy with simd
            __m256* source_vector = (__m256*)src_buffer;
            __m256* dest_vector = (__m256*)dst_buffer;

            uint32 vector_count = size >> 3;
            while (vector_count--)
            {
                *dest_vector = *source_vector;

                ++dest_vector;
                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (float*)source_vector;
            dst_buffer = (float*)dest_vector;

            avx_unroll_tail(
                *dst_buffer++ = *src_buffer++;
            );
        }
    }

};


#endif
