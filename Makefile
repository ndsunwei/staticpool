
CC ?= gcc
CFLAGS = -g -O2 -Wall -I$(LUA_INC) -I$(STATICPOOL_INC) $(MYCFLAGS)
SHARED := -fPIC --shared

LUA_INC ?= /home/ad/skynet/3rd/lua
STATICPOOL_INC ?= /home/ad/skynet/3rd/staticpool

TARGET = staticpool.so

all: $(TARGET)

$(TARGET): staticpool.c luastaticpool.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ -Wno-unused-variable

test:
	$(CC) -g staticpool.c test.c -I$(STATICPOOL_INC) -o staticpool && ./staticpool

clean:
	rm -f $(TARGET)

.PHONY: all clean test