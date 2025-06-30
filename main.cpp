#include <cstdio>

#include "heightmap.hpp"

void foo(eko::heightmap<float>&& my_hm) {}

int main() {
	printf("Hello Water! (%d)\n", __cplusplus);
	
	{
		eko::heightmap<float> my_hm(20, 20);
		
		my_hm.get(1, 1) = 3;
		foo(std::move(my_hm));
	}
	
	printf("Done...\n");
	return 0;
}