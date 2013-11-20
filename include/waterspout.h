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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_H__

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <ctime>


//------------------------------------------------------------------------------

/**
 * SIMD intrinsics definitions
 */

#if defined(__MMX__)
  #define WATERSPOUT_SIMD_MMX
  #include <mmintrin.h>  // MMX
#endif

#if defined(__SSE__)
  #define WATERSPOUT_SIMD_SSE
  #include <xmmintrin.h> // SSE
#endif

#if defined(__SSE2__)
  #define WATERSPOUT_SIMD_SSE2
  #include <emmintrin.h> // SSE2
#endif

#if defined(__SSE3__)
  #define WATERSPOUT_SIMD_SSE3
  #include <pmmintrin.h> // SSE3
#endif

#if defined(__SSSE3__)
  #define WATERSPOUT_SIMD_SSSE3
  #include <tmmintrin.h> // SSSE3
#endif

#if defined(__SSE4_1__)
  #define WATERSPOUT_SIMD_SSE41
  #include <smmintrin.h> // SSE4.1
#endif

#if defined(__SSE4_2__)
  #define WATERSPOUT_SIMD_SSE42
  #include <nmmintrin.h> // SSE4.2
#endif

#if defined(__SSE4A__)
  #define WATERSPOUT_SIMD_SSE4A
  #include <ammintrin.h> // SSE4A
#endif

#if defined(__AES__)
  #define WATERSPOUT_SIMD_AES
  #include <wmmintrin.h> // AES
#endif

#if defined(__AVX__)
  #define WATERSPOUT_SIMD_AVX
  #include <immintrin.h> // AVX
#endif

// #include <x86intrin.h> // pull al the others in !


/**
 * System definitions
 */

#ifdef _WIN32
    // Windows (x64 and x86)
    #define WATERSPOUT_SYSTEM_WINDOWS 1
    #define NOMINMAX
    #include <windows.h>

#elif __linux__
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

#elif defined(__APPLE__) && defined(__MACH__)
    // MacOSX
    #define WATERSPOUT_SYSTEM_MACOSX 1

#elif defined(sun) || defined(__sun)
    #if defined(__SVR4) || defined(__svr4__)
        // Solaris
        #define WATERSPOUT_SYSTEM_SOLARIS 1
    #else
        // SunOS
        #define WATERSPOUT_SYSTEM_SUNOS 1
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
    #endif

#elif defined(__INTEL_COMPILER)
    #define WATERSPOUT_COMPILER_INTEL 1
    
#else
    #error "Unknown compiler: this compiler is not supported !"

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

#if defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW) || defined(WATERSPOUT_COMPILER_CLANG)
    #include <stdint.h>
    #include <malloc.h>
    #include <cpuid.h>
    #include <fenv.h>

    #define aligned(type_name, alignment) \
        __attribute__ ((aligned(alignment))) type_name

    #define forcedinline inline __attribute__ ((always_inline))

    #define enable_floating_point_assertions \
        ::feenableexcept(FE_DIVBYZERO | FE_INVALID | \
          FE_OVERFLOW | FE_UNDERFLOW); \
        ::feclearexcept(FE_ALL_EXCEPT);
                                                                 
    #define disable_floating_point_assertions \
        ::fedisableexcept(FE_ALL_EXCEPT); \
        ::feclearexcept(FE_ALL_EXCEPT);

    //#define isnan(value) isnan(value) // already defined by gcc !
    //#define isinf(value) isinf(value) // already defined by gcc ! 

    namespace waterspout {
        forcedinline double time_now()
        {
            struct timeval t;
            struct timezone tzp;
            gettimeofday(&t, &tzp);
            return t.tv_sec + t.tv_usec * 1e-6;
        }
    } // end namespace

#elif defined(WATERSPOUT_COMPILER_MSVC)
    #include <intrin.h>
    #include <stdint.h> // TODO - check _MSC_VER
    #include <float.h>

    #define aligned(type_name, alignment) \
        __declspec(align(alignment)) type_name 

    #define forcedinline __forceinline

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

    #define isnan(value) ::_isnan(value)
    #define isinf(value) (!::_finite(value))

    namespace waterspout {
        forcedinline double time_now()
        {
            LARGE_INTEGER t, f;
            QueryPerformanceCounter(&t);
            QueryPerformanceFrequency(&f);
            return double(t.QuadPart) / double(f.QuadPart);
        }
    } // end namespace

#endif


//------------------------------------------------------------------------------

/**
 * Common define helpers
 */

// always false assertion
#define assertfalse \
    do { assert(false); } while(0);

// add this to your float to avoid denormalized numbers
#define antidenormal 1.0E-25f

// fast way to undenormalize a float
#define undernormalize(floatvalue) \
    floatvalue += 1.0E-18f; floatvalue -= 1.0E-18f;


//------------------------------------------------------------------------------

namespace waterspout {


//==============================================================================

//------------------------------------------------------------------------------

/**
 * The timer class to measure elapsed time and benchmarking
 */

class timer
{
public:
    timer()
    {
        restart();
    }

    virtual ~timer()
    {
    }

    void restart()
    {
        _stopped = false;
        _clock_start = time_now();
        _cpu_start = clock();
    }

    virtual void stop()
    {
        _stopped = true;
        _cpu_end = clock();
        _clock_end = time_now();
    }

    double cpu_elapsed()
    {
        // return elapsed CPU time in ms
        if (! _stopped)
        {
            stop();
        }

        return ((double) (_cpu_end - _cpu_start)) / CLOCKS_PER_SEC * 1000.0;
    }

    double clock_elapsed()
    {
        // return elapsed wall clock time in ms
        if (! _stopped)
        {
            stop();
        }

        return (_clock_end - _clock_start) * 1000.0;
    }

protected:
    double _clock_start, _clock_end;
    clock_t _cpu_start, _cpu_end;
    bool _stopped;
};


//==============================================================================

//------------------------------------------------------------------------------

/**
 * @brief The memory class
 */

class memory
{
public:

    static void* aligned_alloc(uint32_t size_bytes, uint32_t alignment_bytes)
    {
        return (void*)
#if defined(WATERSPOUT_COMPILER_MSVC)
            ::_aligned_malloc(size_bytes, alignment_bytes);
#elif defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW) || defined(WATERSPOUT_COMPILER_CLANG)
            ::memalign(alignment_bytes, size_bytes);
#endif
    }

    static void aligned_free(void* ptr)
    {
#if defined(WATERSPOUT_COMPILER_MSVC)
        ::_aligned_free(ptr);
#elif defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW) || defined(WATERSPOUT_COMPILER_CLANG)
        ::free(ptr);
#endif
    }
};


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Base aligned buffer memory support
 */

template<class T, uint32_t alignment_bytes=32>
class aligned_buffer
{
public:
    aligned_buffer()
      : _data(NULL),
        _size(0)
    {
    }

    aligned_buffer(uint32_t size)
      : _data(NULL),
        _size(0)
    {
        allocate(size);
    }

    ~aligned_buffer()
    {
        deallocate();
    }

    void resize(uint32_t size)
    {
        allocate(size);
    }

    forcedinline T& operator[](uint32_t index)
    {
        return _data[index];
    }
    
    forcedinline const T& operator[](uint32_t index) const
    {
        return _data[index];
    }

    forcedinline T* data()
    {
        assert(_data != NULL);

        return _data;
    }

    forcedinline uint32_t size()
    {
        return _size;
    }

private:
    void allocate(uint32_t size)
    {
        deallocate();

        const uint32_t size_bytes = size * sizeof(T);
        
        _data = (T*)memory::aligned_alloc(size_bytes, alignment_bytes);
        _size = size;
    }

    void deallocate()
    {
        if (_data != NULL)
        {
            memory::aligned_free(_data);
            _data = NULL;
        }
    }

    T* _data;
    uint32_t _size;
};


typedef aligned_buffer<float, 32> float_buffer;
typedef aligned_buffer<double, 32> double_buffer;


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Base math class interface
 */

class math
{
public:
    // Define a name for the math implementation
    virtual const char* name() = 0;

    // Mono buffer manipulation
    virtual void clear_buffer(
        float* srcBuffer,
        uint32_t size) = 0;

    virtual void scale_buffer(
        float* srcBuffer,
        uint32_t size,
        float gain) = 0;

    virtual void copy_buffer(
        float* srcBuffer,
        float* dstBuffer,
        uint32_t size) = 0;

    // Mono buffer arithmetic
    virtual void add_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) = 0;

    virtual void subtract_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) = 0;

    virtual void multiply_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) = 0;

    virtual void divide_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) = 0;


    virtual ~math() { }

protected:
    math() { }
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
    FORCE_NEON  = 10
};


//------------------------------------------------------------------------------

/**
 * The math factory class let's you instantiate a math helper class suitable for
 * your own processor, it will scale from faster to slower, falling back to FPU
 * when no other SIMD extensions are found.
 */

class math_factory
{
public:
    math_factory(int flag=AUTODETECT, bool fallback=true);
    ~math_factory();

    // returns the current arch name
    const char* name();

    // operate on the underlying math object
    forcedinline math* operator->()
    {
        return _math;
    }

protected:

    math* _math;
};


} // end namespace

#endif // __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_H__
