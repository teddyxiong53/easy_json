# EasyJSON

EasyJSON 是一个基于 cJSON 的简单易用的 C 语言 JSON 解析和生成库。它提供了更友好的 API 接口，让 JSON 数据操作变得更加简单直观。

## 特点

- 简单易用的 API 接口
- 基于成熟的 cJSON 库
- 自动内存管理
- 支持 JSON 指针（RFC6901）

## 使用示例

```c
// 创建 JSON 对象
EasyJSON *json = ej_create_object();
ej_set_string(json, "name", "John");
ej_set_number(json, "age", 30);
ej_set_bool(json, "is_student", 0);

// 创建并添加数组
EasyJSON *array = ej_create_array();
ej_append_string(array, "apple");
ej_append_string(array, "banana");
ej_append_string(array, "orange");

// 添加数组到对象
ej_set(json, "fruits", array);

// 转换为字符串
char *str = ej_to_string(json, 1);  // 1 表示格式化输出
printf("%s\n", str);

// 释放内存
ej_free_string(str);
ej_free(json);
```

## 编译

```bash
make
```

## 安装

```bash
sudo make install
```

## 许可证

本项目基于 MIT 许可证开源。