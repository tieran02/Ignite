#pragma once
#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace Ignite
{
    class Log
        {
        public:
            static void Init();

            static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

        private:
            static std::shared_ptr<spdlog::logger> s_CoreLogger;
        };
}

//Renderer log macros
#define LOG_CORE_TRACE(...) ::Ignite::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...) ::Ignite::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARNING(...) ::Ignite::Log::GetCoreLogger()->warning(__VA_ARGS__)
#define LOG_CORE_ERROR(...) ::Ignite::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...) ::Ignite::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define CORE_ASSERT(x, ...) { if(!(x)) { LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }