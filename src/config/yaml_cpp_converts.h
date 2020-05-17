#include "config.h"

namespace YAML
{
template <>
struct convert<SatelliteConfig>
{
    static Node encode(const SatelliteConfig &satelliteConfig)
    {
        Node node;
        node["norad"] = (int)satelliteConfig.norad;
        node["min_elevation"] = (float)satelliteConfig.min_elevation;
        node["priority"] = (int)satelliteConfig.priority;
        node["downlinks"] = (std::vector<DownlinkConfig>)satelliteConfig.downlinkConfigs;
        return node;
    }

    static bool decode(const Node &node, SatelliteConfig &satelliteConfig)
    {
        if (!node.IsMap() || node.size() != 4)
        {
            return false;
        }

        satelliteConfig.norad = node["norad"].as<int>();
        satelliteConfig.min_elevation = node["min_elevation"].as<float>();
        satelliteConfig.priority = node["priority"].as<int>();
        satelliteConfig.downlinkConfigs = node["downlinks"].as<std::vector<DownlinkConfig>>();

        return true;
    }
};

template <>
struct convert<SatelliteStation>
{
    static Node encode(const SatelliteStation &satelliteStation)
    {
        Node node;
        node["latitude"] = (double)satelliteStation.latitude;
        node["longitude"] = (double)satelliteStation.longitude;
        node["altitude"] = (double)satelliteStation.altitude;
        return node;
    }

    static bool decode(const Node &node, SatelliteStation &satelliteStation)
    {
        if (!node.IsMap() || node.size() != 3)
        {
            return false;
        }

        satelliteStation.latitude = node["latitude"].as<double>();
        satelliteStation.longitude = node["longitude"].as<double>();
        satelliteStation.altitude = node["altitude"].as<double>();

        return true;
    }
};

template <>
struct convert<SDRConfig>
{
    static Node encode(const SDRConfig &sdrConfig)
    {
        Node node;
        node["frequency"] = (long)sdrConfig.centerFrequency;
        node["sample_rate"] = (long)sdrConfig.sampleRate;
        node["gain"] = (int)sdrConfig.gain;
        return node;
    }

    static bool decode(const Node &node, SDRConfig &sdrConfig)
    {
        if (!node.IsMap() || node.size() != 3)
        {
            return false;
        }

        sdrConfig.centerFrequency = node["frequency"].as<long>();
        sdrConfig.sampleRate = node["sample_rate"].as<long>();
        sdrConfig.gain = node["gain"].as<int>();

        return true;
    }
};

template <>
struct convert<DownlinkConfig>
{
    static Node encode(const DownlinkConfig &downlinkConfig)
    {
        Node node;
        node["name"] = (std::string)downlinkConfig.name;
        node["frequency"] = (long)downlinkConfig.frequency;
        node["bandwidth"] = (long)downlinkConfig.bandwidth;
        node["doppler"] = (bool)downlinkConfig.dopplerCorrection;
        node["post_processing_script"] = (std::string)downlinkConfig.postProcessingScript;
        node["output_extension"] = (std::string)downlinkConfig.outputExtension;
        node["type"] = (ModemType)downlinkConfig.modemType;
        switch (downlinkConfig.modemType)
        {
        case FM:
            node["modem_audio_sample_rate"] = (long)downlinkConfig.modem_audioSamplerate;
        }
        return node;
    }

    static bool decode(const Node &node, DownlinkConfig &downlinkConfig)
    {
        if (!node.IsMap() || node.size() < 7)
        {
            return false;
        }

        downlinkConfig.name = node["name"].as<std::string>();
        downlinkConfig.frequency = node["frequency"].as<long>();
        downlinkConfig.bandwidth = node["bandwidth"].as<long>();
        downlinkConfig.dopplerCorrection = node["doppler"].as<bool>();
        downlinkConfig.postProcessingScript = node["post_processing_script"].as<std::string>();
        downlinkConfig.outputExtension = node["output_extension"].as<std::string>();
        downlinkConfig.modemType = node["type"].as<ModemType>();
        switch (downlinkConfig.modemType)
        {
        case FM:
            downlinkConfig.modem_audioSamplerate = node["modem_audio_sample_rate"].as<long>();
        default:
            break;
        }

        return true;
    }
};

template <>
struct convert<ModemType>
{
    static Node encode(const ModemType &modemType)
    {
        Node node;
        switch (modemType)
        {
        case FM:
            node = (std::string) "FM";
            break;
        case IQ:
            node = (std::string) "IQ";
            break;
        case LRPT:
            node = (std::string) "LRPT";
            break;
        }
        return node;
    }

    static bool decode(const Node &node, ModemType &modemType)
    {
        std::string type = node.as<std::string>();
        if (type == "FM")
            modemType = ModemType::FM;
        else if (type == "IQ")
            modemType = ModemType::IQ;
        else if (type == "LRPT")
            modemType = ModemType::LRPT;
        else
            return false;

        return true;
    }
};
} // namespace YAML