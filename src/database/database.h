#pragma once

#include <memory>
#include <pqxx/pqxx>
#include "config/config.h"
#include "orbit/tle.h"
#include <mutex>

void initDatabaseManager();

class DatabaseManager
{
private:
    std::shared_ptr<pqxx::connection> databaseConnection;
    std::mutex dbMutex;

private:
    bool runQuery(std::string sql);
    pqxx::result runQueryGet(std::string sql);

public:
    DatabaseManager(DBConfig &config);
    void init();
    ~DatabaseManager();

public:
    // TLE Stuff
    void setTLE(int norad, TLE tle, time_t age);
    bool tleExists(int norad);
    std::pair<TLE, time_t> getTLE(int norad);

    // Satellite Stuff
    void setSatellite(SatelliteConfig &satConfig);
    std::vector<int> getAllNORADs();
    SatelliteConfig getSatellite(int norad);
    std::vector<SatelliteConfig> getAllSatellites();
    void deleteSatellite(int norad);
};

extern std::shared_ptr<DatabaseManager> databaseManager;