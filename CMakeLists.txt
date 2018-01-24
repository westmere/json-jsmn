cmake_minimum_required(VERSION 2.8)


add_library(json-jsmn-object OBJECT jsmn/jsmn.c)
# target_compile_definitions(json-jsmn-object PRIVATE MQTT_SERVER MQTT_CLIENT)

include_directories(${PAHO_MQTT_SDK}/MQTTPacket/src)
