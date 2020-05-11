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
        node["frequency"] = (long)satelliteConfig.frequency;
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
        satelliteConfig.frequency = node["frequency"].as<long>();

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
} // namespace YAML