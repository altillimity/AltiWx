#include "logger/logger.h"
#include "dsp/dsp.h"

#include "dsp/modem/modem_fm.h"

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

    DeviceDSP device_dsp(2.4e6, 101e6, 49);

    device_dsp.start();

    device_dsp.attachModem("1", std::make_shared<ModemFM>(100.3e6, 200e3, 48e3, "test1.wav"));
    device_dsp.attachModem("2", std::make_shared<ModemFM>(100.7e6, 200e3, 48e3, "test2.wav"));
    device_dsp.attachModem("3", std::make_shared<ModemFM>(101.1e6, 200e3, 48e3, "test3.wav"));

    while (1)
        sleep(1);
}
