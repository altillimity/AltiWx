#include "database.h"
#include "logger/logger.h"

const std::string createTLETable = "CREATE TABLE IF NOT EXISTS TLE ("
                                   "NORAD INT PRIMARY KEY  NOT NULL,"
                                   "NAME           TEXT    NOT NULL,"
                                   "LINE1          TEXT    NOT NULL,"
                                   "LINE2          TEXT    NOT NULL,"
                                   "DATE           BIGINT  NOT NULL"
                                   ");";

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