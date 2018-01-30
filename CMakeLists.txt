cmake_minimum_required(VERSION 2.8)


add_library(json-jsmn-object OBJECT jsmn/jsmn.c json_jsmn.c json_parser.c)
add_library(json-jsmn-static STATIC jsmn/jsmn.c json_jsmn.c json_parser.c)
# target_compile_definitions(json-jsmn-object PRIVATE MQTT_SERVER MQTT_CLIENT)

target_include_directories(json-jsmn-static PUBLIC ${CMAKE_CURRENT_LIST_DIR})
