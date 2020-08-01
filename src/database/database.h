#pragma once

#include <memory>
#include <pqxx/pqxx>
#include "config/config.h"
#include "orbit/tle.h"

void initDatabaseManager();

class DatabaseManager
{
private:
    std::shared_ptr<pqxx::connection> databaseConnection;

private:
    bool runQuery(std::string sql);
    pqxx::result runQueryGet(std::string sql);

public:
    DatabaseManager(DBConfig &config);
    void init();
    ~DatabaseManager();

    // TLE Stuff
public:
    void setTLE(int norad, TLE tle, time_t age);
    bool tleExists(int norad);
    std::pair<TLE, time_t> getTLE(int norad);
};

extern std::shared_ptr<DatabaseManager> databaseManager;