.PHONY: all
all:
	gcc -o example example.c easy_json.c cJSON.c cJSON_Utils.c
.PHONY: clean
clean:
	rm -rf example
	