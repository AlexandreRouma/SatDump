#pragma once

#include "module.h"
#include <complex>
#include <thread>
#include <fstream>
#include <dsp/random.h>
#include "modules/common/dsp/agc.h"
#include "modules/common/dsp/fir.h"
#include "modules/common/dsp/quadrature_demod.h"
#include "modules/common/dsp/clock_recovery_mm.h"
#include "modules/common/dsp/file_source.h"

class FSKDemodModule : public ProcessingModule
{
protected:
    std::shared_ptr<dsp::FileSourceBlock> file_source;
    std::shared_ptr<dsp::AGCBlock> agc;
    std::shared_ptr<dsp::CCFIRBlock> lpf;
    std::shared_ptr<dsp::QuadratureDemodBlock> qua;
    std::shared_ptr<dsp::FFMMClockRecoveryBlock> rec;

    const int d_samplerate;
    const int d_symbolrate;
    const int d_buffer_size;
    const int d_lpf_cutoff;
    const int d_lpf_transition_width;

    int8_t *sym_buffer;

    int8_t clamp(float x)
    {
        if (x < -128.0)
            return -127;
        if (x > 127.0)
            return 127;
        return x;
    }

    std::ofstream data_out;

    std::atomic<size_t> filesize;
    std::atomic<size_t> progress;

    // UI Stuff
    libdsp::Random rng;

public:
    FSKDemodModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
    ~FSKDemodModule();
    void process();
    void drawUI();
    std::vector<ModuleDataType> getInputTypes();
    std::vector<ModuleDataType> getOutputTypes();

public:
    static std::string getID();
    static std::vector<std::string> getParameters();
    static std::shared_ptr<ProcessingModule> getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
};