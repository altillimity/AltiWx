#include "processing_script.h"
#include <pybind11/embed.h>
#include <fstream>
#include <streambuf>
#include <mutex>

// Initializing 2 interpreters causes a segfault
std::mutex interpreterMutex;

ProcessingScript::ProcessingScript(SatellitePass satellite_pass,
                                   SatelliteConfig satelliteConfig,
                                   DownlinkConfig downlinkConfig,
                                   TLE tle,
                                   std::string input_file,
                                   std::string filename,
                                   std::string script) : d_satellite_pass(satellite_pass),
                                                         d_satellite_config(satelliteConfig),
                                                         d_downlink_config(downlinkConfig),
                                                         d_input_file(input_file),
                                                         d_filename(filename),
                                                         d_script(script),
                                                         d_tle(tle)
{
}

void ProcessingScript::process()
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
            std::ifstream script_file(d_script);
            script_content = std::string((std::istreambuf_iterator<char>(script_file)), std::istreambuf_iterator<char>());
        }

        // Set python variables;
        altiWxModule.attr("input_file") = pybind11::cast(d_input_file);
        altiWxModule.attr("filename") = pybind11::cast(d_filename);
        altiWxModule.attr("satellite_name") = pybind11::cast(d_tle.object_name);
        altiWxModule.attr("downlink_name") = pybind11::cast(d_downlink_config.name);
        altiWxModule.attr("samplerate") = pybind11::cast(d_downlink_config.bandwidth);
        altiWxModule.attr("frequency") = pybind11::cast(d_downlink_config.frequency);
        altiWxModule.attr("northbound") = pybind11::cast(d_satellite_pass.direction == NORTHBOUND);
        altiWxModule.attr("southbound") = pybind11::cast(d_satellite_pass.direction == SOUTHBOUND);
        altiWxModule.attr("elevation") = pybind11::cast(d_satellite_pass.elevation);

        // Logging
        logger->trace(script_content);

        // Run, print exception on fail
        try
        {
            pybind11::exec(script_content);
        }
        catch (std::runtime_error &e)
        {
            logger->critical(e.what());
        }
    }
    interpreterMutex.unlock();
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
