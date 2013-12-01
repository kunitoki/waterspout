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


//==============================================================================

//------------------------------------------------------------------------------

#define test_clear_buffer_impl(simd, simd_type, datatype, s) \
    void test_##simd##_clear_buffer_##datatype() \
    { \
        math fpu(FORCE_FPU); \
        math simd(simd_type); \
        \
        datatype##_buffer buffer1(s); \
        datatype##_buffer buffer2(s); \
        \
        simd->clear_buffer_##datatype (buffer1.data(), s); \
        fpu->clear_buffer_##datatype (buffer2.data(), s); \
        \
        TEST_BUFFER_IS_VALUE(buffer1.data(), s, (datatype)0); \
        TEST_BUFFERS_ARE_EQUAL(buffer1.data(), buffer2.data(), s); \
    }

#define test_set_buffer_impl(simd, simd_type, datatype, s) \
    void test_##simd##_set_buffer_##datatype() \
    { \
        math fpu(FORCE_FPU); \
        math simd(simd_type); \
        \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1b(s); \
        datatype##_buffer buffer2b(s); \
        \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)1); \
        simd->set_buffer_ ##datatype (buffer1b.data(), s, (datatype)500); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)1); \
        fpu->set_buffer_ ##datatype (buffer2b.data(), s, (datatype)500); \
        \
        TEST_BUFFER_IS_VALUE(buffer1a.data(), s, (datatype)1); \
        TEST_BUFFER_IS_VALUE(buffer1b.data(), s, (datatype)500); \
        TEST_BUFFERS_ARE_EQUAL(buffer1a.data(), buffer2a.data(), s); \
        TEST_BUFFERS_ARE_EQUAL(buffer1b.data(), buffer2b.data(), s); \
    }

#define test_scale_buffer_impl(simd, simd_type, datatype, s) \
    void test_##simd##_scale_buffer_##datatype() \
    { \
        math fpu(FORCE_FPU); \
        math simd(simd_type); \
        \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)100); \
        simd->scale_buffer_ ##datatype (buffer1a.data(), s, 0.5f); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)100); \
        fpu->scale_buffer_ ##datatype (buffer2a.data(), s, 0.5f); \
        \
        TEST_BUFFER_IS_VALUE(buffer1a.data(), s, (datatype)50); \
        TEST_BUFFERS_ARE_EQUAL(buffer1a.data(), buffer2a.data(), s); \
    }

#define test_copy_buffer_impl(simd, simd_type, datatype, s) \
    void test_##simd##_copy_buffer_##datatype() \
    { \
        math fpu(FORCE_FPU); \
        math simd(simd_type); \
        \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1dest(s); \
        datatype##_buffer buffer2dest(s); \
        \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)1); \
        simd->copy_buffer_ ##datatype (buffer1a.data(), buffer1dest.data(), s); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)1); \
        fpu->copy_buffer_ ##datatype (buffer2a.data(), buffer2dest.data(), s); \
        \
        TEST_BUFFER_IS_VALUE(buffer1dest.data(), s, (datatype)1); \
        TEST_BUFFERS_ARE_EQUAL(buffer1dest.data(), buffer2dest.data(), s); \
    }

#define test_add_buffers_impl(simd, simd_type, datatype, s) \
    void test_##simd##_add_buffers_##datatype() \
    { \
        math fpu(FORCE_FPU); \
        math simd(simd_type); \
        \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1b(s); \
        datatype##_buffer buffer2b(s); \
        datatype##_buffer buffer1dest(s); \
        datatype##_buffer buffer2dest(s); \
        \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)1); \
        simd->set_buffer_ ##datatype (buffer1b.data(), s, (datatype)1); \
        simd->add_buffers_ ##datatype (buffer1a.data(), buffer1b.data(), buffer1dest.data(), s); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)1); \
        fpu->set_buffer_ ##datatype (buffer2b.data(), s, (datatype)1); \
        fpu->add_buffers_ ##datatype (buffer2a.data(), buffer2b.data(), buffer2dest.data(), s); \
        \
        TEST_BUFFER_IS_VALUE(buffer1dest.data(), s, (datatype)2); \
        TEST_BUFFERS_ARE_EQUAL(buffer1dest.data(), buffer2dest.data(), s); \
    }

#define test_subtract_buffers_impl(simd, simd_type, datatype, s) \
    void test_##simd##_subtract_buffers_##datatype() \
    { \
        math fpu(FORCE_FPU); \
        math simd(simd_type); \
        \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1b(s); \
        datatype##_buffer buffer2b(s); \
        datatype##_buffer buffer1dest(s); \
        datatype##_buffer buffer2dest(s); \
        \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)1); \
        simd->set_buffer_ ##datatype (buffer1b.data(), s, (datatype)1); \
        simd->subtract_buffers_ ##datatype (buffer1a.data(), buffer1b.data(), buffer1dest.data(), s); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)1); \
        fpu->set_buffer_ ##datatype (buffer2b.data(), s, (datatype)1); \
        fpu->subtract_buffers_ ##datatype (buffer2a.data(), buffer2b.data(), buffer2dest.data(), s); \
        \
        TEST_BUFFER_IS_VALUE(buffer1dest.data(), s, (datatype)0); \
        TEST_BUFFERS_ARE_EQUAL(buffer1dest.data(), buffer2dest.data(), s); \
    }

#define test_multiply_buffers_impl(simd, simd_type, datatype, s) \
    void test_##simd##_multiply_buffers_##datatype() \
    { \
        math fpu(FORCE_FPU); \
        math simd(simd_type); \
        \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1b(s); \
        datatype##_buffer buffer2b(s); \
        datatype##_buffer buffer1dest(s); \
        datatype##_buffer buffer2dest(s); \
        \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)2); \
        simd->set_buffer_ ##datatype (buffer1b.data(), s, (datatype)2); \
        simd->multiply_buffers_ ##datatype (buffer1a.data(), buffer1b.data(), buffer1dest.data(), s); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)2); \
        fpu->set_buffer_ ##datatype (buffer2b.data(), s, (datatype)2); \
        fpu->multiply_buffers_ ##datatype (buffer2a.data(), buffer2b.data(), buffer2dest.data(), s); \
        \
        TEST_BUFFER_IS_VALUE(buffer1dest.data(), s, (datatype)4); \
        TEST_BUFFERS_ARE_EQUAL(buffer1dest.data(), buffer2dest.data(), s); \
    }

#define test_divide_buffers_impl(simd, simd_type, datatype, s) \
    void test_##simd##_divide_buffers_##datatype() \
    { \
        math fpu(FORCE_FPU); \
        math simd(simd_type); \
        \
        datatype##_buffer buffer1a(s); \
        datatype##_buffer buffer2a(s); \
        datatype##_buffer buffer1b(s); \
        datatype##_buffer buffer2b(s); \
        datatype##_buffer buffer1dest(s); \
        datatype##_buffer buffer2dest(s); \
        \
        simd->set_buffer_ ##datatype (buffer1a.data(), s, (datatype)4); \
        simd->set_buffer_ ##datatype (buffer1b.data(), s, (datatype)2); \
        simd->divide_buffers_ ##datatype (buffer1a.data(), buffer1b.data(), buffer1dest.data(), s); \
        fpu->set_buffer_ ##datatype (buffer2a.data(), s, (datatype)4); \
        fpu->set_buffer_ ##datatype (buffer2b.data(), s, (datatype)2); \
        fpu->divide_buffers_ ##datatype (buffer2a.data(), buffer2b.data(), buffer2dest.data(), s); \
        \
        TEST_BUFFER_IS_VALUE(buffer1dest.data(), s, (datatype)2); \
        TEST_BUFFERS_ARE_EQUAL(buffer1dest.data(), buffer2dest.data(), s); \
    }


//------------------------------------------------------------------------------

#define test_functions_for_impl_datatype(simd, simd_type, datatype) \
    test_clear_buffer_impl(simd, simd_type, datatype, buffer_size) \
    test_set_buffer_impl(simd, simd_type, datatype, buffer_size) \
    test_scale_buffer_impl(simd, simd_type, datatype, buffer_size) \
    test_copy_buffer_impl(simd, simd_type, datatype, buffer_size) \
    test_add_buffers_impl(simd, simd_type, datatype, buffer_size) \
    test_subtract_buffers_impl(simd, simd_type, datatype, buffer_size) \
    test_multiply_buffers_impl(simd, simd_type, datatype, buffer_size) \
    test_divide_buffers_impl(simd, simd_type, datatype, buffer_size)

#define test_functions_for_impl(simd, simd_type) \
    test_functions_for_impl_datatype(simd, simd_type, int8); \
    test_functions_for_impl_datatype(simd, simd_type, uint8); \
    test_functions_for_impl_datatype(simd, simd_type, int16); \
    test_functions_for_impl_datatype(simd, simd_type, uint16); \
    test_functions_for_impl_datatype(simd, simd_type, int32); \
    test_functions_for_impl_datatype(simd, simd_type, uint32); \
    test_functions_for_impl_datatype(simd, simd_type, int64); \
    test_functions_for_impl_datatype(simd, simd_type, uint64); \
    test_functions_for_impl_datatype(simd, simd_type, float); \
    test_functions_for_impl_datatype(simd, simd_type, double);


//------------------------------------------------------------------------------

#define add_test_macro(clazz, func, simd_impl, datatype) \
    add_test(#clazz "::test_" #simd_impl "_" #func "_" #datatype, \
        static_cast<test_runner::test_function>(&clazz::test_##simd_impl##_##func##_##datatype));

#define add_test_macro_no_datatype(clazz, func, simd_impl) \
    add_test(#clazz "::test_" #simd_impl "_" #func, \
        static_cast<test_runner::test_function>(&clazz::test_##simd_impl##_##func));

#define add_tests_for_impl_datatype(simd, datatype) \
    add_test_macro(test_buffers, clear_buffer, simd, datatype); \
    add_test_macro(test_buffers, set_buffer, simd, datatype); \
    add_test_macro(test_buffers, scale_buffer, simd, datatype); \
    add_test_macro(test_buffers, copy_buffer, simd, datatype); \
    add_test_macro(test_buffers, add_buffers, simd, datatype); \
    add_test_macro(test_buffers, subtract_buffers, simd, datatype); \
    add_test_macro(test_buffers, multiply_buffers, simd, datatype); \
    add_test_macro(test_buffers, divide_buffers, simd, datatype);

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


//------------------------------------------------------------------------------

class test_buffers : public test_runner
{
public:
    test_buffers()
        : buffer_size(8192)
    {
        // add tests
#if defined(WATERSPOUT_SIMD_MMX)
        add_tests_for_impl(mmx);
#endif
#if defined(WATERSPOUT_SIMD_SSE)
        add_tests_for_impl(sse);
#endif
#if defined(WATERSPOUT_SIMD_SSE2)
        add_tests_for_impl(sse2);
#endif
#if defined(WATERSPOUT_SIMD_SSE3)
        add_tests_for_impl(sse3);
#endif
#if defined(WATERSPOUT_SIMD_SSSE3)
        add_tests_for_impl(ssse3);
#endif
#if defined(WATERSPOUT_SIMD_SSE41)
        add_tests_for_impl(sse41);
#endif
#if defined(WATERSPOUT_SIMD_SSE42)
        add_tests_for_impl(sse42);
#endif
#if defined(WATERSPOUT_SIMD_AVX)
        add_tests_for_impl(avx);
#endif
#if defined(WATERSPOUT_SIMD_AVX2)
        add_tests_for_impl(avx2);
#endif
    }

    // implementations
#if defined(WATERSPOUT_SIMD_MMX)
    test_functions_for_impl(mmx, FORCE_MMX)
#endif
#if defined(WATERSPOUT_SIMD_SSE)
    test_functions_for_impl(sse, FORCE_SSE)
#endif
#if defined(WATERSPOUT_SIMD_SSE2)
    test_functions_for_impl(sse2, FORCE_SSE2)
#endif
#if defined(WATERSPOUT_SIMD_SSE3)
    test_functions_for_impl(sse3, FORCE_SSE3)
#endif
#if defined(WATERSPOUT_SIMD_SSSE3)
    test_functions_for_impl(ssse3, FORCE_SSSE3)
#endif
#if defined(WATERSPOUT_SIMD_SSE41)
    test_functions_for_impl(sse41, FORCE_SSE41)
#endif
#if defined(WATERSPOUT_SIMD_SSE42)
    test_functions_for_impl(sse42, FORCE_SSE42)
#endif
#if defined(WATERSPOUT_SIMD_AVX)
    test_functions_for_impl(avx, FORCE_AVX)
#endif
#if defined(WATERSPOUT_SIMD_AVX2)
    test_functions_for_impl(avx2, FORCE_AVX2)
#endif

private:

    int buffer_size;
};


#endif // __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_TESTS_UNITTEST_H__
