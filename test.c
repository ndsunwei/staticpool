#include <stdio.h>
#include "staticpool.h"

int main(int argc, char const *argv[])
{
	int ret;

	ret = StaticPoolInit(20);
	printf("pool init %d\n", ret);
	
	ret = StaticPoolAdd("map1", 10);
	printf("map1 %d\n", ret);
	ret = StaticPoolAdd("map2", 10);
	printf("map2 %d\n", ret);

	ret = StaticPoolSetVal("map1", "map1_test1", "11111");
	printf("map1 put %d\n", ret);
	ret = StaticPoolSetVal("map1", "map1_test2", "bbbbb");
	printf("map1 put %d\n", ret);

	ret = StaticPoolSetVal("map2", "map2_test1", "aaaaabbbbb");
	printf("map2 put %d\n", ret);
	ret = StaticPoolSetVal("map2", "map2_test2", "aaaaa33333");
	printf("map2 put %d\n", ret);

	printf("%s\n", StaticPoolGetVal("map1", "map1_test1"));
	printf("%s\n", StaticPoolGetVal("map1", "map1_test2"));
	printf("%s\n", StaticPoolGetVal("map2", "map2_test1"));
	printf("%s\n", StaticPoolGetVal("map2", "map2_test2"));

	ret = StaticPoolClearVal("map1", "map1_test1");
	printf("map1 clear %d\n", ret);
	ret = StaticPoolClearVal("map1", "map1_test2");
	printf("map1 clear %d\n", ret);

	ret = StaticPoolClearVal("map2", "map2_test1");
	printf("map2 clear %d\n", ret);
	ret = StaticPoolClearVal("map2", "map2_test2");
	printf("map2 clear %d\n", ret);

	ret = StaticPoolFree();
	printf("pool free %d\n", ret);
}