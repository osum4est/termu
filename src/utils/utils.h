//
// Created by Forrest Jones on 2019-02-24.
//

#ifndef TERMU_UTILS_H
#define TERMU_UTILS_H


#include <string>
#include <stdio.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

class utils {
    class only_trace_format : public spdlog::formatter {
    public:
        void format(const spdlog::details::log_msg &msg, fmt::memory_buffer &dest) override {
            if (msg.level == spdlog::level::trace) {
                dest.append(msg.payload.data(), msg.payload.data() + msg.payload.size());
                dest.push_back('\n');
            }
        }

        std::unique_ptr<formatter> clone() const override {
            return std::make_unique<utils::only_trace_format>();
        }
    };

public:
    static std::unique_ptr<spdlog::logger> get_logger(const std::string &name) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::info);

        std::string log_file = "./termu_" + name + ".log";
        std::remove(log_file.c_str());

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file);
        file_sink->set_formatter(std::make_unique<utils::only_trace_format>());
        file_sink->set_level(spdlog::level::trace);

        return std::make_unique<spdlog::logger>(name, (spdlog::sinks_init_list) {console_sink, file_sink});
    }

    template<typename ... T>
    static std::string string_format(const std::string &format, T ... args) {
        auto size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
        std::unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), static_cast<size_t>(size), format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1);
    }

    static std::string to_upper(const std::string &str) {
        std::string cpy = str;
        std::transform(cpy.begin(), cpy.end(), cpy.begin(), ::toupper);
        return cpy;
    }
};


#endif //TERMU_UTILS_H
