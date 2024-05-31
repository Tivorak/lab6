#include "util.h"
#include "ref_sobel.h"
#include "sobel.h"




bitmap_t * hw_sobel(bitmap_t * data_in)
{
	uint8_t * g_out = new uint8_t[data_in->width*data_in->height];
	hls_sobel(data_in->width,data_in->height,data_in->data,g_out);
	bitmap_t * r = gray2bmp(g_out,data_in->width,data_in->height);
	delete [] g_out;
	return r;
}


int main(int argc, char * argv[])
{
	if((argc < 3) || (argc > 4) ){
		printf("./bmp infile outfile\n");
		printf("./bmp r width height\n");
		return 1;
	}
	bitmap_t * bm;
	if(argc == 4){
		if(argv[1][0] != 'r'){
			printf("./bmp r width height\n");
			return 1;
		}
		srand(static_cast<unsigned>(time(nullptr)));
		bm = randBMP(atoi(argv[3]),atoi(argv[2]));
	}
	else
		bm = readBMP(argv[1]);



	int width = bm->width;
	int height = bm->height;
	printf("Input image %dx%d\n",width,height);
	
	if((width > MAX_IMAGE_WIDTH) || (height > MAX_IMAGE_HEIGHT)){
		printf("ERROR: Image too big max H: %d max W: %d \n",
				MAX_IMAGE_HEIGHT,
				MAX_IMAGE_WIDTH);
		freeBMP(bm);
		return 1;
	}


	/* Reference Solution */
	bitmap_t * bm_ref = ref_sobel(bm);

	/* HW Solution */
	bitmap_t * bm_out = hw_sobel(bm);

	int ret = 0;
	if(bm_gray_diff(bm_out,bm_ref) > 0.5)
		ret = 1;


	if(argc == 3){
		writeBMP(bm_out,argv[2]);
		writeBMP(bm_ref,"./ref.bmp");
	}

	freeBMP(bm);
	freeBMP(bm_out);
	freeBMP(bm_ref);
	return ret;
}

