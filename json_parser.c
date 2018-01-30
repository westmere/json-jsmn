#include "json_jsmn.h"
 
int json_parse(jsmn_parser *parser, const char *js, unsigned int jslen, jsmntok_t *tokens, int tokcount)
{
    rc = jsmn_parse(parser, js, jslen, tokens, tokcount);
    if (0 > rc)
    {
        switch(rc)
        {
            /* Not enough tokens were provided */
        case JSMN_ERROR_NOMEM:
            printf("jsmn_parse() return JSMN_ERROR_NOMEM\n");
            return -ENOMEM;
            /* Invalid character inside JSON string */
        case JSMN_ERROR_INVAL:
            /* The string is not a full JSON packet, more bytes expected */
            printf("jsmn_parse() return JSMN_ERROR_INVAL\n");
            return -EINVAL;
        case JSMN_ERROR_PART:
            printf("jsmn_parse() return JSMN_ERROR_PART\n");
            break;
        default:
            printf("jsmn_parse() return error: %d\n", rc);
            return -1;
        }
    }
    return rc;
}
 
int json_parse_object(const char *js, unsigned int jslen, jsmntok_t *tokens, int tokcount, json_object_t *json_objects, int json_object_count)
{
	int i,rc;
	jsmn_parser jsmn_parser_object;

	jsmn_init(&jsmn_parser_object);
    
    rc = json_parse(&jsmn_parser_object, js, jslen, tokens, tokcount);
    if(0 > rc)
        return rc;
    
	rc=json_jsmn_parse_object(js, tokens, jsmn_parser_object.toknext, json_objects, json_object_count);

	for(i=0;i<json_object_count;i++)
	{
		if(json_objects[i].status == JSON_JSMN_VALID)
		{
			if(json_objects[i].type == JSMN_STRING)
			{
				printf("jsmn_parse(): %s:%s\r\n", json_objects[i].key, json_objects[i].value);
			}
			else if(json_objects[i].type == JSMN_PRIMITIVE)
			{
				long value;

				memset(&value, 0, sizeof(value));
				memcpy(&value, json_objects[i].value, json_objects[i].size > sizeof(value) ? sizeof(value):json_objects[i].size);
				printf("jsmn_parse(): %s:%d\r\n", json_objects[i].key, value);
			}
		}
	}

	return 0;
}

int json_parse_array(const char *js, unsigned int jslen, jsmntok_t *tokens, int tokcount, const char *name, json_array_element_callback_t callback)
{
	int i,j,rc;
	jsmn_parser jsmn_parser_object;
	const char *json_jsmntok_keys[2];
	json_jsmntok_t json_jsmntok;

	jsmn_init(&jsmn_parser_object);

    rc = json_parse(&jsmn_parser_object, js, jslen, tokens, tokcount);
    if(0 > rc)
        return rc;

	json_jsmntok_keys[0] = name;
	json_jsmntok_keys[1] = NULL;
	rc = json_jsmn_parse(js, tokens, jsmn_parser_object.toknext, json_jsmntok_keys, &json_jsmntok, 1);

	if(rc == 1 && json_jsmntok.value->type == JSMN_ARRAY)
	{
		for(rc=0,i=1;i<json_jsmntok.count;i++)
		{
			if(callback)
			{
				int r = (*callback)(i-1,
									json_jsmntok.value[i].type,
									(void *)(js + json_jsmntok.value[i].start),
									json_jsmntok.value[i].end-json_jsmntok.value[i].start);
				if(!r)
				{
					rc++;
				}
			}
			array_element_print(js, i-1, json_jsmntok.key, &json_jsmntok.value[i]);
		}
	}
	else
	{
		printf("jsmn_parse(): invalid array: %d\n", rc);
		rc = -1;
	}

	return rc;
}
