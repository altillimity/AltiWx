#include "logger/logger.h"
#include "dsp/dsp.h"
#include "dsp/modem/modem_qpsk.h"
#include "orbit/tle_manager.h"
#include "orbit/orbit_predictor.h"
#include "scheduler/scheduler.h"
#include "config/config.h"
#include "orbit/pass_manager.h"
#include "processing/pass_processing.h"
#include <fstream>

int main(int argc, char *argv[])
{
    // Start logger first
    initLogger();

    // Nice graphics!
    logger->info("   ___   ____  _ _      __       ");
    logger->info("  / _ | / / /_(_) | /| / /_ __   ");
    logger->info(" / __ |/ / __/ /| |/ |/ /\\ \\ / ");
    logger->info("/_/ |_/_/\\__/_/ |__/|__//_\\_\\ ");
    logger->info("                                 ");

    // Load config file
    initConfig();

    // Register Modems
    registerModems();

    // Set log level
    logger->set_level(configManager->getConfig().log_level);

    // Start SDR
    std::shared_ptr<DeviceDSP> device_dsp = std::make_shared<DeviceDSP>(configManager->getConfig().radio_config.samplerate,
                                                                        configManager->getConfig().radio_config.frequencies[0],
                                                                        configManager->getConfig().radio_config.gain);
    device_dsp->start();

    // Start scheduler
    initScheduler();

    // Start TLE Manager and fetch them now
    TLEManager tle_manager;
    for (SatelliteConfig &satConfig : configManager->getConfig().satellite_configs)
        tle_manager.addNORAD(satConfig.norad);
    tle_manager.updateTLEs();

    // Schedule TLE refresh
    global_scheduler->cron(configManager->getConfig().tle_update_cron, &TLEManager::updateTLEs, &tle_manager);

    // Start pass scheduler, schedule upcoming passes
    SatellitePassManager pass_manager(device_dsp, tle_manager, global_scheduler);
    pass_manager.schedulePasses();

    // Schedule passes scheduling
    global_scheduler->every(std::chrono::system_clock::duration(std::chrono::seconds(24 * 60 * 60)), &SatellitePassManager::schedulePasses, &pass_manager);

    //processSatellitePass({25544, time(NULL), time(NULL) + 10, 10.0f, NORTHBOUND}, device_dsp, tle_manager.getTLE(25544));
    //processSatellitePass({28654, time(NULL), time(NULL) + 10, 10.0f, NORTHBOUND}, device_dsp, tle_manager.getTLE(28654));

    //logger->info(getBandForDownlink(configManager->getConfig().getSatelliteConfig(25544).downlinkConfigs[0]));

    /*
    std::map<std::string, std::string> parameters = {{"file", "meteor.soft"},
                                                     {"agc_rate", "0.1"},
                                                     {"symbolrate", "72000"},
                                                     {"rrc_alpha", "0.6"},
                                                     {"rrc_taps", "31"},
                                                     {"costas_bw", "0.005"},
                                                     {"iq_invert", "true"}};

    std::ifstream meteorFile("/home/alan/Downloads/piedfase-m2-2.raw");
    std::shared_ptr<ModemQPSK> qpsk = std::make_shared<ModemQPSK>(100, 140e3, parameters, 8192);
    qpsk->start(140e3, 100);
    std::complex<float> buffer[8192];
    while (!meteorFile.eof())
    {
        meteorFile.read((char *)buffer, 8192 * sizeof(std::complex<float>));
        qpsk->push(buffer, 8192);
    }
    exit(0);
    */
    /*
    DeviceDSP device_dsp(2.4e6, 101e6, 49);

    device_dsp.start();
    */
    //DeviceDSP device_dsp(2.4e6, 101e6, 49);
    //device_dsp.start();
    //std::map<std::string, std::string> parameters = {{"file", "test10.wav"}, {"audio_samplerate", "48000"}};
    //std::shared_ptr<ModemFM> fm = std::make_shared<ModemFM>(101.9e6, 200e3, parameters, 8192);
    //device_dsp->attachModem("1", fm);
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    //device_dsp->detachModem("1");
    //fm->stop();
    //device_dsp.attachModem("2", std::make_shared<ModemFM>(100.7e6, 200e3, 48e3, "test2.wav"));
    //device_dsp.attachModem("3", std::make_shared<ModemFM>(101.1e6, 200e3, 48e3, "test3.wav"));

    while (1)
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
}
