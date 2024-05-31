#include "sobel.h"

#define my_abs(x) (((x) >= 0) ? static_cast<float>(x) : static_cast<float>(-x))

void convGray(
		const int N,
		const pixel_t X_in[],
		float g_out[]
		)
{
	for(int i = 0; i < N; i++){
		g_out[i] = X_in[i].r * gray_rgb[0] + X_in[i].g*gray_rgb[1] + X_in[i].b*gray_rgb[2];
	}
}

void convRow(
		const int W,
		const int H,
		const float w_in[3],
		const float X_in[],
		float Z_out[]
		)
{
RowLoop:
	for(int i = 0; i < H; i++){
		for(int j = 0; j < W; j++){
			float acc =0;
			for(int k = -1 ; k < 2; k++){
				float nv;
				if((k + j) >= 0  && (k + j < W)) 
					nv = X_in[i*W + k + j];
				else
					nv = 0;
				acc += nv * w_in[k + 1];
			}
			Z_out[i*W + j] = acc;
		}
	}
}

void convCol(
		const int W,
		const int H,
		const float w_in[3],
		const float X_in[],
		float Z_out[]
		)
{
ColLoop:
	for(int j = 0; j < W; j++){
		for(int i = 0; i < H; i++){
			float acc =0;
			for(int k = -1 ; k < 2; k++){
				float nv;
				if((k + i) >= 0  && (k + i < H)) 
					nv = X_in[j + (k + i)*W];
				else
					nv = 0;
				acc += nv * w_in[k + 1];
			}
			Z_out[i*W + j] = acc;
		}
	}
}

void mergeGrad(
		const int N,
		const float Gx[],
		const float Gy[],
		uint8_t img_out[]
		)
{
	for(int i = 0 ; i< N ; i++){
		img_out[i] =(uint8_t) ((my_abs(Gx[i]) + my_abs(Gy[i]))/4);
	}
}




void hls_sobel(
		const int W,
		const int H,
		const pixel_t img_in[],
		uint8_t img_out[]
		)
{
#pragma HLS INTERFACE mode=m_axi port=img_in bundle=gmem0 depth=(RTL_SIM_HEIGHT * RTL_SIM_WIDTH)
#pragma HLS INTERFACE mode=m_axi port=img_out bundle=gmem0 depth=(RTL_SIM_WIDTH * RTL_SIM_HEIGHT)
#pragma HLS INTERFACE mode=ap_ctrl_hs port=return
#pragma HLS INTERFACE mode=s_axilite port=return
#pragma HLS INTERFACE s_axilite port=H
#pragma HLS INTERFACE s_axilite port=W

	/* Insert your code here */

	/* ------------------------------------------*/
	/* Replace this solution! */
	/* ------------------------------------------*/
#define REF_MAX (RTL_SIM_HEIGHT*RTL_SIM_WIDTH)
	printf("Running Example Implementation!\n");
	if(H*W > REF_MAX){
		printf("Input too big!\n");
		return;
	}
	float X_int[REF_MAX];
	float   Gx0[REF_MAX];
	float   Gy0[REF_MAX];
	float   Gx1[REF_MAX];
	float   Gy1[REF_MAX];

	convGray(W*H,img_in,X_int);
	convRow(W,H,k1,X_int,Gx0);
	convRow(W,H,k2,X_int,Gy0);
	convCol(W,H,k2,Gx0,Gx1);
	convCol(W,H,k1,Gy0,Gy1);
	mergeGrad(W*H,Gx1,Gy1,img_out);
	/* ------------------------------------------*/
	/* Example End */
	/* ------------------------------------------*/


}
