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

int json_jsmn_parse(const char *js, jsmntok_t *tokens, int tokens_count, const char **json_jsmntok_keys, json_jsmntok_t *json_jsmntok, int json_jsmntok_count);

#ifdef __cplusplus
}
#endif

#endif /* __JSON_JSMN_H_ */
