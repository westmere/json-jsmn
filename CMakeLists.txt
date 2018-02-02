cmake_minimum_required(VERSION 2.8)

option(USE_STATIC_JSON_JSMN_LIBRARY "Build json_jsmn static library." OFF)
option(USE_OBJECT_JSON_JSMN_LIBRARY "Build json_jsmn object library." ON)

if(USE_OBJECT_JSON_JSMN_LIBRARY)
    add_library(json-jsmn-object OBJECT jsmn/jsmn.c json_jsmn.c json_parser.c)
endif(USE_OBJECT_JSON_JSMN_LIBRARY)

if(USE_STATIC_JSON_JSMN_LIBRARY)
    add_library(json-jsmn-static STATIC jsmn/jsmn.c json_jsmn.c json_parser.c)
    # target_compile_definitions(json-jsmn-object PRIVATE MQTT_SERVER MQTT_CLIENT)

    target_include_directories(json-jsmn-static PUBLIC ${CMAKE_CURRENT_LIST_DIR})
endif(USE_STATIC_JSON_JSMN_LIBRARY)
