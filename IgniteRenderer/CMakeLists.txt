file(GLOB_RECURSE SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ignite/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include)

add_library(IgniteRenderer ${SOURCE})