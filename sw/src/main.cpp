


#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>
#include <xrt/xrt_bo.h>
#include <xrt/xrt_uuid.h>


#include "util.h"
#include "ref_sobel.h"
#include "bitLoad.h"

bitmap_t * hw_sobel(bitmap_t * data_in)
{
	auto device = xrt::device(0);
	std::cout << "device name:     " << device.get_info<xrt::info::device::name>() << "\n";
 	auto xclbin_uuid = device.load_xclbin("./sobel.xclbin");
	loadBitstream("./sobel.xclbin");
	std::cout << "device bdf:  " << device.get_info<xrt::info::device::bdf>() << "\n";
	auto krnl = xrt::kernel(device, xclbin_uuid, "hls_sobel",xrt::kernel::cu_access_mode::exclusive);
	int N = data_in->width*data_in->height;

	auto img_in = xrt::bo(device,sizeof(pixel_t) * N,0);
	auto img_out= xrt::bo(device,sizeof(uint8_t) * N,0);
	uint8_t * g_out = new uint8_t[data_in->width*data_in->height];

	double t[3];
	auto start = mtick();

	/* Start Writing to Memory Buffer */
	img_in.write(data_in->data);
	t[0] = mtock(start);

	start = mtick();
	auto run = krnl(data_in->width,data_in->height,img_in,img_out);
	run.wait();
	t[1] = mtock(start);

	start = mtick();
	img_out.read(g_out);
	t[2] = mtock(start);

	/* Stop time after resyncing buffer */
	printf("---------------------------\n");
	printf("HW write time[ms]: %lf\n",t[0]);
	printf("HW exec  time[ms]: %lf\n",t[1]);
	printf("HW read  time[ms]: %lf\n",t[2]);
	printf("---------------------------\n");
	printf("HW total time[ms]: %lf\n",t[0] + t[1] + t[2]);
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
