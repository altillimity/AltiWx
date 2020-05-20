#include "SoapyAltiWx.hpp"
#include <SoapySDR/Registry.hpp>
#include <mutex>
#include <map>

static std::vector<SoapySDR::Kwargs> findAltiWx(const SoapySDR::Kwargs &args)
{
    std::vector<SoapySDR::Kwargs> results;

    SoapySDR::Kwargs devInfo;
    devInfo["label"] = "AltiWx";
    devInfo["product"] = "SDR Live";
    devInfo["serial"] = "000000000";
    devInfo["manufacturer"] = "Altillimity";

    results.push_back(devInfo);

    return results;
}

static SoapySDR::Device *makeAltiWx(const SoapySDR::Kwargs &args)
{
    return new SoapyAltiWx(args);
}

static SoapySDR::Registry registerAltiWx("altiwx", &findAltiWx, &makeAltiWx, SOAPY_SDR_ABI_VERSION);
