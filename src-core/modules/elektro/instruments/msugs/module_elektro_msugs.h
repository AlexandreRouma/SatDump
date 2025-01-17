#pragma once

#include "module.h"
#include <fstream>

namespace elektro
{
    namespace msugs
    {
        class ELEKTROMSUGSDecoderModule : public ProcessingModule
        {
        protected:
            std::ifstream data_in;
            std::atomic<size_t> filesize;
            std::atomic<size_t> progress;

        public:
            ELEKTROMSUGSDecoderModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
            void process();
            void drawUI();

        public:
            static std::string getID();
            static std::vector<std::string> getParameters();
            static std::shared_ptr<ProcessingModule> getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
        };
    } // namespace msugs
} // namespace elektro