#include <stdio.h>
#include "easy_json.h"

int main() {
    /* 创建对象并设置值 */
    EasyJSON *obj = ej_create_object();
    ej_set_string(obj, "name", "Alice");
    ej_set_number(obj, "age", 25);
    ej_set_bool(obj, "active", 1);

    /* 创建嵌套数组 */
    EasyJSON *scores = ej_create_array();
    ej_append_number(scores, 90);
    ej_append_number(scores, 85);
    ej_append_null(scores);

    /* 添加数组到对象 */
    ej_set(obj, "scores", scores);

    /* 使用 JSON 指针访问和修改 */
    EasyJSON *name = ej_pointer(obj, "/name");
    printf("Name: %s\n", ej_get_string(name, "Unknown")); // 输出: Alice
    ej_free(name);

    /* 序列化 */
    char *json_str = ej_to_string(obj, 1);
    printf("JSON: %s\n", json_str);
    ej_free_string(json_str);

    /* 生成和应用补丁 */
    EasyJSON *obj2 = ej_parse("{\"name\":\"Bob\",\"age\":30}");
    EasyJSON *patch = ej_generate_patch(obj, obj2);
    json_str = ej_to_string(patch, 1);
    printf("Patch: %s\n", json_str);
    ej_free_string(json_str);

    ej_apply_patch(obj, patch);
    json_str = ej_to_string(obj, 1);
    printf("Patched JSON: %s\n", json_str);
    ej_free_string(json_str);

    /* 清理 */
    ej_free(patch);
    ej_free(obj2);
    ej_free(obj);
    return 0;
}