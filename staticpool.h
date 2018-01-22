/*
 *    staticpool version 1.0.0
 *
 *    ndsunwei@163.com
 *
 *    staticpool is free software: you can redistribute it and/or modify.
 *
 *    StaticPool is a Container pool.
 *    Container is a Hash Bucket(ANSI C hash table for strings), size limit uint16_t.
 *
 */

#ifndef _MK_STATICPOOL
#define _MK_STATICPOOL

#include <stdint.h>

typedef struct Pair Pair;
typedef struct Bucket Bucket;
typedef struct Container Container;
typedef struct Pool Pool;

struct Pair
{
	char *key;
	char *value;
	Pair *next;
};

struct Bucket
{
	uint16_t count;
	Pair *pairs;
};

struct Container
{
	uint16_t count;
	char *name;
	Bucket *buckets;
};

struct Pool
{
	uint16_t count;
	Container *containers;
};

static Pool *staticpool = NULL;

int StaticPoolInit(uint16_t count);
int StaticPoolFree();
int StaticPoolAdd(const char *name, uint16_t count);
int StaitcPoolDel(const char *name);
int StaticPoolCount();
int StaticPoolSetVal(const char *name, const char *key, const char *value);
const char * StaticPoolGetVal(const char *name, const char *key);
int StaticPoolClearVal(const char *name, const char *key);
int StaticPoolConSize(const char *name);

#endif