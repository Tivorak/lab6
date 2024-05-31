#ifndef SOBEL_UTIL_H
#define SOBEL_UTIL_H

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <time.h>



/* Sobel Specific Stuff */
typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t pad;
}pixel_t;

#define MAX_IMAGE_WIDTH  4608
#define MAX_IMAGE_HEIGHT 4608

const float gray_rgb[3] = {0.21875,0.71875,0.0625};

const float k1[3] = {1,0,-1};
const float k2[3] = {1,2,1};


/* Can be used to meassure time */
#ifndef _WIN32
struct timespec mtick();
double mtock(struct timespec start);
#endif



/* Bitmap Reader */
// #define BMP_DEBUG
#define DEFAULT_PPM 7000 // Roughly 4k 27 inch screen
typedef struct{
	uint32_t width;
	uint32_t height;
	pixel_t * data; 
} bitmap_t;


bitmap_t * initBMP(uint32_t  width, uint32_t height);
bitmap_t * readBMP(const char * infile);
bitmap_t * randBMP(uint32_t  width, uint32_t height);

void writeBMP(bitmap_t * bm, const char * outfile);
void freeBMP(bitmap_t * bm);

/*
 * Needs to be freed with freeBMP afterwards!
 */
bitmap_t * gray2bmp(uint8_t * m_gray,int width, int height);

/*
 * Returns the average difference between pixels of two Bitmaps
 */
float bm_gray_diff(bitmap_t * bm0,bitmap_t * bm1);


#endif
