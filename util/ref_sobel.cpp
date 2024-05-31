#include "ref_sobel.h"


#define my_abs(x) (((x) >= 0) ? static_cast<float>(x) : static_cast<float>(-x))


/* Can be the same array! */
void convertGrayScale(pixel_t * data_in, float * data_out, int W, int H)
{
	for(int i = 0; i < H; i++){
		for(int j = 0; j < W; j++){
			const float r = data_in[i*W + j].r/static_cast<float>(255);
			const float g = data_in[i*W + j].g/static_cast<float>(255);
			const float b = data_in[i*W + j].b/static_cast<float>(255);
			data_out[i*W + j] = gray_rgb[0]*r + gray_rgb[1]*g + gray_rgb[2]*b;
		}
	}
}


void convRow(float * img_in, const int W, const int H, 
		const float kernel[3], float * img_out)
{
	float * in_pad = new float[H*(W + 2)]();
	for(int i = 0 ; i < H; i++){
		for(int j = 0 ; j < W; j++){
			in_pad[i*(W+2) + j + 1] = img_in[i*W + j];
		}
	}
	for(int i = 0; i < H; i++){
		for(int j = 0; j < W; j++){
			float acc = 0;
			for(int k = 0; k < 3; k ++){
				acc += in_pad[i*(W+2) + j + k] * kernel[k];
			}
			img_out[i*W + j] = acc;
		}
	}
	delete [] in_pad;
}

void convCol(float * img_in, const int W, const int H, 
		const float kernel[3], float * img_out)
{
	float * in_pad = new float[(H + 2)*W]();
	for(int i = 0 ; i < H; i++){
		for(int j = 0 ; j < W; j++){
			in_pad[(i + 1)*W + j] = img_in[i*W + j];
		}
	}
	for(int j = 0; j < W; j++){
		for(int i = 0; i < H; i++){
			float acc = 0;
			for(int k = 0; k < 3; k ++){
				acc += in_pad[(i + k)* W + j] * kernel[k];
			}
			img_out[i * W + j] = acc;
		}
	}
	delete [] in_pad;
}

void mergeChannels(float * gx, float * gy, int W, int H, uint8_t * g_out)
{
	const float factor = static_cast<float>(255);
	for(int i = 0; i < H; i++){
		for(int j = 0; j < W; j++){
			g_out[i* W  + j] = (uint8_t) ((my_abs(gx[i* W + j]) + my_abs(gy[i* W + j]))*factor);
		}
	}
}




bitmap_t * ref_sobel(bitmap_t * img_in)
{
	int width  = img_in->width;
	int height = img_in->height;
	float * m_gray = 	new float[width * height];
	float * gx1 = 		new float[width * height];
	float * gy1 = 		new float[width * height];
	float * gx2 = 		new float[width * height];
	float * gy2 = 		new float[width * height];
	uint8_t * gout =  new uint8_t[width * height];

	printf("-------------------------\n");
	printf("Starting Reference Sobel:\n");
#ifndef _WIN32
	auto start = mtick();
#endif

	// Gray values
	convertGrayScale(img_in->data,m_gray,width,height);
	convRow(m_gray,width,height,k1,gx1);
	convRow(m_gray,width,height,k2,gy1);
	convCol(gx1,width,height,k2,gx2);
	convCol(gy1,width,height,k1,gy2);
	mergeChannels(gx2,gy2,width,height,gout);

#ifndef _WIN32
	double rt = mtock(start);
	printf("Ref Total Time [ms]: %lf\n",rt);
	printf("-------------------------\n");
#endif

	bitmap_t * img_out = gray2bmp(gout,width,height);


	delete [] m_gray;
	delete [] gx2;
	delete [] gy2;
	delete [] gx1;
	delete [] gy1;
	delete [] gout;

	return img_out;
}

