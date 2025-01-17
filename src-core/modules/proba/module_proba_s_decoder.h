#pragma once

#include "module.h"
#include <complex>
#include "modules/common/sathelper/viterbi27.h"
#include <fstream>
#include <dsp/random.h>

namespace proba
{
    class ProbaSDecoderModule : public ProcessingModule
    {
    protected:
        bool derandomize;

        // Work buffers
        uint8_t rsWorkBuffer[255];

        void shiftWithConstantSize(uint8_t *arr, int pos, int length);

        uint8_t *buffer, *buffer_2;

        std::ifstream data_in;
        std::ofstream data_out;

        std::atomic<size_t> filesize;
        std::atomic<size_t> progress;

        bool locked = false;
        int errors[5];
        uint32_t cor;

        sathelper::Viterbi27 viterbi;

        // UI Stuff
        float ber_history[200];
        float cor_history[200];

        libdsp::Random rng;

    public:
        ProbaSDecoderModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
        ~ProbaSDecoderModule();
        void process();
        void drawUI();

    public:
        static std::string getID();
        static std::vector<std::string> getParameters();
        static std::shared_ptr<ProcessingModule> getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
    };
} // namespace proba