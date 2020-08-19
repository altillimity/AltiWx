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
    struct convert<SDRConfig>
    {
        static Node encode(const SDRConfig &sdrConfig)
        {
            Node node;
            node["name"] = (std::string)sdrConfig.name;
            node["frequency"] = (long)sdrConfig.centerFrequency;
            node["sample_rate"] = (long)sdrConfig.sampleRate;
            node["gain"] = (int)sdrConfig.gain;
            node["device"] = (std::string)sdrConfig.soapyDeviceString;
            node["soapy_redirect"] = (bool)sdrConfig.soapy_redirect;
            node["modem_threads"] = (int)sdrConfig.demodThreads;
            if (sdrConfig.ppmEnabled)
                node["ppm_correction"] = (double)sdrConfig.ppm;
            return node;
        }

        static bool decode(const Node &node, SDRConfig &sdrConfig)
        {
            if (!node.IsMap() || node.size() < 7)
            {
                return false;
            }

            sdrConfig.name = node["name"].as<std::string>();
            sdrConfig.centerFrequency = node["frequency"].as<long>();
            sdrConfig.sampleRate = node["sample_rate"].as<long>();
            sdrConfig.gain = node["gain"].as<int>();
            sdrConfig.soapyDeviceString = node["device"].as<std::string>();
            sdrConfig.soapy_redirect = node["soapy_redirect"].as<bool>();
            sdrConfig.demodThreads = node["modem_threads"].as<int>();

            sdrConfig.soapySocket = ((std::string)ALTIWX_SOCKET_PATH) + "-" + sdrConfig.name;

            if (node["ppm_correction"].IsDefined())
            {
                sdrConfig.ppm = node["ppm_correction"].as<double>();
                sdrConfig.ppmEnabled = true;
            }
            else
                sdrConfig.ppmEnabled = false;

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
    struct convert<DBConfig>
    {
        static Node encode(const DBConfig &dbConfig)
        {
            Node node;
            node["address"] = (std::string)dbConfig.address;
            node["port"] = (int)dbConfig.port;
            node["username"] = (std::string)dbConfig.username;
            node["database"] = (std::string)dbConfig.database;
            node["password"] = (std::string)dbConfig.password;
            return node;
        }

        static bool decode(const Node &node, DBConfig &dbConfig)
        {
            if (!node.IsMap() || node.size() != 5)
            {
                return false;
            }

            dbConfig.address = node["address"].as<std::string>();
            dbConfig.port = node["port"].as<int>();
            dbConfig.username = node["username"].as<std::string>();
            dbConfig.database = node["database"].as<std::string>();
            dbConfig.password = node["password"].as<std::string>();

            return true;
        }
    };

    template <>
    struct convert<WebConfig>
    {
        static Node encode(const WebConfig &webConfig)
        {
            Node node;
            node["address"] = (std::string)webConfig.address;
            node["port"] = (int)webConfig.port;
            return node;
        }

        static bool decode(const Node &node, WebConfig &webConfig)
        {
            if (!node.IsMap() || node.size() != 2)
            {
                return false;
            }

            webConfig.address = node["address"].as<std::string>();
            webConfig.port = node["port"].as<int>();

            return true;
        }
    };
} // namespace YAML