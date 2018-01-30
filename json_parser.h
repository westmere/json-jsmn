#ifndef __JON_PARSER_H_
#define __JON_PARSER_H_

#include "json_jsmn.h" 

#ifdef __cplusplus
extern "C" {
#endif
    
typedef int (*json_array_element_callback_t)(int index, jsmntype_t type, void *value, int len);

int json_parse_object(const char *js, unsigned int jslen, jsmntok_t *tokens, int tokcount, json_object_t *json_objects, int json_object_count);

int json_parse_array(const char *js, unsigned int jslen, jsmntok_t *tokens, int tokcount, const char *name, json_array_element_callback_t callback);

    
#ifdef __cplusplus
}
#endif

#endif  // __JON_PARSER_H_
