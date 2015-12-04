/*
 * waterspout
 *
 *   - simd abstraction library for audio/image manipulation -
 *
 * Copyright (c) 2015 Lucio Asnaghi
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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_H__

#include <cassert>
#include <memory>


//------------------------------------------------------------------------------

/**
 * SIMD intrinsics definitions
 */

#if defined(__MMX__) || (_MSC_VER >= 1400) || defined(_WIN64) || (_M_IX86_FP >= 1)
  #define WATERSPOUT_SIMD_MMX
#endif

#if defined(__SSE__) || (_MSC_VER >= 1400) || defined(_WIN64) || (_M_IX86_FP >= 1)
  #define WATERSPOUT_SIMD_SSE
#endif

#if defined(__SSE2__) || (_MSC_VER >= 1400) || defined(_WIN64) || (_M_IX86_FP >= 2)
  #define WATERSPOUT_SIMD_SSE2
#endif

#if defined(__SSE3__) || (_MSC_VER >= 1400)
  #define WATERSPOUT_SIMD_SSE3
#endif

#if defined(__SSSE3__) || (_MSC_VER >= 1500)
  #define WATERSPOUT_SIMD_SSSE3
#endif

#if defined(__SSE4_1__) || (_MSC_VER >= 1500)
  #define WATERSPOUT_SIMD_SSE41
#endif

#if defined(__SSE4_2__) || (_MSC_VER >= 1500)
  #define WATERSPOUT_SIMD_SSE42
#endif

#if defined(__AVX__) || (_MSC_VER >= 1700)
  #define WATERSPOUT_SIMD_AVX
#endif

#if defined(__AVX2__) || (_MSC_VER >= 1700)
  #define WATERSPOUT_SIMD_AVX2
#endif


/**
 * System bits definitions
 */

#if defined(__X86_64__) || defined(__LP64__)
    #define WATERSPOUT_SYSTEM_64BIT 1

#else
    #define WATERSPOUT_SYSTEM_32BIT 1

#endif


/**
 * System definitions
 */

#if defined(_WIN32) || defined(_WIN64)
    // Windows (x64 and x86)
    #define WATERSPOUT_SYSTEM_WINDOWS 1
    #define NOMINMAX
    #include <windows.h>

#elif defined(__APPLE__)
    #if defined(__MACH__)
	    // MacOSX
    	#define WATERSPOUT_SYSTEM_MACOSX 1
 		#include <sys/time.h>
 	#endif

#elif defined(LINUX) || defined(__linux__)
    #if defined(__ANDROID__)
        // Android
        #define WATERSPOUT_SYSTEM_ANDROID 1
        #include <cpu-features.h>
    #else
        // Linux
        #define WATERSPOUT_SYSTEM_LINUX 1
        #include <sys/time.h> // for gettimeofday() on unix
        #include <sys/resource.h>
   #endif

#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    // BSD
    #define WATERSPOUT_SYSTEM_BSD 1

#endif


/**
 * Compiler definition
 */

#if defined(_MSC_VER)
    #define WATERSPOUT_COMPILER_MSVC 1

#elif defined(__MINGW32__)
    #define WATERSPOUT_COMPILER_MINGW 1

#elif defined(__GNUC__)
    #if defined(__clang__)
        #define WATERSPOUT_COMPILER_CLANG 1
    #else
        #define WATERSPOUT_COMPILER_GCC 1
		#define WATERSPOUT_COMPILER_GCC_VERSION \
 			(__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
    #endif

#elif defined(__INTEL_COMPILER)
    #define WATERSPOUT_COMPILER_INTEL 1

#else
    #error "Unknown compiler: this compiler is not supported !"

#endif


/**
 * C++0x or C++11 support (TODO for MSVC/INTEL)
 */
#if defined(__GXX_EXPERIMENTAL_CXX0X__)
    #define WATERSPOUT_SUPPORT_CXX0X
#else
 	#define nullptr NULL
#endif


/**
 * Debug and Release definitions
 */

#if defined(DEBUG) || defined(_DEBUG)
    #define WATERSPOUT_DEBUG 1
#endif


//------------------------------------------------------------------------------

/**
 * Waterspout global macros helpers by compiler
 */

#if defined(WATERSPOUT_SYSTEM_LINUX) || defined(WATERSPOUT_SYSTEM_SOLARIS) || defined(WATERSPOUT_SYSTEM_BSD)
    #include <stddef.h>
    #include <stdint.h>
    //#include <malloc.h>
    #include <fenv.h>

    #define aligned(type_name, alignment) \
        __attribute__ ((aligned(alignment))) type_name

    #define forcedinline inline __attribute__ ((always_inline))

    #define rounding_mode_type \
        int
    #define rounding_mode(variable) \
        variable

    #define original_rounding_mode(var_name) \
        var_name = fegetround();

    #define round_float_to(mode) \
        fesetround(mode);
    #define round_float_to_nearest \
        fesetround(FE_TONEAREST);
    #define round_float_to_zero \
        fesetround(FE_TOWARDZERO);
    #define round_float_to_up \
        fesetround(FE_UPWARD);
    #define round_float_to_down \
        fesetround(FE_DOWNWARD);

    #define enable_floating_point_assertions \
        ::feenableexcept(FE_DIVBYZERO | FE_INVALID | \
          FE_OVERFLOW | FE_UNDERFLOW); \
        ::feclearexcept(FE_ALL_EXCEPT);

    #define disable_floating_point_assertions \
        ::fedisableexcept(FE_ALL_EXCEPT); \
        ::feclearexcept(FE_ALL_EXCEPT);

#elif defined(WATERSPOUT_SYSTEM_WINDOWS)
    #include <stddef.h>
    //#include <stdint.h> // TODO - check _MSC_VER
    #include <intrin.h>
    #include <float.h>

    #define aligned(type_name, alignment) \
        __declspec(align(alignment)) type_name

    #define forcedinline __forceinline

    #define rounding_mode_type \
        unsigned int
    #define rounding_mode(variable) \
        variable

    #define original_rounding_mode(var_name) \
        var_name = ::_controlfp(0, 0);

    #define round_float_to(mode) \
        ::_controlfp(mode, _MCW_RC);
    #define round_float_to_nearest \
        ::_controlfp(_RC_NEAR, _MCW_RC);
    #define round_float_to_zero \
        ::_controlfp(_RC_CHOP, _MCW_RC);
    #define round_float_to_up \
        ::_controlfp(_RC_UP, _MCW_RC);
    #define round_float_to_down \
        ::_controlfp(_RC_DOWN, _MCW_RC);

    #define enable_floating_point_assertions \
        ::_clearfp(); \
        ::_controlfp((unsigned)~(_EM_INVALID | _EM_ZERODIVIDE | \
          _EM_OVERFLOW | _EM_UNDERFLOW |_EM_DENORMAL),  \
          (unsigned)_MCW_EM);

    #define disable_floating_point_assertions \
        ::_clearfp(); \
        ::_controlfp((unsigned)(_EM_INVALID | _EM_ZERODIVIDE |  \
          _EM_OVERFLOW | _EM_UNDERFLOW |_EM_DENORMAL | _EM_INEXACT),  \
          (unsigned)_MCW_EM);

    #define isnan(value) (::_isnan(value))
    #define isinf(value) (!::_finite(value))

#else
    #define aligned(type_name, alignment)
    #define forcedinline inline
    #define enable_floating_point_assertions
    #define disable_floating_point_assertions
    #define rounding_mode_type int
    #define rounding_mode(variable)
    #define original_rounding_mode(var_name)
    #define round_float_to(mode)
    #define round_float_to_nearest
    #define round_float_to_zero
    #define round_float_to_up
    #define round_float_to_down

#endif


//------------------------------------------------------------------------------

/**
 * Common define helpers
 */

// static assert
namespace waterspout {
    template <bool b> struct staticassert_ {};
    template <> struct staticassert_<true> { static void valid_expression(){} };
}
#define staticassert(x) \
    waterspout::staticassert_<x>::valid_expression();

// always false assertion
#define assertfalse \
    do { assert(false); } while(0);

// unused variable
#define unused(x) \
    ((void)x)

// check if a pointer is aligned
#define is_aligned(ptr, byte_count) \
    (((uintptr_t)(const void*)(ptr)) % (byte_count) == 0)

// add this to your float / double to avoid denormalized numbers
#define antidenormalf 1.0e-25f
#define antidenormald 1.0e-30f

// fast way to undenormalize a float / double
#define undenormalizef(floatvalue) \
    floatvalue += antidenormalf; floatvalue -= antidenormalf;

#define undenormalized(doublevalue) \
    doublevalue += antidenormald; doublevalue -= antidenormald;


//------------------------------------------------------------------------------

namespace waterspout {


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Types valid on all architectures we build
 */

typedef char int8;
typedef unsigned char uint8;

typedef short int16;
typedef unsigned short uint16;

typedef char int24[3];
typedef unsigned char uint24[3];

typedef int int32;
typedef unsigned int uint32;

#if defined(WATERSPOUT_COMPILER_MSVC)
typedef __int64 int64;
typedef unsigned __int64 uint64;

#else
typedef long long int64;
typedef unsigned long long uint64;

#endif


//==============================================================================

//------------------------------------------------------------------------------

enum FloatRoundingModeTypes
{
    NEAREST,
    ZERO,
    UPWARD,
    DOWNWARD
};

//------------------------------------------------------------------------------

struct float_rounding_mode
{
    float_rounding_mode(FloatRoundingModeTypes type)
    {
        original_rounding_mode(old_mode_);

        rounding_mode(
	        switch(type)
	        {
	        case NEAREST:
	            round_float_to_nearest;
	            break;
	        case ZERO:
	            round_float_to_zero;
	            break;
	        case UPWARD:
	            round_float_to_up;
	            break;
	        case DOWNWARD:
	            round_float_to_down;
	            break;
	        }
	    )
    }

    ~float_rounding_mode()
    {
        round_float_to(old_mode_);
    }

private:
    rounding_mode(rounding_mode_type old_mode_;)
};


//==============================================================================

//------------------------------------------------------------------------------

class round
{
public:
    static forcedinline int f2i(float f)
    {
      #if defined(WATERSPOUT_COMPILER_MSVC)
          __asm cvttss2si eax, f

      #else
          int i;
          __asm__ ( "cvttss2si %1, %0" : "=r" (i) : "m" (f) );
          return i;

      #endif
    }

    static forcedinline int d2i(double d)
    {
        // compiler will always use cvttsd2si if possible
        return (int)d;
    }
};


//==============================================================================

//------------------------------------------------------------------------------

/**
 * @brief The memory class
 */

class memory
{
public:
    // allocate aligned memory
    static void* aligned_alloc(uint32 size_bytes, uint32 alignment_bytes);

    // free aligned memory
    static void aligned_free(void* ptr);

private:
    // noncopyable
    memory(const memory&);
    const memory& operator=(const memory&);
};


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Base aligned buffer memory support
 */

template<class T, uint32 alignment_bytes=32>
class aligned_buffer
{
public:
    aligned_buffer()
      : data_(nullptr),
        size_(0)
    {
    }

    aligned_buffer(uint32 size)
      : data_(nullptr),
        size_(0)
    {
        allocate(size);
    }

    ~aligned_buffer()
    {
        deallocate();
    }

    void resize(uint32 size)
    {
        allocate(size);
    }

    forcedinline T& operator[](uint32 index)
    {
        assert(data_ != nullptr);
        assert(index < size_);

        return data_[index];
    }

    forcedinline const T& operator[](uint32 index) const
    {
        assert(data_ != nullptr);
        assert(index < size_);

        return data_[index];
    }

    forcedinline T* data()
    {
        return data_;
    }

    forcedinline uint32 size()
    {
        return size_;
    }

private:
    void allocate(uint32 size)
    {
        if (size != size_)
        {
            deallocate();

            const uint32 size_bytes = size * sizeof(T);

            data_ = (T*)memory::aligned_alloc(size_bytes, alignment_bytes);
            size_ = size;
        }
    }

    void deallocate()
    {
        if (data_ != nullptr)
        {
            memory::aligned_free(data_);
            data_ = nullptr;
            size_ = 0;
        }
    }

    T* data_;
    uint32 size_;

    // noncopyable
    aligned_buffer(const aligned_buffer&);
    const aligned_buffer& operator=(const aligned_buffer&);
};

typedef aligned_buffer<int8, 32> int8_buffer;
typedef aligned_buffer<uint8, 32> uint8_buffer;
typedef aligned_buffer<int16, 32> int16_buffer;
typedef aligned_buffer<uint16, 32> uint16_buffer;
typedef aligned_buffer<int32, 32> int32_buffer;
typedef aligned_buffer<uint32, 32> uint32_buffer;
typedef aligned_buffer<int64, 32> int64_buffer;
typedef aligned_buffer<uint64, 32> uint64_buffer;
typedef aligned_buffer<float, 32> float_buffer;
typedef aligned_buffer<double, 32> double_buffer;


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Base math class interface
 */

#define math_interface_common_functions(datatype) \
    virtual void clear_buffer_ ##datatype ( \
        datatype * src_buffer, \
        uint32 size) const = 0; \
    \
    virtual void set_buffer_ ##datatype ( \
        datatype * src_buffer, \
        uint32 size, \
        datatype value) const = 0; \
    \
    virtual void scale_buffer_ ##datatype ( \
        datatype * src_buffer, \
        uint32 size, \
        float gain) const = 0; \
    \
    virtual void scale_buffer_ ##datatype ( \
        datatype * src_buffer, \
        uint32 size, \
        double gain) const = 0; \
    \
    virtual void copy_buffer_ ##datatype ( \
        datatype * src_buffer, \
        datatype * dst_buffer, \
        uint32 size) const = 0; \
    \
    virtual void add_buffers_ ##datatype ( \
        datatype * src_buffer_a, \
        datatype * src_buffer_b, \
        datatype * dst_buffer, \
        uint32 size) const = 0; \
    \
    virtual void subtract_buffers_ ##datatype ( \
        datatype * src_buffer_a, \
        datatype * src_buffer_b, \
        datatype * dst_buffer, \
        uint32 size) const = 0; \
    \
    virtual void multiply_buffers_ ##datatype ( \
        datatype * src_buffer_a, \
        datatype * src_buffer_b, \
        datatype * dst_buffer, \
        uint32 size) const = 0; \
    \
    virtual void divide_buffers_ ##datatype ( \
        datatype * src_buffer_a, \
        datatype * src_buffer_b, \
        datatype * dst_buffer, \
        uint32 size) const = 0;


//------------------------------------------------------------------------------

class math_interface_
{
public:
    // Define a name for the math implementation
    virtual const char* name() const = 0;

    // Basic types functions
    math_interface_common_functions(int8)
    math_interface_common_functions(uint8)
    math_interface_common_functions(int16)
    math_interface_common_functions(uint16)
    math_interface_common_functions(int32)
    math_interface_common_functions(uint32)
    math_interface_common_functions(int64)
    math_interface_common_functions(uint64)
    math_interface_common_functions(float)
    math_interface_common_functions(double)

    // Other misc functions

    // Destructor
    virtual ~math_interface_() { }

protected:
    // Constructor
    math_interface_() { }
};


//==============================================================================

//------------------------------------------------------------------------------

enum MathFlags
{
    AUTODETECT  =  0,
    FORCE_FPU   =  1,
    FORCE_MMX   =  2,
    FORCE_SSE   =  3,
    FORCE_SSE2  =  4,
    FORCE_SSE3  =  5,
    FORCE_SSSE3 =  6,
    FORCE_SSE41 =  7,
    FORCE_SSE42 =  8,
    FORCE_AVX   =  9,
    FORCE_AVX2  = 10,
    FORCE_NEON  = 11
};


//------------------------------------------------------------------------------

/**
 * The math factory class let's you instantiate a math helper class suitable for
 * your own processor, it will scale from faster to slower, falling back to FPU
 * when no other SIMD extensions are found.
 */

class math
{
public:
    // Construct a math factory
    math(int flag=AUTODETECT, bool fallback=true);

    // Destructor
    virtual ~math();

    // Returns the current arch name
    const char* name() const;

    // Operate on the underlying math object
    forcedinline math_interface_* operator->() const
    {
        return math_implementation_.get();
    }

private:
    std::unique_ptr<math_interface_> math_implementation_;

    // noncopyable
    math(const math&);
    const math& operator=(const math&);
};


} // end namespace

#endif // __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_H__
