#include "database.h"
#include "logger/logger.h"
#include "communication/nlohmann/json.h"

const std::string createTLETable = "CREATE TABLE IF NOT EXISTS TLE ("
                                   "NORAD INT PRIMARY KEY  NOT NULL,"
                                   "NAME           TEXT    NOT NULL,"
                                   "LINE1          TEXT    NOT NULL,"
                                   "LINE2          TEXT    NOT NULL,"
                                   "DATE           BIGINT  NOT NULL"
                                   ");";

const std::string createSatelliteTable = "CREATE TABLE IF NOT EXISTS satellites ("
                                         "norad INT PRIMARY KEY         NOT NULL,"
                                         "min_elevation INT             NOT NULL,"
                                         "priority      INT             NOT NULL,"
                                         "downlinks     JSONB"
                                         ");";

const std::string testDownlinkType = "SELECT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'downlink');";

std::shared_ptr<DatabaseManager> databaseManager;

void initDatabaseManager()
{
    databaseManager = std::make_shared<DatabaseManager>(configManager->getConfig().databaseConfig);
    databaseManager->init();
}

DatabaseManager::DatabaseManager(DBConfig &config)
{
    logger->info("Connection to PostgreSQL database " + config.database + " at " + config.address + ":" + std::to_string(config.port));
    try
    {
        std::string connectionString = "dbname=" + config.database + " user=" + config.username + " password=" + config.password + " hostaddr=" + config.address + " port=" + std::to_string(config.port);
        logger->debug(connectionString);
        databaseConnection = std::make_shared<pqxx::connection>(connectionString);
    }
    catch (const std::exception &e)
    {
        logger->critical(e.what());
        exit(1);
    }
    logger->info("Connected to PostgreSQL successfully!");
}

DatabaseManager::~DatabaseManager()
{
    databaseConnection->disconnect();
}

bool DatabaseManager::runQuery(std::string sql)
{
    try
    {
        pqxx::work statementWork(*databaseConnection);
        logger->trace(sql);
        statementWork.exec(sql);
        statementWork.commit();
    }
    catch (const std::exception &e)
    {
        logger->error(e.what());
        return false;
    }
    return true;
}

pqxx::result DatabaseManager::runQueryGet(std::string sql)
{
    pqxx::result r;
    try
    {
        pqxx::nontransaction statementWork(*databaseConnection);
        logger->trace(sql);
        r = statementWork.exec(sql);
        statementWork.commit();
    }
    catch (const std::exception &e)
    {
        logger->error(e.what());
        return r;
    }
    return r;
}

void DatabaseManager::init()
{
    runQuery(createTLETable);
    runQuery(createSatelliteTable);
}

// TLE Stuff
#define SET_TLE_QUERY(norad, name, tle1, tle2, age) "INSERT INTO tle (NORAD,NAME,LINE1,LINE2,DATE) VALUES ( " + norad + ", '" + name + "', '" + tle1 + "', '" + tle2 + "', " + age + " ) ON CONFLICT (NORAD) DO UPDATE SET NORAD = excluded.NORAD, NAME = excluded.NAME, LINE1 = excluded.LINE1, LINE2 = excluded.LINE2, DATE = excluded.DATE;"
#define TLE_EXISTS_QUERY(norad) "SELECT EXISTS(SELECT 1 FROM tle WHERE norad=" + norad + ");";
#define GET_TLE_QUERY(norad) "SELECT NAME,LINE1,LINE2,DATE FROM tle WHERE norad=" + norad + ";";

void DatabaseManager::setTLE(int norad, TLE tle, time_t age)
{
    std::string sql = SET_TLE_QUERY(std::to_string(norad), tle.name, tle.tle_1, tle.tle_2, std::to_string(age));
    runQuery(sql);
}

bool DatabaseManager::tleExists(int norad)
{
    std::string sql = TLE_EXISTS_QUERY(std::to_string(norad));
    pqxx::result result = runQueryGet(sql);
    return !result.empty() ? result[0][0].as<bool>() : false;
}

std::pair<TLE, time_t> DatabaseManager::getTLE(int norad)
{
    TLE tle;
    time_t time;
    std::string sql = GET_TLE_QUERY(std::to_string(norad));
    pqxx::result result = runQueryGet(sql);
    if (!result.empty())
    {
        std::string name, tle1, tle2;
        name = result[0][0].as<std::string>();
        tle1 = result[0][1].as<std::string>();
        tle2 = result[0][2].as<std::string>();
        tle = {name, tle1, tle2};
        time = result[0][3].as<time_t>();
    }
    return std::make_pair(tle, time);
}

// Satellite Stuff
#define SET_SAT_QUERY(norad, min_elevation, priority, downlinks) "INSERT INTO satellites (norad, min_elevation, priority, downlinks) VALUES (" + norad + ", " + min_elevation + ", " + priority + ", '" + downlinks + "'::jsonb) ON CONFLICT (NORAD) DO UPDATE SET NORAD = excluded.NORAD, min_elevation = excluded.min_elevation, priority = excluded.priority, downlinks = excluded.downlinks;"
#define GET_NORADS_QUERY "SELECT norad FROM satellites;";
#define GET_SATELLITE_QUERY(norad) "SELECT norad, min_elevation, priority, downlinks FROM satellites WHERE norad=" + norad + ";";
#define GET_ALL_SATELLITES_QUERY "SELECT norad, min_elevation, priority, downlinks FROM satellites;";

void DatabaseManager::setSatellite(SatelliteConfig &satConfig)
{
    nlohmann::json downlinks;
    for (DownlinkConfig &downlink : satConfig.downlinkConfigs)
    {
        downlinks[downlink.name]["radio"] = downlink.radio;
        downlinks[downlink.name]["type"] = modemTypeToString(downlink.modemType);
        downlinks[downlink.name]["frequency"] = downlink.frequency;
        downlinks[downlink.name]["bandwidth"] = downlink.bandwidth;
        downlinks[downlink.name]["doppler"] = downlink.dopplerCorrection;
        downlinks[downlink.name]["output_extension"] = downlink.outputExtension;
        downlinks[downlink.name]["processing_script"] = downlink.postProcessingScript;
        if (downlink.modemType == FM)
            downlinks[downlink.name]["modem_audio_sample_rate"] = downlink.modem_audioSamplerate;
        if (downlink.modemType == QPSK)
            downlinks[downlink.name]["modem_qpsk_symbol_rate"] = downlink.modem_symbolRate;
    }
    std::string sql = SET_SAT_QUERY(std::to_string(satConfig.norad), std::to_string(satConfig.min_elevation), std::to_string(satConfig.priority), downlinks.dump());
    runQuery(sql);
}

std::vector<int> DatabaseManager::getAllNORADs()
{
    std::vector<int> norads;
    std::string sql = GET_NORADS_QUERY;
    pqxx::result result = runQueryGet(sql);
    if (!result.empty())
    {
        for (pqxx::result::const_iterator current : result)
        {
            norads.push_back(current[0].as<int>());
        }
    }
    return norads;
}

SatelliteConfig DatabaseManager::getSatellite(int norad)
{
    SatelliteConfig satellite;
    std::string sql = GET_SATELLITE_QUERY(std::to_string(norad));
    pqxx::result result = runQueryGet(sql);
    if (!result.empty())
    {
        satellite.norad = result[0][0].as<int>();
        satellite.min_elevation = result[0][1].as<int>();
        satellite.priority = result[0][2].as<int>();
        nlohmann::json downlinksJson = nlohmann::json::parse(result[0][3].as<std::string>());
        for (nlohmann::detail::iteration_proxy_value<nlohmann::detail::iter_impl<nlohmann::json>> downlink : downlinksJson.items())
        {
            DownlinkConfig downlinkConf;
            downlinkConf.name = (std::string)downlink.key();

            std::string type = (std::string)downlink.value()["type"];
            if (type == "FM")
                downlinkConf.modemType = ModemType::FM;
            else if (type == "IQ")
                downlinkConf.modemType = ModemType::IQ;
            else if (type == "IQWAV")
                downlinkConf.modemType = ModemType::IQWAV;
            else if (type == "QPSK")
                downlinkConf.modemType = ModemType::QPSK;

            downlinkConf.radio = (std::string)downlink.value()["radio"];
            downlinkConf.frequency = (long)downlink.value()["frequency"];
            downlinkConf.bandwidth = (long)downlink.value()["bandwidth"];
            downlinkConf.dopplerCorrection = (bool)downlink.value()["doppler"];
            downlinkConf.outputExtension = (std::string)downlink.value()["output_extension"];
            downlinkConf.radio = (std::string)downlink.value()["processing_script"];

            if (downlinkConf.modemType == FM)
                downlinkConf.modem_audioSamplerate = (long)downlink.value()["modem_audio_sample_rate"];
            if (downlinkConf.modemType == QPSK)
                downlinkConf.modem_symbolRate = (long)downlink.value()["modem_qpsk_symbol_rate"];

            satellite.downlinkConfigs.push_back(downlinkConf);
        }
    }
    return satellite;
}

std::vector<SatelliteConfig> DatabaseManager::getAllSatellites()
{
    std::vector<SatelliteConfig> satellites;
    std::string sql = GET_ALL_SATELLITES_QUERY;
    pqxx::result result = runQueryGet(sql);
    if (!result.empty())
    {
        for (pqxx::row currentSat : result)
        {
            SatelliteConfig satellite;
            satellite.norad = currentSat[0].as<int>();
            satellite.min_elevation = currentSat[1].as<int>();
            satellite.priority = currentSat[2].as<int>();
            nlohmann::json downlinksJson = nlohmann::json::parse(currentSat[3].as<std::string>());
            for (nlohmann::detail::iteration_proxy_value<nlohmann::detail::iter_impl<nlohmann::json>> downlink : downlinksJson.items())
            {
                DownlinkConfig downlinkConf;
                downlinkConf.name = (std::string)downlink.key();

                std::string type = (std::string)downlink.value()["type"];
                if (type == "FM")
                    downlinkConf.modemType = ModemType::FM;
                else if (type == "IQ")
                    downlinkConf.modemType = ModemType::IQ;
                else if (type == "IQWAV")
                    downlinkConf.modemType = ModemType::IQWAV;
                else if (type == "QPSK")
                    downlinkConf.modemType = ModemType::QPSK;

                downlinkConf.radio = (std::string)downlink.value()["radio"];
                downlinkConf.frequency = (long)downlink.value()["frequency"];
                downlinkConf.bandwidth = (long)downlink.value()["bandwidth"];
                downlinkConf.dopplerCorrection = (bool)downlink.value()["doppler"];
                downlinkConf.outputExtension = (std::string)downlink.value()["output_extension"];
                downlinkConf.radio = (std::string)downlink.value()["processing_script"];

                if (downlinkConf.modemType == FM)
                    downlinkConf.modem_audioSamplerate = (long)downlink.value()["modem_audio_sample_rate"];
                if (downlinkConf.modemType == QPSK)
                    downlinkConf.modem_symbolRate = (long)downlink.value()["modem_qpsk_symbol_rate"];

                satellite.downlinkConfigs.push_back(downlinkConf);
            }
            satellites.push_back(satellite);
        }
    }
    return satellites;
}