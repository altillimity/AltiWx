#include "config.h"
#include <spdlog/spdlog.h>

/* 
   File containing functions to support custom classes in YamlCPP
*/

namespace YAML
{
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
    struct convert<RadioConfig>
    {
        static Node encode(const RadioConfig &sdrConfig)
        {
            Node node;
            node["frequencies"] = (std::vector<long>)sdrConfig.frequencies;
            node["samplerate"] = (long)sdrConfig.samplerate;
            node["gain"] = (int)sdrConfig.gain;
            if (sdrConfig.ppm_enabled)
                node["ppm_correction"] = (double)sdrConfig.ppm;
            return node;
        }

        static bool decode(const Node &node, RadioConfig &sdrConfig)
        {
            if (!node.IsMap() || node.size() < 3)
            {
                return false;
            }

            sdrConfig.frequencies = node["frequencies"].as<std::vector<long>>();
            sdrConfig.samplerate = node["samplerate"].as<long>();
            sdrConfig.gain = node["gain"].as<int>();

            if (node["ppm_correction"].IsDefined())
            {
                sdrConfig.ppm = node["ppm_correction"].as<double>();
                sdrConfig.ppm_enabled = true;
            }
            else
                sdrConfig.ppm_enabled = false;

            return true;
        }
    };

    template <>
    struct convert<spdlog::level::level_enum>
    {
        static Node encode(const spdlog::level::level_enum &level)
        {
            Node node;
            switch (level)
            {
            case spdlog::level::trace:
                node = (std::string) "trace";
                break;
            case spdlog::level::debug:
                node = (std::string) "debug";
                break;
            case spdlog::level::info:
                node = (std::string) "info";
                break;
            case spdlog::level::warn:
                node = (std::string) "warn";
                break;
            case spdlog::level::err:
                node = (std::string) "error";
                break;
            case spdlog::level::critical:
                node = (std::string) "critical";
                break;
            case spdlog::level::off:
                node = (std::string) "off";
                break;
            default:
                break;
            }
            return node;
        }

        static bool decode(const Node &node, spdlog::level::level_enum &level)
        {
            std::string type = node.as<std::string>();
            if (type == "trace")
                level = spdlog::level::trace;
            else if (type == "debug")
                level = spdlog::level::debug;
            else if (type == "info")
                level = spdlog::level::info;
            else if (type == "warn")
                level = spdlog::level::warn;
            else if (type == "error")
                level = spdlog::level::err;
            else if (type == "critical")
                level = spdlog::level::critical;
            else if (type == "off")
                level = spdlog::level::off;
            else
                return false;

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
            node["doppler"] = (bool)downlinkConfig.doppler_correction;
            node["post_processing_script"] = (std::string)downlinkConfig.post_processing_script;
            node["output_extension"] = (std::string)downlinkConfig.output_extension;
            node["type"] = (std::string)downlinkConfig.modem_type;
            if (downlinkConfig.modem_parameters.size() > 0)
                node["parameters"] = (std::map<std::string, std::string>)downlinkConfig.modem_parameters;
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
            downlinkConfig.doppler_correction = node["doppler"].as<bool>();
            downlinkConfig.post_processing_script = node["post_processing_script"].as<std::string>();
            downlinkConfig.output_extension = node["output_extension"].as<std::string>();
            downlinkConfig.modem_type = node["type"].as<std::string>();
            if (node["parameters"].IsDefined())
                downlinkConfig.modem_parameters = node["parameters"].as<std::map<std::string, std::string>>();
            return true;
        }
    };

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
} // namespace YAML