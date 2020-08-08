#pragma once

#include "spdlog/sinks/base_sink.h"

extern std::vector<std::string> logs;

template <typename Mutex>
class webSink : public spdlog::sinks::base_sink<Mutex>
{
protected:
    void sink_it_(const spdlog::details::log_msg &msg) override
    {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

        std::string output;
        output = std::string(formatted.begin(), formatted.begin() + msg.color_range_start);

        if (msg.level == SPDLOG_LEVEL_TRACE)
            output += "<label style=\"color: grey\">";
        else if (msg.level == SPDLOG_LEVEL_DEBUG)
            output += "<label style=\"color: #006699\">";
        else if (msg.level == SPDLOG_LEVEL_INFO)
            output += "<label style=\"color: green\">";
        else if (msg.level == SPDLOG_LEVEL_WARN)
            output += "<label style=\"color: yellow\">";
        else if (msg.level == SPDLOG_LEVEL_ERROR)
            output += "<label style=\"color: red\">";
        else if (msg.level == SPDLOG_LEVEL_CRITICAL)
            output += "<label style=\"color: orange\">";

        output += std::string(formatted.begin() + msg.color_range_start, formatted.begin() + msg.color_range_end);
        output += "</label>";
        output += std::string(formatted.begin() + msg.color_range_end, formatted.end());

        logs.push_back(output);
        if (logs.size() > 1000)
            logs.erase(logs.begin());
    }

    void flush_() override
    {
        //std::cout << std::flush;
    }
};

#include "spdlog/details/null_mutex.h"
#include <mutex>
using webSink_mt = webSink<std::mutex>;
using webSink_st = webSink<spdlog::details::null_mutex>;