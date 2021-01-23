#include "tle.h"
#include "logger/logger.h"
#include "cpp-netlib/httplib.h"

TLE fetchCelestrakTLE(int norad)
{
    TLE tle = {false, "", "", ""};

    logger->info("Fetching TLE for NORAD " + std::to_string(norad));

    httplib::Client http_client("http://www.celestrak.com");
    std::string fetch_url = "/satcat/tle.php?CATNR=" + std::to_string(norad);

    logger->debug("Using URL - http://www.celestrak.com" + fetch_url);

    httplib::Result http_reply = http_client.Get(fetch_url.c_str());

    if (http_reply.error())
    {
        logger->error("Couldn't make the request!");
        return tle;
    }

    logger->debug("HTTP " + std::to_string(http_reply->status));

    if (http_reply->status == 200)
    {
        std::string &reply = http_reply->body;

        std::string name = reply.substr(0, reply.find('\n') - 1);
        name.erase(std::find_if(name.rbegin(), name.rend(), std::bind1st(std::not_equal_to<char>(), ' ')).base(), name.end());
        reply.erase(0, reply.find('\n') + 1);
        std::string tle_1 = reply.substr(0, reply.find('\n'));
        reply.erase(0, reply.find('\n') + 1);
        std::string tle_2 = reply.substr(0, reply.find('\n'));

        logger->debug("Got :\n" + name + '\n' + tle_1 + '\n' + tle_2);

        tle = {true, name, tle_1, tle_2};
    }
    else
    {
        logger->error(http_reply->body);
    }

    return tle;
}