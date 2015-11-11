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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_TESTS_COMMON_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_TESTS_COMMON_H__

#include <waterspout.h>

#include <ctime>
#include <typeinfo>
#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>


//==============================================================================

using namespace waterspout;


//==============================================================================

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
        stopped_ = false;
        clock_start_ = time_now();
        cpu_start_ = clock();
    }

    virtual void stop()
    {
        stopped_ = true;
        cpu_end_ = clock();
        clock_end_ = time_now();
    }

    double cpu_elapsed()
    {
        // return elapsed CPU time in ms
        if (! stopped_)
        {
            stop();
        }

        return ((double)(cpu_end_ - cpu_start_)) / CLOCKS_PER_SEC * 1000.0;
    }

    double clock_elapsed()
    {
        // return elapsed wall clock time in ms
        if (! stopped_)
        {
            stop();
        }

        return (clock_end_ - clock_start_) * 1000.0;
    }

    forcedinline double time_now()
    {
#if defined(WATERSPOUT_COMPILER_MSVC)
        LARGE_INTEGER t, f;
        QueryPerformanceCounter(&t);
        QueryPerformanceFrequency(&f);
        return double(t.QuadPart) / double(f.QuadPart);
#else
        struct timeval t;
        struct timezone tzp;
        gettimeofday(&t, &tzp);
        return t.tv_sec + t.tv_usec * 1e-6;
#endif
    }

protected:
    double clock_start_, clock_end_;
    clock_t cpu_start_, cpu_end_;
    bool stopped_;
};


//==============================================================================

class test_exception : public std::exception
{
public:
    explicit test_exception(const std::string& str)
        : str_(str)
    {
    }

    ~test_exception() throw()
    {
    }

    virtual const char* what() const throw()
    {
        return str_.c_str();
    }

protected:

    std::string str_;
};


//==============================================================================

class test_base
{
public:
    virtual ~test_base() {}

    virtual void run() throw() = 0;

    std::string name() const
    {
        return name_;
    }

    void set_name(const std::string& name)
    {
        name_ = name;
    }

protected:
    test_base()
        : name_("unnamed")
    {}

    std::string name_;
};


//==============================================================================

class test_run_status
{
public:
    explicit test_run_status()
    {
        clear();
    }

    ~test_run_status()
    {
    }

    bool is_valid() const
    {
        return valid_;
    }

    std::string status() const
    {
        return status_;
    }

    void signal_exception(bool is_valid, const std::string& status)
    {
        status_ = status;
        valid_ = is_valid;
    }

    void clear()
    {
        status_.clear();
        valid_ = true;
    }

private:
    bool valid_;
    std::string status_;
};


//==============================================================================

/**
 * The testrunner class
 */

//------------------------------------------------------------------------------

class test_runner
{
public:

    typedef void (test_runner::*test_function) (void);

    test_runner()
        : verbose_(false)
    {
    }

    virtual ~test_runner()
    {
    }

    void set_verbose(bool verbose)
    {
        verbose_ = verbose;
    }

    void add_test(const std::string& name, test_function fn)
    {
        names_.push_back(name);
        tests_.push_back(fn);
        status_.push_back(new test_run_status);
    }

    void clear_run_status()
    {
        for (size_t i = 0; i < status_.size(); i++)
        {
            status_[i]->clear();
        }
    }

    void run_tests()
    {
        clear_run_status();

        for (size_t i = 0; i < num_total_tests(); i++)
        {
            try
            {
                test_function this_test_function = tests_[i];
                (this->*this_test_function)();
            }
            catch(std::exception& ex)
            {
                status_[i]->signal_exception(false, ex.what());
            }
        }
    }

    size_t num_total_tests() const
    {
        return tests_.size();
    }

    size_t num_valid_tests() const
    {
        size_t valid_tests = 0;

        for (size_t i = 0; i < status_.size(); i++)
        {
            if (status_[i]->is_valid())
                ++valid_tests;
        }

        return valid_tests;
    }

    size_t num_invalid_tests() const
    {
        size_t invalid_tests = 0;

        for (size_t i = 0; i < status_.size(); i++)
        {
            if (! status_[i]->is_valid())
                ++invalid_tests;
        }

        return invalid_tests;
    }

    void print_results()
    {
        std::clog << "Running tests: " << num_total_tests() << " total" << std::endl;

        for (size_t i = 0; i < status_.size(); i++)
        {
            const bool is_valid = status_[i]->is_valid();
            if (verbose_ || ! is_valid)
            {
                std::clog << "  - Test " << i << " of " << num_total_tests() << " ("
                          << names_[i] << "): ";

                if (is_valid)
                {
                    std::clog << "OK" << std::endl;
                }
            }

            if (! is_valid)
            {
                std::clog << "ERROR" << std::endl;
                std::clog << "  " << status_[i]->status() << std::endl;
            }
        }

        std::clog << "Results: " << num_valid_tests() << " / " << num_total_tests() << " are valid" << std::endl;
    }

protected:
    bool verbose_;

    std::vector< test_function > tests_;
    std::vector< std::string > names_;
    std::vector< scoped_ptr<test_run_status> > status_;
};


//==============================================================================

//------------------------------------------------------------------------------

template<typename T> std::string print_var_(T value)
{
    std::ostringstream str;
    str << value;
    return str.str();
}


//------------------------------------------------------------------------------

template<> std::string print_var_(int8 value)
{
    std::ostringstream str;
    str << (int32)value;
    return str.str();
}


//------------------------------------------------------------------------------

template<> std::string print_var_(uint8 value)
{
    std::ostringstream str;
    str << (uint32)value;
    return str.str();
}


//==============================================================================

//------------------------------------------------------------------------------

template<typename T>
void test_value_is_equal_(const char* file, int line, T a, T b)
{
    if (a != b)
    {
        std::ostringstream error;
        error << file << "(" << line << "): " << "Values should be equal..."
              << " (" << print_var_(a) << "!=" << print_var_(b) << ")" << std::endl;

        throw test_exception(error.str());
    }
}


//------------------------------------------------------------------------------

template<typename T>
void test_value_is_not_equal_(const char* file, int line, T a, T b)
{
    if (a == b)
    {
        std::ostringstream error;
        error << file << "(" << line << "): " << "Values should be not equal..."
              << " (" << print_var_(a) << "==" << print_var_(b) << ")" << std::endl;

        throw test_exception(error.str());
    }
}


//------------------------------------------------------------------------------

template<typename T>
void test_value_is_less_(const char* file, int line, T a, T b)
{
    if (a < b)
    {
        std::ostringstream error;
        error << file << "(" << line << "): " << "Value A should be less than B..."
              << " (" << print_var_(a) << ">=" << print_var_(b) << ")" << std::endl;

        throw test_exception(error.str());
    }
}


//------------------------------------------------------------------------------

template<typename T>
void test_value_is_more_(const char* file, int line, T a, T b)
{
    if (a < b)
    {
        std::ostringstream error;
        error << file << "(" << line << "): " << "Value A should be more than B..."
              << " (" << print_var_(a) << "<=" << print_var_(b) << ")" << std::endl;

        throw test_exception(error.str());
    }
}


//------------------------------------------------------------------------------

template<typename T>
void test_buffer_is_value_(const char* file, int line, T* buffer, uint32 size, T value)
{
    for (uint32 i = 0; i < size; ++i)
    {
        if (buffer[i] != value)
        {
            std::ostringstream error;
            error << file << "(" << line << "): " << "Buffer is not a specific value..."
                  << "at index " << i << " (" << print_var_(buffer[i]) << "!=" << print_var_(value) << ")" << std::endl;

            throw test_exception(error.str());
        }
    }
}


//------------------------------------------------------------------------------

template<typename T>
void test_buffer_is_zero_(const char* file, int line, T* buffer, uint32 size)
{
    test_buffer_is_value_(file, line, buffer, size, static_cast<T>(0));
}


//------------------------------------------------------------------------------

template<typename T>
void test_buffers_are_equal_(const char* file, int line, T* a, T* b, uint32 size)
{
    for (uint32 i = 0; i < size; ++i)
    {
        if (a[i] != b[i])
        {
            std::ostringstream error;
            error << file << "(" << line << "): " << "Buffers are not equals... "
                  << "at index " << i << " (" << print_var_(a[i]) << "!=" << print_var_(b[i]) << ")" << std::endl;

            throw test_exception(error.str());
        }
    }
}


//==============================================================================

//------------------------------------------------------------------------------

#define TEST_IS_EQUAL(a, b) \
    test_value_is_equal_(__FILE__, __LINE__, a, b);

#define TEST_IS_NOT_EQUAL(a, b) \
    test_value_is_not_equal_(__FILE__, __LINE__, a, b);

#define TEST_IS_LESS(a, b) \
    test_value_is_less_(__FILE__, __LINE__, a, b);

#define TEST_IS_MORE(a, b) \
    test_value_is_more_(__FILE__, __LINE__, a, b);


//------------------------------------------------------------------------------

#define TEST_BUFFER_IS_VALUE(buffer, size, value) \
    test_buffer_is_value_(__FILE__, __LINE__, buffer, size, value);

#define TEST_BUFFER_IS_ZERO(buffer, size) \
    test_buffer_is_zero_(__FILE__, __LINE__, buffer, size);

#define TEST_BUFFERS_ARE_EQUAL(a, b, size) \
    test_buffers_are_equal_(__FILE__, __LINE__, a, b, size);


#endif // __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_TESTS_COMMON_H__
