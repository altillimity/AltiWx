#include "tle_fetcher.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <sstream>
#include <curlpp/Easy.hpp>
#include "logger/logger.h"

TLEFetcher::TLEFetcher(int norad) : norad_m(norad), data(false)
{
}

TLE TLEFetcher::getTLE()
{
    return tle;
}

bool TLEFetcher::containsData()
{
    return data;
}

void TLEFetcher::fetch()
{
    // Init CurlPP objects
    curlpp::Easy fetchCelestrakRequest;
    std::ostringstream requestContent;

    // Generate URL from NORAD
    std::string fetchUrl = "https://www.celestrak.com/satcat/tle.php?CATNR=" + std::to_string(norad_m);
    logger->debug("TLEFetcher URL - " + fetchUrl);

    // Redirect output into out stream + set URL
    fetchCelestrakRequest.setOpt(curlpp::options::Url(fetchUrl));
    fetchCelestrakRequest.setOpt(curlpp::options::WriteStream(&requestContent));

    // Perform the request
    try
    {
        fetchCelestrakRequest.perform();
    }
    catch (std::exception &e)
    {
        logger->debug(e.what());
        data = false;
        return;
    }

    // Get the result into a string
    std::string contentString = requestContent.str();

    // Parse that string
    std::string name = contentString.substr(0, contentString.find('\n') - 1);
    name.erase(std::find_if(name.rbegin(), name.rend(), std::bind1st(std::not_equal_to<char>(), ' ')).base(), name.end());
    contentString.erase(0, contentString.find('\n') + 1);
    std::string tle_1 = contentString.substr(0, contentString.find('\n'));
    contentString.erase(0, contentString.find('\n') + 1);
    std::string tle_2 = contentString.substr(0, contentString.find('\n'));

    logger->debug("TLEFetcher got\n" + name + '\n' + tle_1 + '\n' + tle_2);

    // Build TLE object
    tle = {name, tle_1, tle_2};

    // Say we got data
    data = true;
}