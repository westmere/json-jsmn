#ifndef __JSON_JSMN_H_
#define __JSON_JSMN_H_

#include <stddef.h>
#include "jsmn/jsmn.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	jsmntok_t *key;
	jsmntok_t *value;
	int count;
}json_jsmntok_t;

enum jsonstatus
{
	JSON_JSMN_EMPTY,
	JSON_JSMN_VALID,
	JSON_JSMN_INVALID
};

typedef struct{
    const char *key;
    void *value;
    int size;
    jsmntype_t type;
    enum jsonstatus status;
}json_object_t;

int json_jsmn_parse(const char *js, jsmntok_t *tokens, unsigned int token_count, const char **json_jsmntok_keys, json_jsmntok_t *json_jsmntok, int json_jsmntok_count);

int json_jsmn_parse_object(const char *js, jsmntok_t *tokens, unsigned int token_count, json_object_t *objs, int objs_count);

#ifdef __cplusplus
}
#endif

#endif /* __JSON_JSMN_H_ */
