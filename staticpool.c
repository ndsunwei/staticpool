#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "staticpool.h"

static Container * StaticPoolGetContainer(const char *name);
static Pair * StaticPoolGetPair(Bucket *bucket, const char *key);
static int StaticPoolClearBucket(Bucket *bucket);

static unsigned long hash(const char *str)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

int StaticPoolInit(uint16_t count)
{
	if (staticpool != NULL)
	{
		return 0;
	}
	staticpool = malloc(sizeof(*staticpool));
	if (staticpool == NULL)
	{
		return 0;
	}

	staticpool->count = count;
	staticpool->containers = malloc(staticpool->count * sizeof(*staticpool->containers));
	if (staticpool->containers == NULL)
	{
		staticpool->count = 0;
		free(staticpool);
		staticpool = NULL;
		return 0;
	}

	memset(staticpool->containers, 0, staticpool->count * sizeof(*staticpool->containers));
	
	return 1;
}

int StaticPoolFree()
{
	unsigned int n, i;
	Container *container;

	if (staticpool == NULL)
	{
		return 1;
	}
	n = staticpool->count;
	if (n > 0)
	{
		for (i = 0; i < n; ++i)
		{
			container = &(staticpool->containers[i]);
			if (container->name != NULL)
			{
				StaitcPoolDel(container->name);
			}
		}
	}

	staticpool->count = 0;
	free(staticpool->containers);
	staticpool->containers = NULL;
	free(staticpool);
	staticpool = NULL;

	return 1;
}

static Container * StaticPoolGetContainer(const char *name)
{
	unsigned int index, n, i, j;
	Container *container;

	if (name == NULL)
	{
		return NULL;
	}
	if (staticpool == NULL)
	{
		return NULL;
	}
	n = staticpool->count;
	if (n == 0)
	{
		return NULL;
	}

	index = hash(name) % staticpool->count;
	i = index;
	
	for (j = 0; j < n; ++j)
	{
		container = &(staticpool->containers[i]);
		if (container != NULL && container->name != NULL && strcmp(container->name, name) == 0)
		{
			return container;
		}
		i++;
		if (i == n)
		{
			i = 0;
		}
	}

	return NULL;
}

int StaticPoolAdd(const char *name, uint16_t count)
{
	unsigned int name_len, index, n, i, j;
	char *new_name;
	Container *container;

	if (name == NULL)
	{
		return 0;
	}
	if (staticpool == NULL)
	{
		return 0;
	}
	n = staticpool->count;

	container = StaticPoolGetContainer(name);
	if (container != NULL)
	{
		return 0;
	}
	
	index = hash(name) % staticpool->count;
	i = index;

	for (j = 0; j < n; ++j)
	{
		container = &(staticpool->containers[i]);
		if (container->name == NULL)
		{
			name_len = strlen(name);
			new_name = malloc((name_len + 1) * sizeof(char));
			if (new_name == NULL)
			{
				return 0;
			}

			container->count = count;
			container->buckets = malloc(container->count * sizeof(*container->buckets));
			if (container->buckets == NULL)
			{
				container->count = 0;
				free(new_name);
				new_name = NULL;
				return 0;
			}

			container->name = new_name;
			strcpy(container->name, name);
			memset(container->buckets, 0, container->count * sizeof(*container->buckets));

			return 1;
		}
		i++;
		if (i == n)
		{
			i = 0;
		}
	}

	return 0;
}

int StaitcPoolDel(const char *name)
{
	unsigned int n, i;
	Container *container;
	Bucket *bucket;

	if (staticpool == NULL)
	{
		return 1;
	}
	n = staticpool->count;
	if (n == 0)
	{
		return 1;
	}
	container = StaticPoolGetContainer(name);
	if (container == NULL)
	{
		return 1;
	}
	n = container->count;

	if (n > 0)
	{
		for (i = 0; i < n; ++i)
		{
			bucket = &(container->buckets[i]);
			StaticPoolClearBucket(bucket);
		}
	}

	container->count = 0;
	free(container->name);
	container->name = NULL;
	free(container->buckets);
	container->buckets = NULL;

	return 1;
}

int StaticPoolCount()
{
	unsigned int n, i, count;
	Container *container;
	
	if (staticpool == NULL)
	{
		return 0;
	}
	n = staticpool->count;
	if (n == 0)
	{
		return 0;
	}

	count = 0;
	for (i = 0; i < n; ++i)
	{
		container = &(staticpool->containers[i]);
		if (container->name != NULL)
		{
			count++;
		}
	}

	return count;
}

static int StaticPoolClearBucket(Bucket *bucket)
{
	Pair *pair, **ppair;
	ppair = &bucket->pairs;
	while (*ppair)
	{
		pair = *ppair;
		*ppair = pair->next;
		pair->next = NULL;
		free(pair->key);
		pair->key = NULL;
		free(pair->value);
		pair->value = NULL;
		free(pair);
		pair = NULL;
	}
	bucket->count = 0;
	return 1;
}

static Pair * StaticPoolGetPair(Bucket *bucket, const char *key)
{
	unsigned int n;
	Pair *pair;

	n = bucket->count;
	if (n == 0) {
		return NULL;
	}
	
	pair = bucket->pairs;
	while (pair != NULL)
	{
		if (pair->key != NULL && pair->value != NULL) {
			if (strcmp(pair->key, key) == 0) {
				return pair;
			}
		}
		pair = pair->next;
	}
	return NULL;
}

int StaticPoolSetVal(const char *name, const char *key, const char *value)
{
	unsigned int key_len, value_len, index, n;
	char *new_key, *new_value;
	char *tmp_value;
	Container *container;
	Bucket *bucket;
	Pair *pair, *tmp_pairs;

	if (name == NULL)
	{
		return 0;
	}
	if (key == NULL || value == NULL) {
		return 0;
	}
	if (staticpool == NULL)
	{
		return 0;
	}
	n = staticpool->count;
	if (n == 0)
	{
		return 0;
	}
	container = StaticPoolGetContainer(name);
	if (container == NULL)
	{
		return 0;
	}
	n = container->count;
	if (n == 0)
	{
		return 0;
	}

	key_len = strlen(key);
	value_len = strlen(value);
	index = hash(key) % container->count;
	bucket = &(container->buckets[index]);

	pair = StaticPoolGetPair(bucket, key);
	if (pair != NULL)
	{
		if (strlen(pair->value) < value_len) {
			tmp_value = realloc(pair->value, (value_len + 1) * sizeof(char));
			if (tmp_value == NULL) {
				return 0;
			}
			pair->value = tmp_value;
		}
		strcpy(pair->value, value);
		return 1;
	}

	new_key = malloc((key_len + 1) * sizeof(char));
	if (new_key == NULL) {
		return 0;
	}
	new_value = malloc((value_len + 1) * sizeof(char));
	if (new_value == NULL) {
		free(new_key);
		new_key = NULL;
		return 0;
	}

	tmp_pairs = malloc(sizeof(*tmp_pairs));
	if (tmp_pairs == NULL)
	{
		free(new_key);
		new_key = NULL;
		free(new_value);
		new_value = NULL;
		return 0;
	}
	tmp_pairs->key = new_key;
	tmp_pairs->value = new_value;
	tmp_pairs->next = NULL;
	strcpy(tmp_pairs->key, key);
	strcpy(tmp_pairs->value, value);

	pair = bucket->pairs;
	if (pair == NULL)
	{
		bucket->pairs = tmp_pairs;
		bucket->count = 1;
	}
	else
	{
		while (pair->next != NULL)
		{
			pair = pair->next;
		}
		pair->next = tmp_pairs;
		bucket->count++;
	}

	return 1;
}

const char * StaticPoolGetVal(const char *name, const char *key)
{
	unsigned int index, n;
	Container *container;
	Bucket *bucket;
	Pair *pair;

	if (name == NULL)
	{
		return NULL;
	}
	if (key == NULL) {
		return NULL;
	}
	if (staticpool == NULL)
	{
		return NULL;
	}
	n = staticpool->count;
	if (n == 0)
	{
		return NULL;
	}
	container = StaticPoolGetContainer(name);
	if (container == NULL)
	{
		return NULL;
	}
	n = container->count;
	if (n == 0)
	{
		return NULL;
	}

	index = hash(key) % container->count;
	bucket = &(container->buckets[index]);
	pair = StaticPoolGetPair(bucket, key);

	if (pair == NULL) {
		return NULL;
	}

	return pair->value;
}

int StaticPoolClearVal(const char *name, const char *key)
{
	unsigned int index, n;
	Container *container;
	Bucket *bucket;
	Pair *pair, **ppair;

	if (name == NULL)
	{
		return 1;
	}
	if (key == NULL) {
		return 1;
	}
	if (staticpool == NULL)
	{
		return 1;
	}
	n = staticpool->count;
	if (n == 0)
	{
		return 1;
	}
	container = StaticPoolGetContainer(name);
	if (container == NULL)
	{
		return 1;
	}
	n = container->count;
	if (n == 0)
	{
		return 1;
	}

	index = hash(key) % container->count;
	bucket = &(container->buckets[index]);

	ppair = &bucket->pairs;
	while (*ppair)
	{
		if ((*ppair)->key != NULL && (*ppair)->value != NULL && strcmp((*ppair)->key, key) == 0)
		{
			pair = *ppair;
			*ppair = pair->next;
			pair->next = NULL;
			free(pair->key);
			pair->key = NULL;
			free(pair->value);
			pair->value = NULL;
			free(pair);
			pair = NULL;
			bucket->count--;
			break;
		}
		else
		{
			ppair = &(*ppair)->next;
		}
	}

	return 1;
}

int StaticPoolConSize(const char *name)
{
	unsigned int n, i, count;
	Container *container;
	Bucket *bucket;

	if (name == NULL)
	{
		return 0;
	}
	if (staticpool == NULL)
	{
		return 0;
	}
	n = staticpool->count;
	if (n == 0)
	{
		return 0;
	}
	container = StaticPoolGetContainer(name);
	if (container == NULL)
	{
		return 0;
	}
	n = container->count;
	if (n == 0)
	{
		return 0;
	}

	count = 0;
	for (i = 0; i < n; ++i)
	{
		bucket = &(container->buckets[i]);
		count = count+bucket->count;
	}

	return count;
}