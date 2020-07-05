#include "downlink_processor.h"
#include <pybind11/embed.h>
#include <fstream>
#include <streambuf>
#include <mutex>

// Initializing 2 interpreters causes a segfault
std::mutex interpreterMutex;

DownlinkProcessor::DownlinkProcessor(SatellitePass satellitePass, SatelliteConfig satelliteConfig, DownlinkConfig downlinkConfig, std::string inputFile, std::string filename, std::string script) : satellitePass_m(satellitePass), satelliteConfig_m(satelliteConfig), downlinkConfig_m(downlinkConfig), inputFile_m(inputFile), filename_m(filename), script_m(script)
{
}

void DownlinkProcessor::process()
{
    // Prevent running multiples interpreters at once!
    interpreterMutex.lock();
    {
        // Start python interpreter
        pybind11::scoped_interpreter guard{};
        // Open our module
        pybind11::module altiWxModule = pybind11::module::import("altiwx");

        // Read script file, scoped since once that's done we don't need the ifstream anymore...
        {
            std::ifstream scriptFile(script_m);
            scriptContent_m = std::string((std::istreambuf_iterator<char>(scriptFile)), std::istreambuf_iterator<char>());
        }

        // Probably should move that somewhere else
        std::tm *timeReadable = gmtime(&satellitePass_m.aos);
        std::string dateString = std::to_string(timeReadable->tm_year + 1900) +
                                 "-" + std::to_string(timeReadable->tm_mon + 1) + "-" + std::to_string(timeReadable->tm_mday) +
                                 "--" + std::to_string(timeReadable->tm_hour) + ":" + (timeReadable->tm_min > 9 ? std::to_string(timeReadable->tm_min) : "0" + std::to_string(timeReadable->tm_min));

        // Set python variables;
        altiWxModule.attr("input_file") = pybind11::cast<std::string>(inputFile_m);
        altiWxModule.attr("filename") = pybind11::cast<std::string>(filename_m);
        altiWxModule.attr("satellite_name") = pybind11::cast<std::string>(satelliteConfig_m.getName());
        altiWxModule.attr("downlink_name") = pybind11::cast<std::string>(downlinkConfig_m.name);
        altiWxModule.attr("samplerate") = pybind11::cast<long>(downlinkConfig_m.bandwidth);
        altiWxModule.attr("frequency") = pybind11::cast<long>(downlinkConfig_m.frequency);
        altiWxModule.attr("northbound") = pybind11::cast<bool>(satellitePass_m.northbound);
        altiWxModule.attr("southbound") = pybind11::cast<bool>(satellitePass_m.southbound);
        altiWxModule.attr("date") = pybind11::cast<std::string>(dateString);
        altiWxModule.attr("elevation") = pybind11::cast<float>(satellitePass_m.elevation);

        // Logging
        logger->trace(scriptContent_m);

        // Run, print exception on fail
        try
        {
            pybind11::exec(scriptContent_m);
        }
        catch (std::runtime_error e)
        {
            logger->critical(e.what());
        }
    }
    interpreterMutex.lock();
}

std::vector<std::string> DownlinkProcessor::getOutputs()
{
    return outputFiles_m;
}

void trace(std::string log)
{
    logger->trace(log);
}

void debug(std::string log)
{
    logger->debug(log);
}

void info(std::string log)
{
    logger->info(log);
}

void warn(std::string log)
{
    logger->warn(log);
}

void error(std::string log)
{
    logger->error(log);
}

void critical(std::string log)
{
    logger->critical(log);
}

PYBIND11_EMBEDDED_MODULE(altiwx, m)
{
    // Variables
    m.attr("input_file") = "";
    m.attr("filename") = "";
    m.attr("satellite_name") = "";
    m.attr("samplerate") = 0;
    m.attr("southbound") = 0;
    m.attr("northbound") = false;
    m.attr("southbound") = false;
    m.attr("elevation") = 0;

    // Functions
    m.def("trace", &trace);
    m.def("debug", &debug);
    m.def("info", &info);
    m.def("warn", &warn);
    m.def("error", &error);
    m.def("critical", &critical);
}
