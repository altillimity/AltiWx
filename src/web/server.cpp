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
#include "dsp/modem/modem.h"

// Server objects we need
std::shared_ptr<std::thread> serverThreadObj;
std::shared_ptr<Onion::Onion> webServer;
std::shared_ptr<Onion::Url> webRoot;

void serverThread()
{
    // Just run it
    webServer->listen();
}

void initWebServer()
{
    // Get config settings
    std::string adress = configManager->getConfig().webConfig.address;
    std::string port = std::to_string(configManager->getConfig().webConfig.port);
    logger->info("Starting web server on " + adress + ":" + port);

    // Init our objects
    webServer = std::make_shared<Onion::Onion>(O_THREADED);
    webServer->addListenPoint(adress, port, Onion::HttpListenPoint());
    webRoot = std::make_shared<Onion::Url>(webServer.get());

    // Register static resources
    webRoot->add("starter-template.css", Onion::ExportLocal("web/starter-template.css"));
    webRoot->add("css/bootstrap-material-design.min.css", Onion::ExportLocal("web/css/bootstrap-material-design.min.css"));
    webRoot->add("js/bootstrap-material-design.min.js", Onion::ExportLocal("web/js/bootstrap-material-design.min.js"));
    webRoot->add("js/jquery-slim.min.js", Onion::ExportLocal("web/js/jquery-slim.min.js"));
    webRoot->add("js/popper.min.js", Onion::ExportLocal("web/js/popper.min.js"));

    // Register static pages
    webRoot->add("", Onion::ExportLocal("web/index.html"));
    webRoot->add("index.html", Onion::ExportLocal("web/index.html"));
    webRoot->add("logs.html", Onion::ExportLocal("web/logs.html"));
    webRoot->add("satellites.html", Onion::ExportLocal("web/satellites.html"));

    // Return logs
    webRoot->add("ajax/logs", [](Onion::Request &, Onion::Response &res) {
        for (std::string &current : logs)
            res << current << "</br>";
        return OCS_PROCESSED;
    });

    // Return satellite list
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

    // Return downlink list for a satellite
    webRoot->add("ajax/downlinklist", [](Onion::Request &req, Onion::Response &res) {
        nlohmann::json jsonObj;
        for (DownlinkConfig &downlinkConfig : databaseManager->getSatellite(std::stoi(req.post()["norad"])).downlinkConfigs)
        {
            jsonObj[downlinkConfig.name]["radio"] = downlinkConfig.radio;
            jsonObj[downlinkConfig.name]["type"] = downlinkConfig.modemType;
            jsonObj[downlinkConfig.name]["frequency"] = downlinkConfig.frequency;
            jsonObj[downlinkConfig.name]["bandwidth"] = downlinkConfig.bandwidth;
            jsonObj[downlinkConfig.name]["doppler"] = downlinkConfig.dopplerCorrection;
            jsonObj[downlinkConfig.name]["output_extension"] = downlinkConfig.outputExtension;
            jsonObj[downlinkConfig.name]["processing_script"] = downlinkConfig.postProcessingScript;
            jsonObj[downlinkConfig.name]["parameters"] = downlinkConfig.modemParameters;
        }
        res.setHeader("content-type", "application/json");
        res << jsonObj.dump();
        return OCS_PROCESSED;
    });

    // Return modem parameters
    webRoot->add("ajax/modemparameters", [](Onion::Request &req, Onion::Response &res) {
        nlohmann::json jsonObj;

        if (modemRegistry.find(req.post()["type"]) != modemRegistry.end())
            jsonObj = modemRegistry[req.post()["type"]]()->getParameters();
        else
            jsonObj = (std::vector<std::string>){};

        res.setHeader("content-type", "application/json");
        res << jsonObj.dump();
        return OCS_PROCESSED;
    });

    // Return modem list
    webRoot->add("ajax/modemlist", [](Onion::Request &req, Onion::Response &res) {
        nlohmann::json jsonObj;

        for (std::pair<const std::string, std::function<std::shared_ptr<Modem>()>> &it : modemRegistry)
            jsonObj += it.first;

        res.setHeader("content-type", "application/json");
        res << jsonObj.dump();
        return OCS_PROCESSED;
    });

    // Modify a satellite
    webRoot->add("ajax/modifysat", [](Onion::Request &req, Onion::Response &) {
        SatelliteConfig cfg = databaseManager->getSatellite(std::stoi(req.post()["norad"]));
        cfg.norad = std::stoi(req.post()["norad"]);
        cfg.min_elevation = std::stoi(req.post()["min_elevation"]);
        cfg.priority = std::stoi(req.post()["priority"]);
        databaseManager->setSatellite(cfg);
        return OCS_PROCESSED;
    });

    // Delete a satellite
    webRoot->add("ajax/delsat", [](Onion::Request &req, Onion::Response &) {
        databaseManager->deleteSatellite(std::stoi(req.post()["norad"]));
        return OCS_PROCESSED;
    });

    // Modify a downlink
    webRoot->add("ajax/modifydownlink", [](Onion::Request &req, Onion::Response &) {
        SatelliteConfig cfg = databaseManager->getSatellite(std::stoi(req.post()["norad"]));

        std::vector<DownlinkConfig>::iterator it = std::find_if(cfg.downlinkConfigs.begin(), cfg.downlinkConfigs.end(), [&req](DownlinkConfig &config) -> bool { return config.name == req.post()["downlink"]; });

        if (it != cfg.downlinkConfigs.end())
        {
            it->radio = req.post()["radio"];

            it->modemType = (std::string)req.post()["type"];

            it->frequency = std::stoi(req.post()["frequency"]);
            it->bandwidth = std::stoi(req.post()["bandwidth"]);
            it->outputExtension = req.post()["output_extension"];
            it->postProcessingScript = req.post()["script"];
            it->dopplerCorrection = req.post()["doppler"] == "true";

            it->modemParameters = (std::unordered_map<std::string, std::string>)nlohmann::json::parse(req.post()["parameters"]);
        }
        else
        {
            DownlinkConfig cfgDownlink;

            cfgDownlink.name = req.post()["downlink"];
            cfgDownlink.radio = req.post()["radio"];

            cfgDownlink.modemType = (std::string)req.post()["type"];

            cfgDownlink.frequency = std::stoi(req.post()["frequency"]);
            cfgDownlink.bandwidth = std::stoi(req.post()["bandwidth"]);
            cfgDownlink.outputExtension = req.post()["output_extension"];
            cfgDownlink.postProcessingScript = req.post()["script"];
            cfgDownlink.dopplerCorrection = req.post()["doppler"] == "true";

            it->modemParameters = (std::unordered_map<std::string, std::string>)nlohmann::json::parse(req.post()["parameters"]);

            cfg.downlinkConfigs.push_back(cfgDownlink);
        }

        databaseManager->setSatellite(cfg);
        return OCS_PROCESSED;
    });

    // Delete a downlink
    webRoot->add("ajax/deldownlink", [](Onion::Request &req, Onion::Response &) {
        SatelliteConfig cfg = databaseManager->getSatellite(std::stoi(req.post()["norad"]));

        std::vector<DownlinkConfig>::iterator it = std::find_if(cfg.downlinkConfigs.begin(), cfg.downlinkConfigs.end(), [&req](DownlinkConfig &config) -> bool { return config.name == req.post()["downlink"]; });

        if (it != cfg.downlinkConfigs.end())
            cfg.downlinkConfigs.erase(it);

        databaseManager->setSatellite(cfg);
        return OCS_PROCESSED;
    });

    // Start it for real in a thread
    serverThreadObj = std::make_shared<std::thread>(&serverThread);
    logger->info("Web server started!");
}

void stopWebServer()
{
    // Stop and wait for it to exit
    webServer->listenStop();
    if (serverThreadObj->joinable())
        serverThreadObj->join();
    logger->info("Web server stopped!");
}