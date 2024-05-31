#include "util.h"


static void write8(FILE * f, uint8_t val)
{
	if(1 != fwrite(&val,1,1,f))
		printf("ERROR failed to write Little Endian!\n");
}


static void write16(FILE * f, uint16_t val)
{
	write8(f,val & 0xFF);
	write8(f,val >> 8);
}


static void write32(FILE * f, uint32_t val)
{
	write16(f,val & 0xFFFF);
	write16(f,val >> 16);
}


uint8_t read8(FILE * f)
{
	uint8_t val;
	if( 1 != fread(&val,1,1,f)){
		printf("ERROR reading Little Endian!\n");
		return 0xFF;
	}
	return val;
}

uint16_t read16(FILE *f)
{
	uint16_t val = read8(f) | (read8(f) << 8);
	return val;
}

uint32_t read32(FILE * f)
{
	uint32_t val = read16(f) | (read16(f) << 16 );
	return val;
}





/* Write Bitmap info header 
 *
 * Can be converted to from imagemagick: convert imput.* -define bmp:format=bmp3 output.bmp
 *
 * 4: size header 40
 * 4: width (signed int)
 * 4: height (signed int)
 * 2: color planes
 * 2: bits per pixel (24 (3 channels))
 * 4: compression method (0: none) BI_RBG
 * 4: raw image size (can be 0 for no compression)
 * 4: horizontal pixel per meter ?
 * 4: vertical resolution pixel per meter
 * 4: number of colors in color palette (I guess 0)
 * 4: generally ignored set to 0
 */

struct bmp_info_s{
	uint32_t header_size = 40;
	uint32_t width;
	uint32_t height;
	uint16_t color_planes = 1;
	uint16_t bpp = 24;
	uint32_t compression_method = 0;
	uint32_t raw_image_size = 0;
	uint32_t res_hor = DEFAULT_PPM; // Default Value we don't intend to print anyways
	uint32_t res_ver = DEFAULT_PPM; // Default Value
	uint32_t n_palette = 0;
	uint32_t important_palette = 0;
};

static void writeBMP3header(FILE *f, struct bmp_info_s header)
{
	write32(f,header.header_size);
	write32(f,header.width);
	write32(f,header.height);
	//
	write16(f,header.color_planes);
	write16(f,header.bpp);
	//
	write32(f,header.compression_method);
	write32(f,header.raw_image_size);
	write32(f,header.res_hor);
	write32(f,header.res_ver);
	write32(f,header.n_palette);
	write32(f,header.important_palette);
}


static int readBMP3header(FILE *f, struct bmp_info_s * header)
{
	header->header_size = read32(f);
	if(header->header_size != 40){
		printf("WARNING header not BITMAPINFOHEADER (Size: %d)\n",
				header->header_size);
	}
	header->width = read32(f);
	header->height = read32(f);
	#ifdef BMP_DEBUG
		printf("Header size: %u \n",header->header_size);
		printf("Img: %ux%u \n",header->width,header->height);
	#endif
	header->color_planes = read16(f);
	if(header->color_planes != 1){
		printf("WARNING color planes != 1\n");
		return 1;
	}
	header->bpp = read16(f);
	if(header->bpp != 24){
		printf("ERROR wrong bpp: %d\n",header->bpp);
		return 1;
	}
	header->compression_method = read32(f);
	if(header->compression_method!= 0){
		printf("ERROR wrong compression method: %d\n",
				header->compression_method);
		return 1;
	}
	header->raw_image_size = read32(f);
	header->res_hor = read32(f);
	header->res_ver = read32(f);
#ifdef BMP_DEBUG
	printf("Resolution p/m H: %d, V: %d\n",header->res_hor,header->res_ver);
#endif 
	header->n_palette = read32(f);
	header->important_palette = read32(f);
	return 0;
}



bitmap_t * initBMP(uint32_t width,uint32_t  height)
{
	bitmap_t * bm = (bitmap_t *) malloc(sizeof(bitmap_t));
	if(bm == NULL){
		printf("ERROR: failed to allocate bitmap!\n");
		return NULL;
	}
	bm->width = width;
	bm->height = height;
	bm->data = (pixel_t*) malloc(sizeof(pixel_t) * width*height);
	if(bm->data == NULL){
		printf("ERROR: failed to allocated data array!\n");
		return NULL;
	}
	return bm;
}

bitmap_t * randBMP(uint32_t width, uint32_t height)
{
	bitmap_t * bm = initBMP(width,height);
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			bm->data[i*width + j].r = (uint8_t) (rand() % 256);
			bm->data[i*width + j].g = (uint8_t) (rand() % 256);
			bm->data[i*width + j].b = (uint8_t) (rand() % 256);
		}
	}
	return bm;
}


/*
 * Can read both BMP3 and BMP4
 */
bitmap_t * readBMP(const char * infile)
{
	FILE * f = fopen(infile,"rb");
	uint16_t val = read16(f);
	if(val != 0x4d42){
		printf("ERROR wrong bmp val!\n");
		return NULL;
	}
	uint32_t size = read32(f);
#ifdef BMP_DEBUG
	printf("BMP size: %u \n",size);
#endif
	/* Discard 4 bytes */
	read32(f);
	uint32_t offset = read32(f);
	struct bmp_info_s bmp_info;
	if(readBMP3header(f,&bmp_info)){
		return NULL;
	}
	uint32_t width = bmp_info.width;
	uint32_t height = bmp_info.height;
	if(fseek(f,offset,SEEK_SET)){
		printf("ERROR fseek failed!\n");
		return NULL;
	}
	bitmap_t * bm = initBMP(width,height);
	if(bm == NULL)
		return NULL;
	for(int i = height - 1; i >= 0; i--){
		pixel_t * row = bm->data + i*width;
		for(int j = 0; j < width; j++){
			row[j].r = read8(f);
			row[j].g = read8(f);
			row[j].b = read8(f);
		}
		int tot = width * 3;
		while(tot  % 4){
			read8(f);
			tot++;
		}
	}
	fclose(f);
	return bm;
}



void writeBMP(bitmap_t * bm, const char * outfile)
{
	struct bmp_info_s bmpinfo;
	int bpp = 3;
	FILE * f = fopen(outfile,"wb");
	// Header to indetify BMP and DIB
	write16(f,0x4d42); //2
	// Size 
	uint32_t size = bm->width*bm->height * 3 + 14 + bmpinfo.header_size; // Total size
	write32(f,size); // 4
	// Reserved can be 0 
	write16(f,0); // 2
	write16(f,0); // 2
	// Offset is the 14 bytes + 40 bytes for dib header
	write32(f, 14 + bmpinfo.header_size);
	// Write the actualheader
	bmpinfo.width= bm->width;
	bmpinfo.height= bm->height;
	bmpinfo.raw_image_size = bm->width * bm->height * 3;
	writeBMP3header(f,bmpinfo);
	// Now write the data!
	for(int i = bm->height - 1; i >= 0; i--){
		pixel_t * row = bm->data + i*bm->width;
		for(int j = 0; j < bm->width; j++){
			write8(f,row[j].r);
			write8(f,row[j].g);
			write8(f,row[j].b);
		}
		int tot = bm->width* 3;
		while(tot  % 4){
			write8(f,0);
			tot++;
		}
	}
}


void freeBMP(bitmap_t * bmp)
{
	if(bmp->data != NULL)
		free(bmp->data);
	free(bmp);
}


/*
 * Needs to be freed with freeBMP afterwards!
 */
bitmap_t * gray2bmp(uint8_t * m_gray,int width, int height)
{
	bitmap_t * bm_out = initBMP(width,height);
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			const uint8_t val =  m_gray[i*width + j];
			bm_out->data[i*width + j].r = val;
			bm_out->data[i*width + j].g = val;
			bm_out->data[i*width + j].b = val;
		}
	}
	return bm_out;
}


float bm_gray_diff(
		bitmap_t * bm0,
		bitmap_t * bm1
		)
{
	float avg_diff = 0.0;
	int n = 0;
	int height = bm0->height;
	int width = bm0->width;
	if((width != bm1->width) || (height != bm1->height)){
		printf("WARNING: Bitmaps width and height don't match!\n");
		return 1000;
	}

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			uint8_t diff = abs(bm0->data[i*width + j].r - bm1->data[i*width + j].r);
			if(diff){
				avg_diff += (float) diff;
				n++;
			}
		}
	}
	printf("Avg Diff %f, #diff %d\n",avg_diff/(height*width),n);
	return avg_diff/(height*width);
}




#ifndef _WIN32
struct timespec mtick(){
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);
	return start;
}

double mtock(struct timespec start){
	struct timespec stop;
	clock_gettime(CLOCK_MONOTONIC, &stop);
	double time = (stop.tv_sec - start.tv_sec)*1000 + 
		(stop.tv_nsec - start.tv_nsec)/(1000.f * 1000.f);
	return time;
}
#endif


