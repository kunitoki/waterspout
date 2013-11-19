#include "../src/waterspout.cpp"
#include <iostream>

using namespace waterspout;

void check_buffers(const char* method, float* a, float*b, uint32_t size)
{
    for (int i = 0; i < size; ++i)
    {
        if (a[i] != b[i])
        {
            std::cout << method << ": Copy is invalid !" << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    const uint32_t size = 1024 * 1024;
    
    {
        math_factory fpu(FORCE_FPU);
        float_buffer srcBuffer(size), dstBuffer(size);

        for (int i = 0; i < srcBuffer.size(); ++i)
        {
            srcBuffer[i] = i / (float)srcBuffer.size();
            dstBuffer[i] = 0.0f;
        }

        timer t;
        fpu->copy_buffer(srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        double elapsed = t.clock_elapsed();

        check_buffers("FPU", srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        std::cout << "FPU: " << std::setprecision(32) <<  elapsed << " " << srcBuffer.size() << std::endl;
    }

    {
        math_factory sse2(FORCE_SSE2);
        float_buffer srcBuffer(size), dstBuffer(size);

        for (int i = 0; i < srcBuffer.size(); ++i)
        {
            srcBuffer[i] = i / (float)srcBuffer.size();
            dstBuffer[i] = 0.0f;
        }

        timer t;
        sse2->copy_buffer(srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        double elapsed = t.clock_elapsed();
        
        check_buffers("SSE", srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        std::cout << "SSE: " << std::setprecision(32) << elapsed << " " << srcBuffer.size() << std::endl;
    }

    {
        math_factory avx(FORCE_AVX);
        float_buffer srcBuffer(size), dstBuffer(size);

        for (int i = 0; i < srcBuffer.size(); ++i)
        {
            srcBuffer[i] = i / (float)srcBuffer.size();
            dstBuffer[i] = 0.0f;
        }

        timer t;
        avx->copy_buffer(srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        double elapsed = t.clock_elapsed();
        
        check_buffers("AVX", srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        std::cout << "AVX: " << std::setprecision(32) << elapsed << " " << srcBuffer.size() << std::endl;
    }
    
    return 0;
}
