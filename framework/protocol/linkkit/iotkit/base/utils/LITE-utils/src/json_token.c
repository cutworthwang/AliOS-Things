/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include "lite-utils_internal.h"
#include "json_parser.h"

int contain_arr(const char *src, int src_len, const char **arr_pre)
{
    int         i = 0;
    int         ret = -1;
    int         deep = 0;
    const char *pre = NULL;

    if (NULL == src || NULL == arr_pre || src_len <= 0) {
        return -1;
    }

    *arr_pre = NULL;
    for (i = 0; i < src_len; ++i) {
        switch (src[i]) {
            case '[': {
                if (deep != 0) {
                    return ret;
                }
                ++deep;
                if (!pre) {
                    pre = &src[i];
                }
            }
            break;
            case ']': {
                if (deep != 1) {
                    return ret;
                }
                --deep;
                if ('[' == src[i - 1]) {
                    return ret;
                }
            }
            break;
            default: {
                if ((pre != NULL) && (0 == deep)) {
                    return ret;
                }
            }
            break;
        }
    }
    if ((NULL != pre) && (pre < src + src_len) && (pre >= src)) {
        *arr_pre = pre;
        ret = 0;
    }
    return ret;
}

static list_head_t *_LITE_json_keys_of_ext(int type, char *src, int src_len, char *prefix, ...)
{
    static          LIST_HEAD(keylist);
    char           *module_name = NULL;
    char           *iter_pre = NULL;
    char           *pos = 0, *key = 0, *val = 0;
    int             klen = 0, vlen = 0, vtype = 0;
    int             magic = 0;
    unsigned int    count = 1;
    unsigned int    mlen = 0;

    if (src == NULL || prefix == NULL) {
        return NULL;
    }

#if WITH_MEM_STATS_PER_MODULE
    va_list         ap;
    va_start(ap, prefix);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    if (!strcmp("", prefix)) {
        INIT_LIST_HEAD(&keylist);
    }

    if (JOBJECT == type) {
        json_object_for_each_kv(src, src_len, pos, key, klen, val, vlen, vtype) {
            if (key && klen && val && vlen) {

                json_key_t     *entry = NULL;

                entry =  LITE_malloc(sizeof(json_key_t), magic, module_name);

                if (NULL == entry) {
                    log_err("LITE_malloc failed!");
                    return NULL;
                }
                memset(entry, 0, sizeof(json_key_t));

                mlen = strlen(prefix) + klen + 1;
                if (module_name) {
                    entry->key = LITE_format_string(mlen, "%s%.*s", magic, module_name, prefix, klen, key);
                } else {
                    entry->key = LITE_format_string(mlen, "%s%.*s", prefix, klen, key);
                }

                if (NULL == entry->key) {
                    LITE_free(entry);
                    return NULL;
                }

                list_add_tail(&entry->list, &keylist);

                if (JOBJECT == vtype) {
                    mlen = strlen(prefix) + klen + 2;
                    if (module_name) {
                        iter_pre = LITE_format_string(mlen, "%s%.*s.", magic, module_name, prefix, klen, key);
                    } else {
                        iter_pre = LITE_format_string(mlen, "%s%.*s.", prefix, klen, key);
                    }
                    if (NULL == iter_pre) {
                        return NULL;
                    }

                    _LITE_json_keys_of_ext(vtype, val, vlen, iter_pre, magic, module_name);
                    LITE_free(iter_pre);
                } else if (JARRAY == vtype) {
                    mlen = strlen(prefix) + klen + 1;
                    if (module_name) {
                        iter_pre = LITE_format_string(mlen, "%s%.*s", magic, module_name, prefix, klen, key);
                    } else {
                        iter_pre = LITE_format_string(mlen, "%s%.*s", prefix, klen, key);
                    }
                    if (NULL == iter_pre) {
                        return NULL;
                    }

                    _LITE_json_keys_of_ext(vtype, val, vlen, iter_pre, magic, module_name);
                    LITE_free(iter_pre);
                }
            }
        }
    } else if (JARRAY == type) {
        json_array_for_each_entry(src, src_len, pos, val, vlen, vtype) {
            if (val && vlen) {

                json_key_t     *entry = NULL;
                unsigned int    tmp = 0;
                unsigned int    arridxlen = 0;
                entry = LITE_malloc(sizeof(json_key_t), magic, module_name);
                if (NULL == entry) {
                    log_err("LITE_malloc failed!");
                    return NULL;
                }
                memset(entry, 0, sizeof(json_key_t));

                tmp = count;
                do {
                    tmp /= 10;
                    ++arridxlen;
                } while (tmp);
                mlen = strlen("%s[%d]") + strlen(prefix) + arridxlen;
                if (module_name) {
                    entry->key = LITE_format_string(mlen, "%s[%d]", magic, module_name, prefix, count);
                } else {
                    entry->key = LITE_format_string(mlen, "%s[%d]", prefix, count);
                }
                if (NULL == entry->key) {
                    LITE_free(entry);
                    return NULL;
                }

                list_add_tail(&entry->list, &keylist);

                if (JOBJECT == vtype) {
                    mlen = strlen("%s[%d].") + strlen(prefix) + arridxlen;
                    if (module_name) {
                        iter_pre = LITE_format_string(mlen, "%s[%d].", magic, module_name, prefix, count);
                    } else {
                        iter_pre = LITE_format_string(mlen, "%s[%d].", prefix, count);
                    }
                    if (NULL == iter_pre) {
                        return NULL;
                    }

                    _LITE_json_keys_of_ext(vtype, val, vlen, iter_pre, magic, module_name);
                    LITE_free(iter_pre);
                } else if (JARRAY == vtype) {
                    mlen = strlen("%s[%d]") + strlen(prefix) + arridxlen;
                    if (module_name) {
                        iter_pre = LITE_format_string(mlen, "%s[%d]", magic, module_name, prefix, count);
                    } else {
                        iter_pre = LITE_format_string(mlen, "%s[%d]", prefix, count);
                    }
                    if (NULL == iter_pre) {
                        return NULL;
                    }

                    _LITE_json_keys_of_ext(vtype, val, vlen, iter_pre, magic, module_name);
                    LITE_free(iter_pre);
                }
                ++count;
            }
        }
    }

    if (!strcmp("", prefix)) {
        json_key_t     *entry = NULL;

        entry = LITE_malloc(sizeof(json_key_t), magic, module_name);
        if (NULL == entry) {
            log_err("LITE_malloc failed!");
            return NULL;
        }
        memset(entry, 0, sizeof(json_key_t));
        list_add_tail(&entry->list, &keylist);

        return &keylist;
    }

    return NULL;

}

static char *_json_value_by_arrname(char *src, int src_len, const char *key, int key_len, int *val_len)
{
    char       *pos = src;
    char       *entry = NULL;
    const char *p = NULL;
    const char *arr_pre = key;
    const char *arr_suf = NULL;
    int         vtype;
    int         loop;
    int         loop_tmp = 0;
    do {
        loop = 0;

        arr_pre = strchr(arr_pre, '[');
        if (arr_pre && (arr_pre <  key + key_len)) {
            arr_suf = strchr(arr_pre, ']');
        }
        if (arr_pre && arr_suf && (arr_suf < key + key_len)) {
            loop_tmp = 0;
            for (p = arr_pre + 1; p < arr_suf; ++p) {
                if (*p > '9' || *p < '0') {
                    return NULL;
                }
                loop_tmp *= 10;
                loop_tmp += *p - '0';

            }

            pos = json_get_object(JARRAY, pos, src + src_len);
            if (pos != 0 && *pos != 0) {
                if (*pos == '[' && *(pos + 1) == ']') {
                    return NULL;
                }
            }

            json_array_for_each_entry(src, src_len, pos, entry, *val_len, vtype) {
                if (entry && *val_len) {
                    if (++loop >= loop_tmp) {
                        break;
                    }
                }
            }

            if (loop != loop_tmp) {
                return NULL;
            }
            src = entry;
            arr_pre = arr_suf + 1;
        }
    } while (arr_pre && arr_suf && (arr_pre < key + key_len));
    return entry;
}

char *LITE_json_value_of(char *key, char *src, ...)
{
    char       *value = NULL;
    char       *ret = NULL;
    char       *delim = NULL;
    char       *key_iter;
    char       *key_next;
    char       *src_iter;
    char       *module_name = NULL;

    int         key_len;
    int         value_len = -1;
    int         src_iter_len;
    int         magic = 0;

    if (NULL == key || NULL == src) {
        return NULL;
    }

#if WITH_MEM_STATS_PER_MODULE

    va_list     ap;
    va_start(ap, src);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    src_iter = src;
    src_iter_len = strlen(src_iter);
    key_iter = key;

    do {
        if ((delim = strchr(key_iter, '.')) != NULL) {
            key_len = delim - key_iter;
            key_next = key_iter;

            value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, key_len, &value_len, 0);
            if (value == NULL) {
                return NULL;
            }

            src_iter = value;
            src_iter_len = value_len;
            key_iter = delim + 1;
        }
    } while (delim);

    key_len = strlen(key_iter);
    key_next = key_iter;
    value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, key_len, &value_len, 0);
    if (NULL == value) {
        return NULL;
    }
    ret = LITE_malloc((value_len + 1) * sizeof(char), magic, module_name);
    if (NULL == ret) {
        return NULL;
    }
    LITE_snprintf(ret, value_len + 1, "%s", value);
    return ret;
}

void LITE_json_keys_release(list_head_t *keylist)
{
    json_key_t         *pos, *tmp;

    if (NULL == keylist) {
        return;
    }

    list_for_each_entry_safe(pos, tmp, keylist, list, json_key_t) {
        if (pos->key) {
            LITE_free(pos->key);
        }
        list_del(&pos->list);
        LITE_free(pos);
    }
}

static list_head_t *_LITE_json_keys_of(char *src, int src_len, char *prefix, ...)
{
    static          LIST_HEAD(keylist);
    char           *module_name = NULL;
    char           *iter_pre = NULL;
    char           *pos = 0, *key = 0, *val = 0;
    int             klen = 0, vlen = 0, vtype = 0;
    int             magic = 0;
    unsigned int    mlen = 0;

#if WITH_MEM_STATS_PER_MODULE

    va_list         ap;
    va_start(ap, prefix);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    if (!strcmp("", prefix)) {
        INIT_LIST_HEAD(&keylist);
    }

    json_object_for_each_kv(src, src_len, pos, key, klen, val, vlen, vtype) {
        if (key && klen && val && vlen) {

            json_key_t     *entry = NULL;

            entry = LITE_malloc(sizeof(json_key_t), magic, module_name);
            if (NULL == entry) {
                log_err("LITE_malloc failed!");
                return NULL;
            }
            memset(entry, 0, sizeof(json_key_t));

            mlen = strlen(prefix) + klen + 1;
            if (module_name) {
                entry->key = LITE_format_string(mlen, "%s%.*s", magic, module_name, prefix, klen, key);
            } else {
                entry->key = LITE_format_string(mlen, "%s%.*s", prefix, klen, key);
            }
            if (NULL == entry->key) {
                LITE_free(entry);
                return NULL;
            }

            list_add_tail(&entry->list, &keylist);

            if (JOBJECT == vtype) {
                mlen = strlen(prefix) + klen + 2;
                if (module_name) {
                    iter_pre = LITE_format_string(mlen, "%s%.*s.", magic, module_name, prefix, klen, key);
                } else {
                    iter_pre = LITE_format_string(mlen, "%s%.*s.", prefix, klen, key);
                }
                if (NULL == iter_pre) {
                    return NULL;
                }

                _LITE_json_keys_of(val, vlen, iter_pre, magic, module_name);
                LITE_free(iter_pre);
            }
        }
    }

    if (!strcmp("", prefix)) {
        json_key_t     *entry = NULL;

        entry = LITE_malloc(sizeof(json_key_t), magic, module_name);
        if (NULL == entry) {
            log_err("LITE_malloc failed!");
            return NULL;
        }
        memset(entry, 0, sizeof(json_key_t));
        list_add_tail(&entry->list, &keylist);

        return &keylist;
    }

    return NULL;
}



list_head_t *LITE_json_keys_of(char *src, char *prefix, ...)
{
    char *module_name = NULL;
    int magic = 0;

    if (!src || !prefix) {
        return NULL;
    }

#if WITH_MEM_STATS_PER_MODULE

    va_list ap;
    va_start(ap, prefix);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    return _LITE_json_keys_of(src, strlen(src), prefix, magic, module_name);

}

list_head_t *LITE_json_keys_of_ext(char *src, char *prefix, ...)
{
    char *module_name = NULL;
    int magic = 0;

#if WITH_MEM_STATS_PER_MODULE

    va_list ap;
    va_start(ap, prefix);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    if (!src || !prefix) {
        return NULL;
    }

    return _LITE_json_keys_of_ext(JOBJECT, src, strlen(src), prefix, magic, module_name);
}


static char *_LITE_json_value_of_ext(char *key, char *src, int src_len, int *val_len)
{
    char       *value = NULL;
    char       *delim = NULL;
    char       *key_iter;
    char       *key_next;
    char       *src_iter;
    const char *arr_pre = NULL;

    int         value_len;
    int         key_len;
    int         obj_key_len = 0;
    int         key_type;
    int         src_iter_len;

    src_iter = src;
    src_iter_len = src_len;
    key_iter = key;
    value_len = src_iter_len;
    do {
        if ((delim = strchr(key_iter, '.')) != NULL) {
            key_len = delim - key_iter;
            key_type = JOBJECT;
            key_next = key_iter;
            if (0 == contain_arr(key_next, key_len, &arr_pre)) {
                key_type = JARRAY;
                obj_key_len = arr_pre - key_iter;
                if (obj_key_len) {
                    value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, obj_key_len, &value_len, 0);
                } else {
                    value = src_iter;
                }
            } else {
                value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, key_len, &value_len, 0);
            }

            if (NULL == value) {
                return NULL;
            }

            if (key_type == JARRAY) {
                if (NULL == (value = _json_value_by_arrname(value, value_len, arr_pre, key_len - obj_key_len, &value_len))) {
                    return NULL;
                }
            }
            src_iter = value;
            src_iter_len = value_len;
            key_iter = delim + 1;

        }
    } while (delim);

    key_len = strlen(key_iter);
    key_next = key_iter;
    key_type = JOBJECT;
    if (0 == contain_arr(key_next, key_len, &arr_pre)) {
        key_type = JARRAY;
        obj_key_len = arr_pre - key_iter;
        if (obj_key_len) {
            value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, obj_key_len, &value_len, 0);
        } else {
            value = src_iter;
        }
    } else {
        value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, key_len, &value_len, 0);
    }

    if (NULL == value) {
        return NULL;
    }
    if (key_type == JARRAY) {
        if (NULL == (value = _json_value_by_arrname(value, value_len, arr_pre, key_len - obj_key_len, &value_len))) {
            return NULL;
        }
    }
    *val_len = value_len;
    return value;
}

char *LITE_json_value_of_ext(char *key, char *src, ...)
{
    char       *ret = NULL;
    char       *value = NULL;
    char       *module_name = NULL;
    int         value_len = 0;
    int         magic = 0;

    if (NULL == key || NULL == src) {
        return NULL;
    }

#if WITH_MEM_STATS_PER_MODULE

    va_list     ap;
    va_start(ap, src);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    value = _LITE_json_value_of_ext(key, src, strlen(src), &value_len);
    if (!value) {
        return NULL;
    }

    ret = LITE_malloc((value_len + 1) * sizeof(char), magic, module_name);
    if (NULL == ret) {
        return NULL;
    }
    LITE_snprintf(ret, value_len + 1, "%s", value);

    return ret;
}

char *LITE_json_value_of_ext2(char *key, char *src, int src_len, int *value_len)
{
    if (NULL == key || NULL == src || NULL == value_len || src_len <= 0) {
        return NULL;
    }

    return _LITE_json_value_of_ext(key, src, src_len, value_len);
}

int get_json_item_size(char *src, int src_len)
{
    char       *iter_pos = NULL;
    int         iter_size = 0;
    int         iter_obj_deep = 0;
    int         iter_arr_deep = 0;
    int         iter_str_deep = 0;
    if (!src || (src_len <= 0)) {
        return 0;
    }

    if ((('[' == *src) && (']' == *(src + 1))) \
        || (('{' == *src) && ('}' == *(src + 1)))) {
        return 0;
    }

    iter_pos = src + 1;

    while (iter_pos && (iter_pos < src + src_len)) {
        if (*iter_pos == '[') {
            iter_arr_deep++;
        } else if (*iter_pos == ']') {
            iter_arr_deep--;
        } else if (*iter_pos == '{') {
            iter_obj_deep++;
        } else if (*iter_pos == '}') {
            iter_obj_deep--;
        } else if (*iter_pos == '\"') {
            ++iter_str_deep;
            iter_str_deep %= 2;
        }
        if (!iter_arr_deep && !iter_obj_deep && !iter_str_deep && *iter_pos == ',') {
            iter_size ++;
        }
        ++iter_pos;
    }

    return iter_size + 1;
}
