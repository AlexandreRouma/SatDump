#include "module_metop_amsu.h"
#include <fstream>
#include "amsu_a1_reader.h"
#include "amsu_a2_reader.h"
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "logger.h"
#include <filesystem>

#define BUFFER_SIZE 8192

// Return filesize
size_t getFilesize(std::string filepath);

namespace metop
{
    namespace amsu
    {
        MetOpAMSUDecoderModule::MetOpAMSUDecoderModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters) : ProcessingModule(input_file, output_file_hint, parameters)
        {
        }

        void MetOpAMSUDecoderModule::process()
        {
            size_t filesize = getFilesize(d_input_file);
            std::ifstream data_in(d_input_file, std::ios::binary);

            std::string directory = d_output_file_hint.substr(0, d_output_file_hint.rfind('/')) + "/AMSU";

            logger->info("Using input frames " + d_input_file);
            logger->info("Decoding to " + directory);

            time_t lastTime = 0;

            libccsds::Demuxer ccsdsDemuxer(882, true);
            AMSUA1Reader a1reader;
            AMSUA2Reader a2reader;
            uint64_t amsu_cadu = 0, ccsds = 0, amsu1_ccsds = 0, amsu2_ccsds = 0;
            libccsds::CADU cadu;

            logger->info("Demultiplexing and deframing...");

            while (!data_in.eof())
            {
                // Read buffer
                data_in.read((char *)&cadu, 1024);

                // Parse this transport frame
                libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

                // Right channel? (VCID 3 is AMSU)
                if (vcdu.vcid == 3)
                {
                    amsu_cadu++;

                    // Demux
                    std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

                    // Count frames
                    ccsds += ccsdsFrames.size();

                    // Push into processor (filtering APID 64)
                    for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
                    {
                        if (pkt.header.apid == 39)
                        {
                            a1reader.work(pkt);
                            amsu1_ccsds++;
                        }
                        if (pkt.header.apid == 40)
                        {
                            a2reader.work(pkt);
                            amsu2_ccsds++;
                        }
                    }
                }

                if (time(NULL) % 10 == 0 && lastTime != time(NULL))
                {
                    lastTime = time(NULL);
                    logger->info("Progress " + std::to_string(round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f) + "%");
                }
            }

            data_in.close();

            logger->info("VCID 3 (AMSU) Frames : " + std::to_string(amsu_cadu));
            logger->info("CCSDS Frames         : " + std::to_string(ccsds));
            logger->info("AMSU A1 Frames       : " + std::to_string(amsu1_ccsds));
            logger->info("AMSU A2 Frames       : " + std::to_string(amsu2_ccsds));

            logger->info("Writing images.... (Can take a while)");

            if (!std::filesystem::exists(directory))
                std::filesystem::create_directory(directory);

            for (int i = 0; i < 2; i++)
            {
                logger->info("Channel " + std::to_string(i + 1) + "...");
                a2reader.getChannel(i).save_png(std::string(directory + "/AMSU-A2-" + std::to_string(i + 1) + ".png").c_str());
                d_output_files.push_back(directory + "/AMSU-A2-" + std::to_string(i + 1) + ".png");
            }

            for (int i = 0; i < 13; i++)
            {
                logger->info("Channel " + std::to_string(i + 3) + "...");
                a1reader.getChannel(i).save_png(std::string(directory + "/AMSU-A1-" + std::to_string(i + 3) + ".png").c_str());
                d_output_files.push_back(directory + "/AMSU-A1-" + std::to_string(i + 3) + ".png");
            }

            // Output a few nice composites as well
            logger->info("Global Composite...");
            cimg_library::CImg<unsigned short> imageAll(30 * 8, a1reader.getChannel(0).height() * 2, 1, 1);
            {
                int height = a1reader.getChannel(0).height();

                // Row 1
                imageAll.draw_image(30 * 0, 0, 0, 0, a2reader.getChannel(0));
                imageAll.draw_image(30 * 1, 0, 0, 0, a2reader.getChannel(1));
                imageAll.draw_image(30 * 2, 0, 0, 0, a1reader.getChannel(0));
                imageAll.draw_image(30 * 3, 0, 0, 0, a1reader.getChannel(1));
                imageAll.draw_image(30 * 4, 0, 0, 0, a1reader.getChannel(2));
                imageAll.draw_image(30 * 5, 0, 0, 0, a1reader.getChannel(3));
                imageAll.draw_image(30 * 6, 0, 0, 0, a1reader.getChannel(4));
                imageAll.draw_image(30 * 7, 0, 0, 0, a1reader.getChannel(5));

                // Row 2
                imageAll.draw_image(30 * 0, height, 0, 0, a1reader.getChannel(6));
                imageAll.draw_image(30 * 1, height, 0, 0, a1reader.getChannel(7));
                imageAll.draw_image(30 * 2, height, 0, 0, a1reader.getChannel(8));
                imageAll.draw_image(30 * 3, height, 0, 0, a1reader.getChannel(9));
                imageAll.draw_image(30 * 4, height, 0, 0, a1reader.getChannel(10));
                imageAll.draw_image(30 * 5, height, 0, 0, a1reader.getChannel(11));
                imageAll.draw_image(30 * 6, height, 0, 0, a1reader.getChannel(12));
            }
            imageAll.save_png(std::string(directory + "/AMSU-ALL.png").c_str());
            d_output_files.push_back(directory + "/AMSU-ALL.png");
        }

        std::string MetOpAMSUDecoderModule::getID()
        {
            return "metop_amsu";
        }

        std::vector<std::string> MetOpAMSUDecoderModule::getParameters()
        {
            return {};
        }

        std::shared_ptr<ProcessingModule> MetOpAMSUDecoderModule::getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters)
        {
            return std::make_shared<MetOpAMSUDecoderModule>(input_file, output_file_hint, parameters);
        }
    } // namespace amsu
} // namespace metop