file(GLOB_RECURSE SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/IgniteRenderer/include/)
include_directories(${CMAKE_SOURCE_DIR}/IgniteRenderer/vendor/glm)

add_executable(Sandbox ${SOURCE})
target_link_libraries(Sandbox LINK_PUBLIC IgniteRenderer)