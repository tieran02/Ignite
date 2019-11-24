//
// Created by Tieran on 24/11/2019.
//

#include "ignite/Log.h"

namespace Ignite{

    std::shared_ptr<spdlog::logger> Log::s_CoreLogger = nullptr;

    void Log::Init()
    {
        spdlog::set_pattern("%^[%T} %n: %v%$");
        s_CoreLogger = spdlog::stdout_color_mt("IGNITE");
        s_CoreLogger->set_level(spdlog::level::trace);
    }
}