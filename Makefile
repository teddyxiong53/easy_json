CC = gcc
CFLAGS = -Wall -g -fPIC
AR = ar
ARFLAGS = rcs

LIB_NAME = libeasy_json.a
LIB_SRCS = easy_json.c cJSON.c cJSON_Utils.c
LIB_OBJS = $(LIB_SRCS:.c=.o)

INCLUDE_DIR = /usr/local/include/easy_json
LIB_DIR = /usr/local/lib

.PHONY: all clean install

all: $(LIB_NAME) example

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_NAME): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

example: example.c $(LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $< -L. -leasy_json

install:
	@mkdir -p $(INCLUDE_DIR)
	@mkdir -p $(LIB_DIR)
	cp *.h $(INCLUDE_DIR)/
	cp $(LIB_NAME) $(LIB_DIR)/
	ldconfig

clean:
	rm -f $(LIB_OBJS) $(LIB_NAME) example
	
