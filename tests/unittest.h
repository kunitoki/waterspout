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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_TESTS_UNITTEST_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_TESTS_UNITTEST_H__

#include "common.h"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>


//==============================================================================

//------------------------------------------------------------------------------

#define test_clear_buffer_impl(simd, datatype, s) \
    void test_clear_buffer_##simd##_##datatype() \
    { \
        datatype##_buffer buffer1(s); \
        datatype##_buffer buffer2(s); \
        simd->clear_buffer_##datatype (buffer1.data(), s); \
        fpu->clear_buffer_##datatype (buffer2.data(), s); \
        check_buffers_are_equal(buffer1.data(), buffer2.data(), s); \
    }

#define test_set_buffer_impl(simd, datatype, s) \
    void test_set_buffer_##simd##_##datatype() \
    { \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1b(s); \
        datatype##_buffer buffer2b(s); \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)1); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)1); \
        simd->set_buffer_ ##datatype (buffer1b.data(), s, (datatype)500); \
        fpu->set_buffer_ ##datatype (buffer2b.data(), s, (datatype)500); \
        check_buffer_is_value(buffer1a.data(), s, (datatype)1); \
        check_buffer_is_value(buffer1b.data(), s, (datatype)500); \
        check_buffers_are_equal(buffer1a.data(), buffer2a.data(), s); \
        check_buffers_are_equal(buffer1b.data(), buffer2b.data(), s); \
    }

#define test_scale_buffer_impl(simd, datatype, s) \
    void test_scale_buffer_##simd##_##datatype() \
    { \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        simd->scale_buffer_ ##datatype (buffer1a.data(), s, 2.0f); \
        fpu->scale_buffer_ ##datatype (buffer2a.data(), s, 2.0f); \
        check_buffers_are_equal(buffer1a.data(), buffer2a.data(), s); \
    }


#define test_copy_buffer_impl(simd, datatype, s) \
    void test_copy_buffer_##simd##_##datatype() \
    { \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1dest(s); \
        datatype##_buffer buffer2dest(s); \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)1); \
        simd->copy_buffer_ ##datatype (buffer1a.data(), buffer1dest.data(), s); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)1); \
        fpu->copy_buffer_ ##datatype (buffer2a.data(), buffer2dest.data(), s); \
        check_buffer_is_value(buffer1dest.data(), s, (datatype)1); \
        check_buffers_are_equal(buffer1dest.data(), buffer2dest.data(), s); \
    }

#define test_add_buffers_impl(simd, datatype, s) \
    void test_add_buffers_##simd##_##datatype() \
    { \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1b(s); \
        datatype##_buffer buffer2b(s); \
        datatype##_buffer buffer1dest(s); \
        datatype##_buffer buffer2dest(s); \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)1); \
        simd->set_buffer_ ##datatype (buffer1b.data(), s, (datatype)1); \
        simd->add_buffers_ ##datatype (buffer1a.data(), buffer1b.data(), buffer1dest.data(), s); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)1); \
        fpu->set_buffer_ ##datatype (buffer2b.data(), s, (datatype)1); \
        fpu->add_buffers_ ##datatype (buffer2a.data(), buffer2b.data(), buffer2dest.data(), s); \
        check_buffer_is_value(buffer1dest.data(), s, (datatype)2); \
        check_buffers_are_equal(buffer1dest.data(), buffer2dest.data(), s); \
    }

#define test_subtract_buffers_impl(simd, datatype, s) \
    void test_subtract_buffers_##simd##_##datatype() \
    { \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1b(s); \
        datatype##_buffer buffer2b(s); \
        datatype##_buffer buffer1dest(s); \
        datatype##_buffer buffer2dest(s); \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)1); \
        simd->set_buffer_ ##datatype (buffer1b.data(), s, (datatype)1); \
        simd->subtract_buffers_ ##datatype (buffer1a.data(), buffer1b.data(), buffer1dest.data(), s); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)1); \
        fpu->set_buffer_ ##datatype (buffer2b.data(), s, (datatype)1); \
        fpu->subtract_buffers_ ##datatype (buffer2a.data(), buffer2b.data(), buffer2dest.data(), s); \
        check_buffer_is_value(buffer1dest.data(), s, (datatype)0); \
        check_buffers_are_equal(buffer1dest.data(), buffer2dest.data(), s); \
    }

#define test_multiply_buffers_impl(simd, datatype, s) \
    void test_multiply_buffers_##simd##_##datatype() \
    { \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1b(s); \
        datatype##_buffer buffer2b(s); \
        datatype##_buffer buffer1dest(s); \
        datatype##_buffer buffer2dest(s); \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)2); \
        simd->set_buffer_ ##datatype (buffer1b.data(), s, (datatype)2); \
        simd->multiply_buffers_ ##datatype (buffer1a.data(), buffer1b.data(), buffer1dest.data(), s); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)2); \
        fpu->set_buffer_ ##datatype (buffer2b.data(), s, (datatype)2); \
        fpu->multiply_buffers_ ##datatype (buffer2a.data(), buffer2b.data(), buffer2dest.data(), s); \
        check_buffer_is_value(buffer1dest.data(), s, (datatype)4); \
        check_buffers_are_equal(buffer1dest.data(), buffer2dest.data(), s); \
    }

#define test_divide_buffers_impl(simd, datatype, s) \
    void test_divide_buffers_##simd##_##datatype() \
    { \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1b(s); \
        datatype##_buffer buffer2b(s); \
        datatype##_buffer buffer1dest(s); \
        datatype##_buffer buffer2dest(s); \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)4); \
        simd->set_buffer_ ##datatype (buffer1b.data(), s, (datatype)2); \
        simd->divide_buffers_ ##datatype (buffer1a.data(), buffer1b.data(), buffer1dest.data(), s); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)4); \
        fpu->set_buffer_ ##datatype (buffer2b.data(), s, (datatype)2); \
        fpu->divide_buffers_ ##datatype (buffer2a.data(), buffer2b.data(), buffer2dest.data(), s); \
        check_buffer_is_value(buffer1dest.data(), s, (datatype)2); \
        check_buffers_are_equal(buffer1dest.data(), buffer2dest.data(), s); \
    }


//------------------------------------------------------------------------------

#define test_functions_for_impl_datatype(simd, datatype) \
    test_clear_buffer_impl(simd, datatype, buffer_size) \
    test_set_buffer_impl(simd, datatype, buffer_size) \
    test_scale_buffer_impl(simd, datatype, buffer_size) \
    test_copy_buffer_impl(simd, datatype, buffer_size) \
    test_add_buffers_impl(simd, datatype, buffer_size) \
    test_subtract_buffers_impl(simd, datatype, buffer_size) \
    test_multiply_buffers_impl(simd, datatype, buffer_size) \
    test_divide_buffers_impl(simd, datatype, buffer_size)

#define test_functions_for_impl(simd) \
    test_functions_for_impl_datatype(simd, int8); \
    test_functions_for_impl_datatype(simd, uint8); \
    test_functions_for_impl_datatype(simd, int16); \
    test_functions_for_impl_datatype(simd, uint16); \
    test_functions_for_impl_datatype(simd, int32); \
    test_functions_for_impl_datatype(simd, uint32); \
    test_functions_for_impl_datatype(simd, int64); \
    test_functions_for_impl_datatype(simd, uint64); \
    test_functions_for_impl_datatype(simd, float); \
    test_functions_for_impl_datatype(simd, double);

#define test_functions \
    test_functions_for_impl(mmx); \
    test_functions_for_impl(sse); \
    test_functions_for_impl(sse2); \
    test_functions_for_impl(sse3); \
    test_functions_for_impl(ssse3); \
    test_functions_for_impl(sse41); \
    test_functions_for_impl(sse42); \
    test_functions_for_impl(avx);


//------------------------------------------------------------------------------

#define add_tests_for_impl_datatype(simd, datatype) \
    add_test_macro(test_buffers, test_clear_buffer, simd, datatype); \
    add_test_macro(test_buffers, test_set_buffer, simd, datatype); \
    add_test_macro(test_buffers, test_scale_buffer, simd, datatype); \
    add_test_macro(test_buffers, test_copy_buffer, simd, datatype); \
    add_test_macro(test_buffers, test_add_buffers, simd, datatype); \
    add_test_macro(test_buffers, test_subtract_buffers, simd, datatype); \
    add_test_macro(test_buffers, test_multiply_buffers, simd, datatype); \
    add_test_macro(test_buffers, test_divide_buffers, simd, datatype);

#define add_tests_for_impl(simd) \
    add_tests_for_impl_datatype(simd, int8); \
    add_tests_for_impl_datatype(simd, uint8); \
    add_tests_for_impl_datatype(simd, int16); \
    add_tests_for_impl_datatype(simd, uint16); \
    add_tests_for_impl_datatype(simd, int32); \
    add_tests_for_impl_datatype(simd, uint32); \
    add_tests_for_impl_datatype(simd, int64); \
    add_tests_for_impl_datatype(simd, uint64); \
    add_tests_for_impl_datatype(simd, float); \
    add_tests_for_impl_datatype(simd, double);

#define add_tests \
    add_tests_for_impl(mmx); \
    add_tests_for_impl(sse); \
    add_tests_for_impl(sse2); \
    add_tests_for_impl(sse3); \
    add_tests_for_impl(ssse3); \
    add_tests_for_impl(sse41); \
    add_tests_for_impl(sse42); \
    add_tests_for_impl(avx);


//------------------------------------------------------------------------------

class test_buffers : public test_runner
{
public:
    test_buffers()
        : fpu(FORCE_FPU),
          mmx(FORCE_MMX),
          sse(FORCE_SSE),
          sse2(FORCE_SSE2),
          sse3(FORCE_SSE3),
          ssse3(FORCE_SSSE3),
          sse41(FORCE_SSE41),
          sse42(FORCE_SSE42),
          avx(FORCE_AVX),
          buffer_size(8192)
    {
        add_tests;
    }

    test_functions

private:

    math fpu;
    math mmx;
    math sse;
    math sse2;
    math sse3;
    math ssse3;
    math sse41;
    math sse42;
    math avx;

    int buffer_size;
};


#endif // __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_TESTS_UNITTEST_H__
