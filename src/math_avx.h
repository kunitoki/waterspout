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

    
    //==========================================================================

    //--------------------------------------------------------------------------

    void clear_buffer_float(
        float* src_buffer,
        uint32 size) const
    {
        if (size < AVX_MIN_SAMPLES)
        {
            math_sse42::clear_buffer_float(src_buffer, size);
        }
        else 
        { 
            assert(size >= AVX_MIN_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & AVX_ALIGN);

            // Copy unaligned head
            simd_unroll_head_8(
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

            simd_unroll_tail_8(
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
            simd_unroll_head_8(
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

            simd_unroll_tail_8(
                *src_buffer++ = value;
            );
        }
    }


    //--------------------------------------------------------------------------

    void scale_buffer_float(
        float* src_buffer,
        uint32 size,
        float gain) const
    {
        if (size < AVX_MIN_SAMPLES)
        {
            math_sse42::scale_buffer_float(src_buffer, size, gain);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & AVX_ALIGN);

            // Copy unaligned head
            simd_unroll_head_8(
                --size;
                *src_buffer = *src_buffer * gain;
                undenormalizef(*src_buffer);
                ++src_buffer;
            );

            // Copy with simd
            const __m256 vscale = _mm256_set1_ps(gain);
            __m256* source_vector = (__m256*)src_buffer;

            uint32 vector_count = size >> 3;
            while (vector_count--)
            {
                *source_vector = _mm256_mul_ps(*source_vector, vscale);

                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (float*)source_vector;

            simd_unroll_tail_8(
                *src_buffer = *src_buffer * gain;
                undenormalizef(*src_buffer);
                ++src_buffer;
            );
        }
    }


    //--------------------------------------------------------------------------

    void scale_buffer_float(
        float* src_buffer,
        uint32 size,
        double gain) const
    {
        if (size < AVX_MIN_SAMPLES)
        {
            math_sse42::scale_buffer_float(src_buffer, size, gain);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & AVX_ALIGN);

            // Copy unaligned head
            simd_unroll_head_8(
                --size;
                *src_buffer = *src_buffer * (float)gain;
                undenormalizef(*src_buffer);
                ++src_buffer;
            );

            // Copy with simd
            const __m256 vscale = _mm256_set1_ps((float)gain);
            __m256* source_vector = (__m256*)src_buffer;

            uint32 vector_count = size >> 3;
            while (vector_count--)
            {
                *source_vector = _mm256_mul_ps(*source_vector, vscale);

                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (float*)source_vector;

            simd_unroll_tail_8(
                *src_buffer = *src_buffer * (float)gain;
                undenormalizef(*src_buffer);
                ++src_buffer;
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
            simd_unroll_head_8(
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

            simd_unroll_tail_8(
                *dst_buffer++ = *src_buffer++;
            );
        }
    }


    //--------------------------------------------------------------------------

    void add_buffers_float(
        float* src_buffer_a,
        float* src_buffer_b,
        float* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)dst_buffer & AVX_ALIGN);

        if (size < AVX_MIN_SAMPLES ||
            (align_bytes != ((ptrdiff_t)src_buffer_a & AVX_ALIGN) ||
             align_bytes != ((ptrdiff_t)src_buffer_b & AVX_ALIGN)))
        {
            math_sse42::add_buffers_float(src_buffer_a, src_buffer_b, dst_buffer, size);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_8(
                --size;
                *dst_buffer++ = *src_buffer_a++ + *src_buffer_b++;
            );

            // Scale with simd
            __m256* vector_buffer_a = (__m256*)src_buffer_a;
            __m256* vector_buffer_b = (__m256*)src_buffer_b;
            __m256* vector_dst_buffer = (__m256*)dst_buffer;

            uint32 vector_count = size >> 3;
            while (vector_count--)
            {
                *vector_dst_buffer =
                  _mm256_add_ps(*vector_buffer_a, *vector_buffer_b);

                ++vector_buffer_a;
                ++vector_buffer_b;
                ++vector_dst_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer_a = (float*)vector_buffer_a;
            src_buffer_b = (float*)vector_buffer_b;
            dst_buffer = (float*)vector_dst_buffer;

            simd_unroll_tail_8(
                *dst_buffer++ = *src_buffer_a++ + *src_buffer_b++;
            );
        }
    }


    //--------------------------------------------------------------------------

    void subtract_buffers_float(
        float* src_buffer_a,
        float* src_buffer_b,
        float* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)dst_buffer & AVX_ALIGN);

        if (size < AVX_MIN_SAMPLES ||
            (align_bytes != ((ptrdiff_t)src_buffer_a & AVX_ALIGN) ||
             align_bytes != ((ptrdiff_t)src_buffer_b & AVX_ALIGN)))
        {
            math_sse42::subtract_buffers_float(src_buffer_a, src_buffer_b, dst_buffer, size);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_8(
                --size;
                *dst_buffer++ = *src_buffer_a++ - *src_buffer_b++;
            );

            // Scale with simd
            __m256* vector_buffer_a = (__m256*)src_buffer_a;
            __m256* vector_buffer_b = (__m256*)src_buffer_b;
            __m256* vector_dst_buffer = (__m256*)dst_buffer;

            uint32 vector_count = size >> 3;
            while (vector_count--)
            {
                *vector_dst_buffer =
                  _mm256_sub_ps(*vector_buffer_a, *vector_buffer_b);

                ++vector_buffer_a;
                ++vector_buffer_b;
                ++vector_dst_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer_a = (float*)vector_buffer_a;
            src_buffer_b = (float*)vector_buffer_b;
            dst_buffer = (float*)vector_dst_buffer;

            simd_unroll_tail_8(
                *dst_buffer++ = *src_buffer_a++ - *src_buffer_b++;
            );
        }
    }


    //--------------------------------------------------------------------------

    void multiply_buffers_float(
        float* src_buffer_a,
        float* src_buffer_b,
        float* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)dst_buffer & AVX_ALIGN);

        if (size < AVX_MIN_SAMPLES ||
            (align_bytes != ((ptrdiff_t)src_buffer_a & AVX_ALIGN) ||
             align_bytes != ((ptrdiff_t)src_buffer_b & AVX_ALIGN)))
        {
            math_sse42::multiply_buffers_float(src_buffer_a, src_buffer_b, dst_buffer, size);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_8(
                --size;
                *dst_buffer = *src_buffer_a++ * *src_buffer_b++;
                undenormalizef(*dst_buffer);
                ++dst_buffer;
            );

            // Scale with simd
            __m256* vector_buffer_a = (__m256*)src_buffer_a;
            __m256* vector_buffer_b = (__m256*)src_buffer_b;
            __m256* vector_dst_buffer = (__m256*)dst_buffer;

            uint32 vector_count = size >> 3;
            while (vector_count--)
            {
                *vector_dst_buffer =
                  _mm256_mul_ps(*vector_buffer_a, *vector_buffer_b);

                ++vector_buffer_a;
                ++vector_buffer_b;
                ++vector_dst_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer_a = (float*)vector_buffer_a;
            src_buffer_b = (float*)vector_buffer_b;
            dst_buffer = (float*)vector_dst_buffer;

            simd_unroll_tail_8(
                *dst_buffer = *src_buffer_a++ * *src_buffer_b++;
                undenormalizef(*dst_buffer);
                ++dst_buffer;
            );
        }
    }


    //--------------------------------------------------------------------------

    void divide_buffers_float(
        float* src_buffer_a,
        float* src_buffer_b,
        float* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)dst_buffer & AVX_ALIGN);

        if (size < AVX_MIN_SAMPLES ||
            (align_bytes != ((ptrdiff_t)src_buffer_a & AVX_ALIGN) ||
             align_bytes != ((ptrdiff_t)src_buffer_b & AVX_ALIGN)))
        {
            math_sse42::multiply_buffers_float(src_buffer_a, src_buffer_b, dst_buffer, size);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_8(
                --size;
                *dst_buffer = *src_buffer_a++ / *src_buffer_b++;
                undenormalizef(*dst_buffer);
                ++dst_buffer;
            );

            // Scale with simd
            __m256* vector_buffer_a = (__m256*)src_buffer_a;
            __m256* vector_buffer_b = (__m256*)src_buffer_b;
            __m256* vector_dst_buffer = (__m256*)dst_buffer;

            uint32 vector_count = size >> 3;
            while (vector_count--)
            {
                *vector_dst_buffer =
                  _mm256_div_ps(*vector_buffer_a, *vector_buffer_b);

                ++vector_buffer_a;
                ++vector_buffer_b;
                ++vector_dst_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer_a = (float*)vector_buffer_a;
            src_buffer_b = (float*)vector_buffer_b;
            dst_buffer = (float*)vector_dst_buffer;

            simd_unroll_tail_8(
                *dst_buffer = *src_buffer_a++ / *src_buffer_b++;
                undenormalizef(*dst_buffer);
                ++dst_buffer;
            );
        }
    }


    //==========================================================================

    //--------------------------------------------------------------------------

    void clear_buffer_double(
        double* src_buffer,
        uint32 size) const
    {
        if (size < AVX_MIN_SAMPLES)
        {
            math_sse42::clear_buffer_double(src_buffer, size);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & AVX_ALIGN);

            // Copy unaligned head
            simd_unroll_head_4(
                --size;
                *src_buffer++ = 0.0;
            );

            // Copy with simd
            const __m256d vvalue = _mm256_setzero_pd();
            __m256d* source_vector = (__m256d*)src_buffer;

            uint32 vector_count = size >> 2;
            while (vector_count--)
            {
                *source_vector = vvalue;

                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (double*)source_vector;

            simd_unroll_tail_4(
                *src_buffer++ = 0.0;
            );
        }
    }


    //--------------------------------------------------------------------------

    void set_buffer_double(
        double* src_buffer,
        uint32 size,
        double value) const
    {
        if (size < AVX_MIN_SAMPLES)
        {
            math_sse42::set_buffer_double(src_buffer, size, value);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & AVX_ALIGN);

            // Copy unaligned head
            simd_unroll_head_4(
                --size;
                *src_buffer++ = value;
            );

            // Copy with simd
            const __m256d vvalue = _mm256_set1_pd(value);
            __m256d* source_vector = (__m256d*)src_buffer;

            uint32 vector_count = size >> 2;
            while (vector_count--)
            {
                *source_vector = vvalue;

                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (double*)source_vector;

            simd_unroll_tail_4(
                *src_buffer++ = value;
            );
        }
    }


    //--------------------------------------------------------------------------

    void scale_buffer_double(
        double* src_buffer,
        uint32 size,
        float gain) const
    {
        if (size < AVX_MIN_SAMPLES)
        {
            math_sse42::scale_buffer_double(src_buffer, size, gain);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & AVX_ALIGN);

            // Copy unaligned head
            simd_unroll_head_4(
                --size;
                *src_buffer = *src_buffer * (double)gain;
                undenormalizef(*src_buffer);
                ++src_buffer;
            );

            // Copy with simd
            const __m256d vscale = _mm256_set1_pd((double)gain);
            __m256d* source_vector = (__m256d*)src_buffer;

            uint32 vector_count = size >> 2;
            while (vector_count--)
            {
                *source_vector = _mm256_mul_pd(*source_vector, vscale);

                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (double*)source_vector;

            simd_unroll_tail_4(
                *src_buffer = *src_buffer * (double)gain;
                undenormalizef(*src_buffer);
                ++src_buffer;
            );
        }
    }


    //--------------------------------------------------------------------------

    void copy_buffer_double(
        double* src_buffer,
        double* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & AVX_ALIGN);

        if (size < AVX_MIN_SAMPLES ||
              ((ptrdiff_t)dst_buffer & AVX_ALIGN) != align_bytes)
        {
            math_sse42::copy_buffer_double(src_buffer, dst_buffer, size);
        }
        else
        {
            assert(size >= AVX_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_4(
                --size;
                *dst_buffer++ = *src_buffer++;
            );

            // Copy with simd
            __m256d* source_vector = (__m256d*)src_buffer;
            __m256d* dest_vector = (__m256d*)dst_buffer;

            uint32 vector_count = size >> 2;
            while (vector_count--)
            {
                *dest_vector = *source_vector;

                ++dest_vector;
                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (double*)source_vector;
            dst_buffer = (double*)dest_vector;

            simd_unroll_tail_4(
                *dst_buffer++ = *src_buffer++;
            );
        }
    }

};


#endif
