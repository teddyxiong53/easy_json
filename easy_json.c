#include "easy_json.h"
#include <stdlib.h>
#include <string.h>

static void ensure_valid(EasyJSON *ej) {
    if (!ej) return; /* 由调用者处理空指针 */
    if (!ej->node) {
        ej->node = cJSON_CreateObject(); /* 默认创建对象 */
        ej->owns_memory = 1;
    }
}

static EasyJSON *wrap_cjson(cJSON *node, int owns_memory) {
    if (!node) return NULL;
    EasyJSON *ej = (EasyJSON *)malloc(sizeof(EasyJSON));
    if (!ej) {
        cJSON_Delete(node);
        return NULL;
    }
    ej->node = node;
    ej->owns_memory = owns_memory;
    return ej;
}

/* 创建函数 */
EasyJSON *ej_create_object(void) {
    return wrap_cjson(cJSON_CreateObject(), 1);
}

EasyJSON *ej_create_array(void) {
    return wrap_cjson(cJSON_CreateArray(), 1);
}

EasyJSON *ej_create_null(void) {
    return wrap_cjson(cJSON_CreateNull(), 1);
}

EasyJSON *ej_create_bool(int value) {
    return wrap_cjson(cJSON_CreateBool(value), 1);
}

EasyJSON *ej_create_number(double value) {
    return wrap_cjson(cJSON_CreateNumber(value), 1);
}

EasyJSON *ej_create_string(const char *value) {
    return wrap_cjson(cJSON_CreateString(value), 1);
}

EasyJSON *ej_parse(const char *json_str) {
    cJSON *node = cJSON_Parse(json_str);
    return wrap_cjson(node, 1);
}

/* 释放函数 */
void ej_free(EasyJSON *ej) {
    if (ej && ej->owns_memory && ej->node) {
        cJSON_Delete(ej->node);
    }
    if (ej) {
        free(ej);
    }
}

void ej_free_string(char *str) {
    if (str) {
        cJSON_free(str);
    }
}

/* 类型检查 */
EJType ej_type(const EasyJSON *ej) {
    if (!ej || !ej->node) return EJ_INVALID;
    int type = ej->node->type & 0xFF;
    switch (type) {
        case cJSON_NULL: return EJ_NULL;
        case cJSON_True: case cJSON_False: return EJ_BOOL;
        case cJSON_Number: return EJ_NUMBER;
        case cJSON_String: return EJ_STRING;
        case cJSON_Array: return EJ_ARRAY;
        case cJSON_Object: return EJ_OBJECT;
        default: return EJ_INVALID;
    }
}

int ej_is_null(const EasyJSON *ej) { return ej_type(ej) == EJ_NULL; }
int ej_is_bool(const EasyJSON *ej) { return ej_type(ej) == EJ_BOOL; }
int ej_is_number(const EasyJSON *ej) { return ej_type(ej) == EJ_NUMBER; }
int ej_is_string(const EasyJSON *ej) { return ej_type(ej) == EJ_STRING; }
int ej_is_array(const EasyJSON *ej) { return ej_type(ej) == EJ_ARRAY; }
int ej_is_object(const EasyJSON *ej) { return ej_type(ej) == EJ_OBJECT; }

/* 获取值 */
int ej_get_bool(const EasyJSON *ej, int default_value) {
    if (ej_is_bool(ej)) return cJSON_IsTrue(ej->node) ? 1 : 0;
    return default_value;
}

double ej_get_number(const EasyJSON *ej, double default_value) {
    if (ej_is_number(ej)) return ej->node->valuedouble;
    return default_value;
}

const char *ej_get_string(const EasyJSON *ej, const char *default_value) {
    if (ej_is_string(ej)) return ej->node->valuestring;
    return default_value;
}

EasyJSON *ej_get(const EasyJSON *ej, const char *key) {
    if (!ej_is_object(ej)) return NULL;
    cJSON *node = cJSON_GetObjectItem(ej->node, key);
    return wrap_cjson(node, 0); /* 不拥有内存 */
}

EasyJSON *ej_get_index(const EasyJSON *ej, int index) {
    if (!ej_is_array(ej) || index < 0) return NULL;
    cJSON *node = cJSON_GetArrayItem(ej->node, index);
    return wrap_cjson(node, 0); /* 不拥有内存 */
}

EasyJSON *ej_pointer(const EasyJSON *ej, const char *pointer) {
    if (!ej || !ej->node) return NULL;
    cJSON *node = cJSONUtils_GetPointer(ej->node, pointer);
    return wrap_cjson(node, 0); /* 不拥有内存 */
}

/* 设置值 */
void ej_set(EasyJSON *ej, const char *key, EasyJSON *value) {
    if (!ej) return;
    ensure_valid(ej);
    if (!ej_is_object(ej)) {
        if (ej->owns_memory) cJSON_Delete(ej->node);
        ej->node = cJSON_CreateObject();
        ej->owns_memory = 1;
    }
    if (!value || !value->node) return; /* 无效值，跳过 */
    cJSON *old = cJSON_DetachItemFromObject(ej->node, key);
    if (old) cJSON_Delete(old);
    cJSON_AddItemToObject(ej->node, key, value->node);
    value->owns_memory = 0; /* 转移所有权 */
    ej_free(value);
}

void ej_set_string(EasyJSON *ej, const char *key, const char *value) {
    if (!ej || !value) return;
    EasyJSON *val = ej_create_string(value);
    if (!val) return;
    ej_set(ej, key, val);
}

void ej_set_number(EasyJSON *ej, const char *key, double value) {
    if (!ej) return;
    EasyJSON *val = ej_create_number(value);
    if (!val) return;
    ej_set(ej, key, val);
}

void ej_set_bool(EasyJSON *ej, const char *key, int value) {
    if (!ej) return;
    EasyJSON *val = ej_create_bool(value);
    if (!val) return;
    ej_set(ej, key, val);
}

void ej_set_null(EasyJSON *ej, const char *key) {
    if (!ej) return;
    EasyJSON *val = ej_create_null();
    if (!val) return;
    ej_set(ej, key, val);
}

/* 数组操作 */
void ej_append(EasyJSON *ej, EasyJSON *value) {
    if (!ej) return;
    ensure_valid(ej);
    if (!ej_is_array(ej)) {
        if (ej->owns_memory) cJSON_Delete(ej->node);
        ej->node = cJSON_CreateArray();
        ej->owns_memory = 1;
    }
    if (!value || !value->node) return; /* 无效值，跳过 */
    cJSON_AddItemToArray(ej->node, value->node);
    value->owns_memory = 0; /* 转移所有权 */
    ej_free(value);
}

void ej_append_string(EasyJSON *ej, const char *value) {
    if (!ej || !value) return;
    EasyJSON *val = ej_create_string(value);
    if (!val) return;
    ej_append(ej, val);
}

void ej_append_number(EasyJSON *ej, double value) {
    if (!ej) return;
    EasyJSON *val = ej_create_number(value);
    if (!val) return;
    ej_append(ej, val);
}

void ej_append_bool(EasyJSON *ej, int value) {
    if (!ej) return;
    EasyJSON *val = ej_create_bool(value);
    if (!val) return;
    ej_append(ej, val);
}

void ej_append_null(EasyJSON *ej) {
    if (!ej) return;
    EasyJSON *val = ej_create_null();
    if (!val) return;
    ej_append(ej, val);
}

/* 删除 */
void ej_remove(EasyJSON *ej, const char *key) {
    if (ej && ej_is_object(ej)) {
        cJSON *old = cJSON_DetachItemFromObject(ej->node, key);
        if (old) cJSON_Delete(old);
    }
}

void ej_remove_index(EasyJSON *ej, int index) {
    if (ej && ej_is_array(ej) && index >= 0) {
        cJSON *old = cJSON_DetachItemFromArray(ej->node, index);
        if (old) cJSON_Delete(old);
    }
}

/* 序列化 */
char *ej_to_string(const EasyJSON *ej, int formatted) {
    if (!ej || !ej->node) return NULL;
    return formatted ? cJSON_Print(ej->node) : cJSON_PrintUnformatted(ej->node);
}

/* 补丁操作 */
EasyJSON *ej_generate_patch(EasyJSON *from, EasyJSON *to) {
    if (!from || !to || !from->node || !to->node) return NULL;
    cJSON *patch = cJSONUtils_GeneratePatches(from->node, to->node);
    return wrap_cjson(patch, 1);
}

int ej_apply_patch(EasyJSON *ej, EasyJSON *patch) {
    if (!ej || !patch || !ej->node || !patch->node) return -1;
    return cJSONUtils_ApplyPatches(ej->node, patch->node);
}

/* 内存所有权控制 */
void ej_take_ownership(EasyJSON *ej) {
    if (ej) ej->owns_memory = 1;
}

void ej_release_ownership(EasyJSON *ej) {
    if (ej) ej->owns_memory = 0;
}