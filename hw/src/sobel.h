#ifndef SOBEL_H
#define SOBEL_H



#include "util.h"




/* 
 * Set this to the Width and Height used in the C/RTL Co-Simulation
 */
#define RTL_SIM_WIDTH 256
#define RTL_SIM_HEIGHT 192



void hls_sobel(
		const int width,
		const int height,
		const pixel_t img_in[],
		uint8_t img_out[]
		);

#endif
