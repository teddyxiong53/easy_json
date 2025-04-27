#ifndef EASY_JSON_H
#define EASY_JSON_H

#include "cJSON.h"
#include "cJSON_Utils.h"

/* 链式调用宏定义 */
#define EJ_CHAIN_CALL(obj) ((obj) ? (obj) : ej_create_object())
#define EJ_CHAIN_ARRAY(obj) ((obj) ? (obj) : ej_create_array())

#ifdef __cplusplus
extern "C" {
#endif

/* JSON 类型枚举 */
typedef enum {
    EJ_INVALID,
    EJ_NULL,
    EJ_BOOL,
    EJ_NUMBER,
    EJ_STRING,
    EJ_ARRAY,
    EJ_OBJECT
} EJType;

/* EasyJSON 结构 */
typedef struct EasyJSON {
    cJSON *node;            /* 底层 cJSON 节点 */
    int owns_memory;        /* 是否拥有内存所有权 */
    /* 链式调用函数指针 */
    struct EasyJSON *(*set)(const char *key, struct EasyJSON *value);
    struct EasyJSON *(*set_string)(const char *key, const char *value);
    struct EasyJSON *(*set_number)(const char *key, double value);
    struct EasyJSON *(*set_bool)(const char *key, int value);
    struct EasyJSON *(*set_null)(const char *key);
    struct EasyJSON *(*append)(struct EasyJSON *value);
    struct EasyJSON *(*append_string)(const char *value);
    struct EasyJSON *(*append_number)(double value);
    struct EasyJSON *(*append_bool)(int value);
    struct EasyJSON *(*append_null)(void);
} EasyJSON;

/* 创建函数 */
EasyJSON *ej_create_object(void);
EasyJSON *ej_create_array(void);
EasyJSON *ej_create_null(void);
EasyJSON *ej_create_bool(int value);
EasyJSON *ej_create_number(double value);
EasyJSON *ej_create_string(const char *value);
EasyJSON *ej_parse(const char *json_str);

/* 释放函数 */
void ej_free(EasyJSON *ej);
void ej_free_string(char *str); /* 释放 ej_to_string 返回的字符串 */

/* 类型检查 */
EJType ej_type(const EasyJSON *ej);
int ej_is_null(const EasyJSON *ej);
int ej_is_bool(const EasyJSON *ej);
int ej_is_number(const EasyJSON *ej);
int ej_is_string(const EasyJSON *ej);
int ej_is_array(const EasyJSON *ej);
int ej_is_object(const EasyJSON *ej);

/* 获取值 */
int ej_get_bool(const EasyJSON *ej, int default_value);
double ej_get_number(const EasyJSON *ej, double default_value);
const char *ej_get_string(const EasyJSON *ej, const char *default_value);
EasyJSON *ej_get(const EasyJSON *ej, const char *key); /* 对象键 */
EasyJSON *ej_get_index(const EasyJSON *ej, int index); /* 数组索引 */
EasyJSON *ej_pointer(const EasyJSON *ej, const char *pointer); /* JSON 指针 */

/* 设置值 */
EasyJSON *ej_set(EasyJSON *ej, const char *key, EasyJSON *value); /* 对象 */
EasyJSON *ej_set_string(EasyJSON *ej, const char *key, const char *value);
EasyJSON *ej_set_number(EasyJSON *ej, const char *key, double value);
EasyJSON *ej_set_bool(EasyJSON *ej, const char *key, int value);
EasyJSON *ej_set_null(EasyJSON *ej, const char *key);

/* 数组操作 */
EasyJSON *ej_append(EasyJSON *ej, EasyJSON *value); /* 添加到数组 */
EasyJSON *ej_append_string(EasyJSON *ej, const char *value);
EasyJSON *ej_append_number(EasyJSON *ej, double value);
EasyJSON *ej_append_bool(EasyJSON *ej, int value);
EasyJSON *ej_append_null(EasyJSON *ej);

/* 删除 */
EasyJSON *ej_remove(EasyJSON *ej, const char *key); /* 从对象删除 */
EasyJSON *ej_remove_index(EasyJSON *ej, int index); /* 从数组删除 */

/* 序列化 */
char *ej_to_string(const EasyJSON *ej, int formatted); /* 返回字符串，需用 ej_free_string 释放 */

/* 补丁操作 */
EasyJSON *ej_generate_patch(EasyJSON *from, EasyJSON *to);
int ej_apply_patch(EasyJSON *ej, EasyJSON *patch);

/* 内存所有权控制 */
void ej_take_ownership(EasyJSON *ej);
void ej_release_ownership(EasyJSON *ej);

#ifdef __cplusplus
}
#endif

#endif /* EASY_JSON_H */