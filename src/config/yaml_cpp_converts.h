#include "config.h"
#include <spdlog/spdlog.h>

/* 
   File containing functions to support custom classes in YamlCPP
*/

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
    struct convert<DownlinkConfig>
    {
        static Node encode(const DownlinkConfig &downlinkConfig)
        {
            Node node;
            node["name"] = (std::string)downlinkConfig.name;
            node["radio"] = (std::string)downlinkConfig.radio;
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
                break;
            case QPSK:
                node["modem_qpsk_symbol_rate"] = (long)downlinkConfig.modem_symbolRate;
                break;
            default:
                break;
            }
            return node;
        }

        static bool decode(const Node &node, DownlinkConfig &downlinkConfig)
        {
            if (!node.IsMap() || node.size() < 8)
            {
                return false;
            }

            downlinkConfig.name = node["name"].as<std::string>();
            downlinkConfig.radio = node["radio"].as<std::string>();
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
                break;
            case QPSK:
                downlinkConfig.modem_symbolRate = node["modem_qpsk_symbol_rate"].as<long>();
                break;
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
            case IQWAV:
                node = (std::string) "IQWAV";
                break;
            case QPSK:
                node = (std::string) "QPSK";
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
            else if (type == "IQWAV")
                modemType = ModemType::IQWAV;
            else if (type == "QPSK")
                modemType = ModemType::QPSK;
            else
                return false;

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
} // namespace YAML