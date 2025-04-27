#include <stdio.h>
#include "easy_json.h"

int main() {
    /* 创建对象并链式设置值 */
    EasyJSON *obj = ej_create_object()
        ->set_string("name", "Alice")
        ->set_number("age", 25)
        ->set_bool("active", 1);

    /* 创建嵌套数组 */
    EasyJSON *scores = ej_create_array()
        ->append_number(90)
        ->append_number(85)
        ->append_null();
    obj->set("scores", scores);

    /* 使用 JSON 指针访问和修改 */
    EasyJSON *name = ej_pointer(obj, "/name");
    printf("Name: %s\n", ej_get_string(name, "Unknown")); // 输出: Alice
    ej_free(name);

    /* 序列化 */
    char *json_str = ej_to_string(obj, 1);
    printf("JSON: %s\n", json_str);
    ej_free_string(json_str); // 使用 ej_free_string 代替 cJSON_free

    /* 生成和应用补丁 */
    EasyJSON *obj2 = ej_parse("{\"name\":\"Bob\",\"age\":30}");
    EasyJSON *patch = ej_generate_patch(obj, obj2);
    json_str = ej_to_string(patch, 1);
    printf("Patch: %s\n", json_str);
    ej_free_string(json_str); // 使用 ej_free_string

    ej_apply_patch(obj, patch);
    json_str = ej_to_string(obj, 1);
    printf("Patched JSON: %s\n", json_str);
    ej_free_string(json_str); // 使用 ej_free_string

    /* 清理 */
    ej_free(patch);
    ej_free(obj2);
    ej_free(obj);
    return 0;
}