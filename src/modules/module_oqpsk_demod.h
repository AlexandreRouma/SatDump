#pragma once

#include "module.h"
#include <complex>
#include <dsp/agc.h>
#include <dsp/fir_filter.h>
#include <dsp/costas_loop.h>
#include <dsp/dc_blocker.h>
#include <dsp/clock_recovery_mm.h>
#include "common/delay_one_imag.h"
#include <dsp/pipe.h>
#include <thread>
#include <fstream>

class OQPSKDemodModule : public ProcessingModule
{
protected:
    std::shared_ptr<libdsp::DCBlocker> dcb;
    std::shared_ptr<libdsp::AgcCC> agc;
    std::shared_ptr<libdsp::FIRFilterCCF> rrc;
    std::shared_ptr<libdsp::CostasLoop> pll;
    std::shared_ptr<DelayOneImag> del;
    std::shared_ptr<libdsp::ClockRecoveryMMCC> rec;

    const float d_agc_rate;
    const int d_samplerate;
    const int d_symbolrate;
    const float d_rrc_alpha;
    const int d_rrc_taps;
    const float d_loop_bw;
    const int d_buffer_size;

    std::complex<float> *in_buffer, *in_buffer1, *in_buffer2;
    std::complex<float> *agc_buffer, *agc_buffer2;
    std::complex<float> *rrc_buffer, *rrc_buffer2;
    std::complex<float> *pll_buffer, *pll_buffer1, *pll_buffer2;
    std::complex<float> *rec_buffer, *rec_buffer2;
    int8_t *sym_buffer;

    // All FIFOs we use along the way
    libdsp::Pipe<std::complex<float>> *in_pipe;
    libdsp::Pipe<std::complex<float>> *agc_pipe;
    libdsp::Pipe<std::complex<float>> *rrc_pipe;
    libdsp::Pipe<std::complex<float>> *pll_pipe;
    libdsp::Pipe<std::complex<float>> *rec_pipe;

    bool agcRun, rrcRun, pllRun, recRun;

    // Int16 buffer
    int16_t *buffer_i16;
    // Int8 buffer
    int8_t *buffer_i8;
    // Uint8 buffer
    uint8_t *buffer_u8;

    bool f32 = false, i16 = false, i8 = false, w8 = false;

    int8_t clamp(float x)
    {
        if (x < -128.0)
            return -127;
        if (x > 127.0)
            return 127;
        return x;
    }

    void fileThreadFunction();
    void agcThreadFunction();
    void rrcThreadFunction();
    void pllThreadFunction();
    void clockrecoveryThreadFunction();

    std::thread fileThread, agcThread, rrcThread, pllThread, recThread;

    std::ifstream data_in;
    std::ofstream data_out;

public:
    OQPSKDemodModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
    ~OQPSKDemodModule();
    void process();

public:
    static std::string getID();
    static std::vector<std::string> getParameters();
    static std::shared_ptr<ProcessingModule> getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
};