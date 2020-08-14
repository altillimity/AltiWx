#include "server.h"
#include <onion/onion.hpp>
#include <onion/url.hpp>
#include <onion/response.hpp>
#include <onion/http.hpp>
#include <onion/shortcuts.hpp>
#include <onion/request.hpp>
#include "logger/logger.h"
#include <thread>
#include "logger/web_sink.h"
#include "database/database.h"
#include "libs/nlohmann/json.h"

std::shared_ptr<std::thread> serverThreadObj;
std::shared_ptr<Onion::Onion> webServer;
std::shared_ptr<Onion::Url> webRoot;

void serverThread()
{
    webServer->listen();
}

void initWebServer()
{
    std::string adress = configManager->getConfig().webConfig.address;
    std::string port = std::to_string(configManager->getConfig().webConfig.port);
    logger->info("Starting web server on " + adress + ":" + port);
    webServer = std::make_shared<Onion::Onion>(O_THREADED);
    webServer->addListenPoint(adress, port, Onion::HttpListenPoint());
    webRoot = std::make_shared<Onion::Url>(webServer.get());

    webRoot->add("test", "Some static text", HTTP_OK);
    webRoot->add("", Onion::ExportLocal("web/index.html"));
    webRoot->add("index.html", Onion::ExportLocal("web/index.html"));
    webRoot->add("logs.html", Onion::ExportLocal("web/logs.html"));
    webRoot->add("satellites.html", Onion::ExportLocal("web/satellites.html"));
    webRoot->add("starter-template.css", Onion::ExportLocal("web/starter-template.css"));
    webRoot->add("css/bootstrap-material-design.min.css", Onion::ExportLocal("web/css/bootstrap-material-design.min.css"));
    webRoot->add("js/bootstrap-material-design.min.js", Onion::ExportLocal("web/js/bootstrap-material-design.min.js"));
    webRoot->add("js/jquery-slim.min.js", Onion::ExportLocal("web/js/jquery-slim.min.js"));
    webRoot->add("js/popper.min.js", Onion::ExportLocal("web/js/popper.min.js"));

    webRoot->add("ajax/logs", [](Onion::Request &, Onion::Response &res) {
        for (std::string &current : logs)
            res << current << "</br>";
        return OCS_PROCESSED;
    });

    webRoot->add("ajax/satlist", [](Onion::Request &, Onion::Response &res) {
        nlohmann::json jsonObj;
        for (SatelliteConfig &satConfig : databaseManager->getAllSatellites())
        {
            jsonObj[std::to_string(satConfig.norad)]["name"] = satConfig.getName();
            jsonObj[std::to_string(satConfig.norad)]["min_elevation"] = satConfig.min_elevation;
            jsonObj[std::to_string(satConfig.norad)]["priority"] = satConfig.priority;
        }
        res.setHeader("content-type", "application/json");
        res << jsonObj.dump();
        return OCS_PROCESSED;
    });

    webRoot->add("ajax/downlinklist", [](Onion::Request &req, Onion::Response &res) {
        nlohmann::json jsonObj;
        for (DownlinkConfig &downlinkConfig : databaseManager->getSatellite(std::stoi(req.post()["norad"])).downlinkConfigs)
        {
            jsonObj[downlinkConfig.name]["radio"] = downlinkConfig.radio;
            jsonObj[downlinkConfig.name]["type"] = modemTypeToString(downlinkConfig.modemType);
            jsonObj[downlinkConfig.name]["frequency"] = downlinkConfig.frequency;
            jsonObj[downlinkConfig.name]["bandwidth"] = downlinkConfig.bandwidth;
            jsonObj[downlinkConfig.name]["doppler"] = downlinkConfig.dopplerCorrection;
            jsonObj[downlinkConfig.name]["output_extension"] = downlinkConfig.outputExtension;
            jsonObj[downlinkConfig.name]["processing_script"] = downlinkConfig.postProcessingScript;
            if (downlinkConfig.modemType == FM)
                jsonObj[downlinkConfig.name]["modem_audio_sample_rate"] = downlinkConfig.modem_audioSamplerate;
            if (downlinkConfig.modemType == QPSK)
                jsonObj[downlinkConfig.name]["modem_qpsk_symbol_rate"] = downlinkConfig.modem_symbolRate;
        }
        res.setHeader("content-type", "application/json");
        res << jsonObj.dump();
        return OCS_PROCESSED;
    });

    webRoot->add("ajax/modifysat", [](Onion::Request &req, Onion::Response &) {
        SatelliteConfig cfg = databaseManager->getSatellite(std::stoi(req.post()["norad"]));
        cfg.norad = std::stoi(req.post()["norad"]);
        cfg.min_elevation = std::stoi(req.post()["min_elevation"]);
        cfg.priority = std::stoi(req.post()["priority"]);
        databaseManager->setSatellite(cfg);
        return OCS_PROCESSED;
    });

    webRoot->add("ajax/delsat", [](Onion::Request &req, Onion::Response &) {
        databaseManager->deleteSatellite(std::stoi(req.post()["norad"]));
        return OCS_PROCESSED;
    });

    webRoot->add("ajax/modifydownlink", [](Onion::Request &req, Onion::Response &) {
        SatelliteConfig cfg = databaseManager->getSatellite(std::stoi(req.post()["norad"]));

        std::vector<DownlinkConfig>::iterator it = std::find_if(cfg.downlinkConfigs.begin(), cfg.downlinkConfigs.end(), [&req](DownlinkConfig &config) -> bool { return config.name == req.post()["downlink"]; });

        if (it != cfg.downlinkConfigs.end())
        {
            it->radio = req.post()["radio"];

            std::string type = (std::string)req.post()["type"];
            if (type == "FM")
                it->modemType = ModemType::FM;
            else if (type == "IQ")
                it->modemType = ModemType::IQ;
            else if (type == "IQWAV")
                it->modemType = ModemType::IQWAV;
            else if (type == "QPSK")
                it->modemType = ModemType::QPSK;

            it->frequency = std::stoi(req.post()["frequency"]);
            it->bandwidth = std::stoi(req.post()["bandwidth"]);
            it->outputExtension = req.post()["output_extension"];
            it->postProcessingScript = req.post()["script"];
            it->dopplerCorrection = req.post()["doppler"] == "true";

            if (type == "FM")
                it->modem_audioSamplerate = std::stoi(req.post()["audiorate"]);
            else if (type == "QPSK")
                it->modem_symbolRate = std::stoi(req.post()["symbolrate"]);
        }
        else
        {
            DownlinkConfig cfgDownlink;

            cfgDownlink.name = req.post()["downlink"];
            cfgDownlink.radio = req.post()["radio"];

            std::string type = (std::string)req.post()["type"];
            if (type == "FM")
                cfgDownlink.modemType = ModemType::FM;
            else if (type == "IQ")
                cfgDownlink.modemType = ModemType::IQ;
            else if (type == "IQWAV")
                cfgDownlink.modemType = ModemType::IQWAV;
            else if (type == "QPSK")
                cfgDownlink.modemType = ModemType::QPSK;

            cfgDownlink.frequency = std::stoi(req.post()["frequency"]);
            cfgDownlink.bandwidth = std::stoi(req.post()["bandwidth"]);
            cfgDownlink.outputExtension = req.post()["output_extension"];
            cfgDownlink.postProcessingScript = req.post()["script"];
            cfgDownlink.dopplerCorrection = req.post()["doppler"] == "true";

            if (type == "FM")
                cfgDownlink.modem_audioSamplerate = std::stoi(req.post()["audiorate"]);
            else if (type == "QPSK")
                cfgDownlink.modem_symbolRate = std::stoi(req.post()["symbolrate"]);

            cfg.downlinkConfigs.push_back(cfgDownlink);
        }

        databaseManager->setSatellite(cfg);
        return OCS_PROCESSED;
    });

    webRoot->add("ajax/deldownlink", [](Onion::Request &req, Onion::Response &) {
        SatelliteConfig cfg = databaseManager->getSatellite(std::stoi(req.post()["norad"]));

        std::vector<DownlinkConfig>::iterator it = std::find_if(cfg.downlinkConfigs.begin(), cfg.downlinkConfigs.end(), [&req](DownlinkConfig &config) -> bool { return config.name == req.post()["downlink"]; });

        if (it != cfg.downlinkConfigs.end())
            cfg.downlinkConfigs.erase(it);

        databaseManager->setSatellite(cfg);
        return OCS_PROCESSED;
    });

    serverThreadObj = std::make_shared<std::thread>(&serverThread);
    logger->info("Web server started!");
}

void stopWebServer()
{
    webServer->listenStop();
    if (serverThreadObj->joinable())
        serverThreadObj->join();
    logger->info("Web server stopped!");
}