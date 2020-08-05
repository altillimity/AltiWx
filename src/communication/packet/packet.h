#pragma once

#include <unordered_map>
#include <string>
#include "libs/nlohmann/json.h"
#include <memory>

void registerPackets();

class Packet
{
public:
    static std::string getID();
    virtual std::string process(nlohmann::json& inputData) = 0;
};

extern std::unordered_map<std::string, std::shared_ptr<Packet>> packtMap;