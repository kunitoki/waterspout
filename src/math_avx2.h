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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_AVX2_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_AVX2_H__


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific AVX2 math class elaborating on __m256i buffers
 */

class math_avx2 : public math_avx
{
public:

    //--------------------------------------------------------------------------

    const char* name() const { return "AVX"; }


    //--------------------------------------------------------------------------

    enum AVX2MathDefines
    {
        AVX2_MIN_SIZE    = 8,
        AVX2_MIN_SAMPLES = 32,
        AVX2_ALIGN       = 0x0F
    };


    //--------------------------------------------------------------------------

    math_avx2()
    {
        //assertfalse; // not implemented !
    }

};


#endif
