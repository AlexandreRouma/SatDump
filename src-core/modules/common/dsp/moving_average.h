#pragma once

#include "block.h"
#include <dsp/moving_average.h>

namespace dsp
{
    class CCMovingAverageBlock : public Block<std::complex<float>, std::complex<float>>
    {
    private:
        libdsp::MovingAverageCC d_mov;
        void work();

    public:
        CCMovingAverageBlock(std::shared_ptr<dsp::stream<std::complex<float>>> input, int length, std::complex<float> scale, int max_iter = 4096, unsigned int vlen = 1);
    };

    class FFMovingAverageBlock : public Block<float, float>
    {
    private:
        libdsp::MovingAverageFF d_mov;
        void work();

    public:
        FFMovingAverageBlock(std::shared_ptr<dsp::stream<float>> input, int length, float scale, int max_iter = 4096, unsigned int vlen = 1);
    };
}