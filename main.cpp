#include <cstdio>

#include "heightmap.hpp"

int main() {
	printf("Hello Water! (%d)\n", __cplusplus);
	
	{
		eko::heightmap<uint8_t> my_hm(20, 20);
		
		my_hm.fill(0);
		my_hm.get(1, 1) = 3;
		
		my_hm.save("out.ekhm");
		
		std::ifstream fin("out.ekhm");
		eko::heightmap<uint8_t> my_read_hm(fin);
		fin.close();
	}
	
	printf("Done...\n");
	return 0;
}