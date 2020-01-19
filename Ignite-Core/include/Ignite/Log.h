#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"
#include "NonCopyable.h"

namespace Ignite
{
    class Log : NonCopyable
    {
    public:
        static std::shared_ptr<spdlog::logger>& GetCoreLogger();
    private:
        static void Init();
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
    };
}

//IRenderer log macros
#define LOG_CORE_TRACE(...) ::Ignite::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...) ::Ignite::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARNING(...) ::Ignite::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...) ::Ignite::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...) ::Ignite::Log::GetCoreLogger()->critical(__VA_ARGS__)

#ifdef IGCORE_DEBUG
	#define CORE_ASSERT(x, ...) { if(!(x)) { LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }	
#else
	#define CORE_ASSERT(x, ...)
#endif // IG_ASSERTIONS

