#include <cmath>
#include <cstdio>

#include "heightmap.hpp"

int main() {
	printf("Hello Water! (%d)\n", __cplusplus);
	
	{
		eko::heightmap<float> my_hm(60, 60);
		
		my_hm.fill(0);
		for (uint32_t x = 0; x < my_hm.get_width(); x++) {
			for (uint32_t y = 0; y < my_hm.get_height(); y++) {
				my_hm[x, y] = sin((float) (x + y) / 4);
			}
		}
		
		my_hm.draw("out.pgm");
	}
	
	printf("Done...\n");
	return 0;
}