set(SPDLOG_DEBUG_ON true)
set(SPDLOG_TRACE_ON true)

find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
  # pull nlohmann json as dependency
  message(STATUS "nlohmann_json NOT FOUND, fetching the release package")
  include(FetchContent)
  set(JSON_Install ON)
  FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
  FetchContent_MakeAvailable(json)
endif()

set(OLINK_SOURCES
    olink/core/basenode.cpp
    olink/core/protocol.cpp
    olink/core/types.cpp
    olink/consolelogger.cpp
    olink/clientnode.cpp
    olink/clientregistry.cpp
    olink/remotenode.cpp
    olink/remoteregistry.cpp 
    )

SET(OLINK_HEADERS
    olink/core/basenode.h
    olink/core/olink_common.h
    olink/core/protocol.h
    olink/core/types.h
    olink/core/uniqueidobjectstorage.h
    olink/clientnode.h
    olink/clientregistry.h
    olink/consolelogger.h
    olink/iclientnode.h
    olink/iobjectsink.h
    olink/iobjectsource.h
    olink/iremotenode.h
    olink/remotenode.h
    olink/remoteregistry.h
    )


add_library (olink_core STATIC ${OLINK_SOURCES} ${OLINK_HEADERS})

target_include_directories (olink_core 
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(olink_core PUBLIC nlohmann_json::nlohmann_json)

# install binary files
install(TARGETS olink_core
        EXPORT objectlink-core-cppConfig
        RUNTIME DESTINATION bin COMPONENT Runtime
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib/objectlink-core-cpp COMPONENT Development)
# install includes
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/olink DESTINATION include FILES_MATCHING PATTERN "*.h")
include(CMakePackageConfigHelpers)
export(TARGETS
    olink_core
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/objectlink-core-cppConfig.cmake"
)
install(EXPORT
    objectlink-core-cppConfig
    DESTINATION "lib/objectlink-core-cpp/cmake"
)
