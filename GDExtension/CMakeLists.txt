cmake_minimum_required(VERSION 3.29)
project(GodotDecTalk)

set(CMAKE_CXX_STANDARD 20)

set(GODOT_CPP_PATH "${PROJECT_SOURCE_DIR}/godot-cpp")

set(DAPI_PATH "${PROJECT_SOURCE_DIR}/dectalk/src/dapi/src")

set(GODOT_CPP_LIB "${GODOT_CPP_PATH}/bin/libgodot-cpp.windows.debug.64.a")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")

include_directories(
        "${GODOT_CPP_PATH}/godot-headers"
        "${GODOT_CPP_PATH}/include"
        "${GODOT_CPP_PATH}/include/core"
        "${GODOT_CPP_PATH}/include/gen"
        "${DAPI_PATH}"
        "${DAPI_PATH}/api"
        "${DAPI_PATH}/include"
        "${DAPI_PATH}/osf"
        BEFORE
)


add_library(GodotDecTalk SHARED library.cpp)
target_link_libraries(GodotDecTalk PRIVATE ${GODOT_CPP_LIB})
target_link_libraries(GodotDecTalk PRIVATE "${PROJECT_SOURCE_DIR}/dectalk/dist/DECtalk.dll")
