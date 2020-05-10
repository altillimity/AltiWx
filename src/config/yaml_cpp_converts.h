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
        node["frequency"] = (float)satelliteConfig.frequency;
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
        satelliteConfig.frequency = node["frequency"].as<float>();

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
} // namespace YAML