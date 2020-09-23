#include "igpch.h"
#include "Ignite/Log.h"

namespace Ignite
{
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger = nullptr;

    void Log::Init()
    {
        spdlog::set_pattern("%^[%T} %n: %v%$");
        s_CoreLogger = spdlog::stdout_color_mt("IGNITE");
		#ifdef IGCORE_DEBUG
        s_CoreLogger->set_level(spdlog::level::trace);
		#else
        s_CoreLogger->set_level(spdlog::level::info);
		#endif
    }

    void Log::Clean()
    {
        s_CoreLogger.reset();
    }

    std::shared_ptr<spdlog::logger> Log::GetCoreLogger()
    {
        assert(s_CoreLogger);
        return s_CoreLogger;
    }
}