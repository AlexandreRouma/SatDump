#pragma once

#include "module.h"
#include <complex>
#include <thread>
#include <fstream>
#include "deframer.h"
#include <dsp/random.h>
#include "modules/common/dsp/agc.h"
#include "modules/common/dsp/fir.h"
#include "modules/common/dsp/moving_average.h"
#include "modules/common/dsp/clock_recovery_mm.h"
#include "modules/common/dsp/file_source.h"
#include "modules/common/dsp/bpsk_carrier_pll.h"

namespace meteor
{
    class METEORHRPTDemodModule : public ProcessingModule
    {
    protected:
        std::shared_ptr<dsp::FileSourceBlock> file_source;
        std::shared_ptr<dsp::AGCBlock> agc;
        std::shared_ptr<dsp::CCFIRBlock> rrc;
        std::shared_ptr<dsp::BPSKCarrierPLLBlock> pll;
        std::shared_ptr<dsp::FFMovingAverageBlock> mov;
        std::shared_ptr<dsp::FFMMClockRecoveryBlock> rec;

        const int d_samplerate;
        const int d_buffer_size;

        uint8_t *bits_buffer;

        std::ofstream data_out;

        uint8_t byteToWrite;
        int inByteToWrite = 0;

        std::vector<uint8_t> getBytes(uint8_t *bits, int length);

        std::atomic<size_t> filesize;
        std::atomic<size_t> progress;

        // UI Stuff
        libdsp::Random rng;

    public:
        METEORHRPTDemodModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
        ~METEORHRPTDemodModule();
        void process();
        void drawUI();
        std::vector<ModuleDataType> getInputTypes();
        std::vector<ModuleDataType> getOutputTypes();

    public:
        static std::string getID();
        static std::vector<std::string> getParameters();
        static std::shared_ptr<ProcessingModule> getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
    };
} // namespace meteor