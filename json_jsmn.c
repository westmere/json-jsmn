#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "json_jsmn.h"

#ifndef assert
#define assert(c)
#endif
#ifndef Log_info
#define Log_info(fmt,args...)
#endif
#ifndef Log_error
#define Log_error(fmt,args...)
#endif

typedef enum { START, KEY, VALUE, SKIP, STOP } parse_state;

/**
 *
 */
int json_token_strcmp(const char *js, jsmntok_t *t, const char *s)
{
    return strncmp(js + t->start, s, t->end - t->start);
}

static int jsmn_object_size(jsmntok_t *t, size_t count)
{
	int i, j;

	if (count == 0)
	{
		return 0;
	}

	switch(t->type)
	{
	case JSMN_OBJECT:
		// bypass
		j = 0;
		for (i = 0; i < t->size; i++)
		{
			// name
			j+= jsmn_object_size(t+1+j, count-j);

			//value
			j+= jsmn_object_size(t+1+j, count-j);
		}
		return j+1;
	case JSMN_STRING:
	case JSMN_PRIMITIVE:
		return 1;
	case JSMN_ARRAY:
		j = 0;
		for (i = 0; i < t->size; i++)
		{
			j+= jsmn_object_size(t+1+j, count-j);
		}
		return j+1;
	default:
		return 0;
	}
}

int json_jsmn_parse(const char *js, jsmntok_t *tokens, unsigned int token_count, const char **json_jsmntok_keys, json_jsmntok_t *json_jsmntok, int json_jsmntok_count)
{
	int i, j, k, n;

    parse_state state = START;

    int object_tokens = 0;

    for (n = 0, i = 0, j = 1; i < token_count; i += j)
    {
        jsmntok_t *t = &tokens[i];

        // Should never reach uninitialized tokens
        assert(t->start != -1 && t->end != -1);

        switch (state)
        {
            case START:
                if (t->type != JSMN_OBJECT)
                    Log_error("Invalid object(%d): root element must be an object.", t->type);

                if (!t->size)
                {
                	state = START;
                	Log_error("Empty object.");
                }
                else
                {
                	state = KEY;
					object_tokens = t->size;
                }

                j = 1;
                break;

            case KEY:
                object_tokens--;
                j = 1;

                if (t->type != JSMN_STRING)
                    Log_error("Invalid object(%d): object keys must be strings.", t->type);

                state = SKIP;

                if(json_jsmntok_keys)
                {
                    // keys list with null at end
                    for (k = 0; json_jsmntok_keys[k]; k++)
                    {
                        if (0 == json_token_strcmp(js, t, json_jsmntok_keys[k]))
                        {
                            if(n < json_jsmntok_count)
                            {
                                json_jsmntok[n].key = t;
                                state = VALUE;

                            }
                            break;
                        }
                    }
                }
                else if(n < json_jsmntok_count)
                {
                    json_jsmntok[n].key = t;
                    state = VALUE;
                }

                if(state == SKIP)
				{
					Log_info("skip token: %.*s", t->end - t->start, js+t->start);
				}
				else
				{
					Log_info("add token: %.*s", t->end - t->start, js+t->start);
				}

                break;

            case SKIP:
            	j = jsmn_object_size(t, token_count-j);
//            	object_tokens -= 1;
                state = KEY;

                if (object_tokens == 0)
                    state = START;

                break;

            case VALUE:
            	j = jsmn_object_size(t, token_count-j);
//				object_tokens -= 1;

            	if(n < json_jsmntok_count)
                {
					json_jsmntok[n].value = t;
					json_jsmntok[n].count = j;
					n++;
            	}

                state = KEY;

                if (object_tokens == 0)
                    state = START;

                break;

            case STOP:
                // Just consume the tokens
                break;

            default:
                Log_error("Invalid state %u", state);
        }
    }
//    assert_fmt(n <= json_jsmntok_count, "invalid return (%d)", n);
    return n;
}

static int jsmn_get_value(const char *js, jsmntok_t *t, size_t count, void *out, int size)
{
	int i, j;

	if (count == 0)
	{
		return 0;
	}

	switch(t->type)
	{
	case JSMN_STRING:
		if(out && size)
		{
			i = t->end - t->start + 1;
			if(size > i)
				size = i;
			strlcpy((char *)out, js + t->start, size);
		}
		return 1;
	case JSMN_PRIMITIVE:
		if(out && size)
		{
			long n;

			if(0 == strncmp(js + t->start, "true", 4))
			{
				n = 1;
			}
			else if(0 == strncmp(js + t->start, "true", 4))
			{
				n = 0;
			}
			else
			{
				n = strtol(js + t->start, NULL, 10);
			}

			switch(size)
			{
				case sizeof(uint64_t):
				{
					int64_t value = n;
					memcpy(out, &value, size);
				}
				break;

				case sizeof(int32_t):
				{
					int32_t value = n;
					memcpy(out, &value, size);
				}
				break;

				case sizeof(int16_t):
				{
					int16_t value = n;
					memcpy(out, &value, size);
				}

				case sizeof(int8_t):
				{
					int8_t value = n;
					memcpy(out, &value, size);
				}
			}
		}
		return 1;
	case JSMN_OBJECT:
		// bypass
		j = 0;
		for (i = 0; i < t->size; i++)
		{
			// name
			j+= jsmn_get_value(js, t+1+j, count-j, NULL, 0);

			//value
			j+= jsmn_get_value(js, t+1+j, count-j, NULL, 0);
		}
		return j+1;
	case JSMN_ARRAY:
		j = 0;
		for (i = 0; i < t->size; i++)
		{
			j+= jsmn_get_value(js, t+1+j, count-j, NULL, 0);
		}
		return j+1;
	default:
		return 0;
	}
}

int json_jsmn_parse_object(const char *js, jsmntok_t *tokens, unsigned int token_count, json_object_t *objs, int objs_count)
{
	int i, j, k, n;
	parse_state state = START;
	int object_tokens = 0;

	for(i=0;i<objs_count;i++)
		objs[i].status = JSON_JSMN_EMPTY;

	for (n = 0, i = 0, j = 1, k=0; i < token_count; i += j)
	{
		jsmntok_t *t = &tokens[i];

		// Should never reach uninitialized tokens
		assert(t->start != -1 && t->end != -1);

		switch (state)
		{
			case START:
				if (t->type != JSMN_OBJECT)
					Log_error("Invalid object(%d): root element must be an object.", t->type);

				if (!t->size){
					state = START;
					Log_error("Empty object.");
				}
				else{
					state = KEY;
					object_tokens = t->size;
				}

				j = 1;
				break;

			case KEY:
				object_tokens--;
				j = 1;

				if (t->type != JSMN_STRING)
					Log_error("Invalid object(%d): object keys must be strings.", t->type);

				state = SKIP;

				// keys list with null at end
				for (k = 0; k < objs_count; k++)
				{
					if (0 == json_token_strcmp(js, t, objs[k].key))
					{
						state = VALUE;
						break;
					}
				}

				if(state == SKIP)
				{
					Log_info("skip token: %.*s", t->end - t->start, js+t->start);
				}
				else
				{
					Log_info("add token: %.*s", t->end - t->start, js+t->start);
				}

				break;

			case SKIP:
				j = jsmn_get_value(js, t, token_count-j, NULL, 0);
//            	object_tokens -= 1;
				state = KEY;

				if (object_tokens == 0)
					state = START;
				break;

			case VALUE:
				j = jsmn_get_value(js, t, token_count-j, objs[k].value, objs[k].size);
//				object_tokens -= 1;

				if(objs[k].type == t->type)
				{
					objs[k].status = JSON_JSMN_VALID;
                    n++;
				}
				else
				{
					objs[k].status = JSON_JSMN_INVALID;
				}

				state = KEY;

				if (object_tokens == 0)
					state = START;

				break;

			case STOP:
				// Just consume the tokens
				break;

			default:
				Log_error("Invalid state %u", state);
		}
	}
//    assert_fmt(n <= json_jsmntok_count, "invalid return (%d)", n);
	return n;
}

