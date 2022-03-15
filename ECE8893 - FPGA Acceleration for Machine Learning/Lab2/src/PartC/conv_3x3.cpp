//---------------------------------------------------------------------------
// Perform synthesizable tiling-based convolution for a single tile.
//---------------------------------------------------------------------------
#include "conv.h"

void conv_3x3 (
    fm_t Y_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH], 
    fm_t X_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH],
    wt_t W_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][3][3]
)
{
//---------------------------------------------------------------------------
// Your code for Task B and Task C goes here. 
//
// Task B: Implement a trivial functionally-correct single-tile convolution.
//         This should have an overall latency in the order of 22-23 seconds.
//         If it's worse than trivial, first fix this!
//
// Task C: Optimize to achieve an overall latency of less than 950ms 
//---------------------------------------------------------------------------
	for (int f = 0; f < OUT_BUF_DEPTH; f++) {
		for (int i = 0; i < OUT_BUF_HEIGHT; i++) {
			for (int j = 0; j < OUT_BUF_WIDTH; j++) {
				Y_buf[f][i][j] = 0;
#pragma HLS PIPELINE
				for (int c = 0; c < IN_BUF_DEPTH; c++) {
					for (int ki = 0; ki < 3; ki++) {
						for (int kj = 0; kj < 3; kj++) {
							Y_buf[f][i][j] += X_buf[c][i+ki][j+kj] * W_buf[f][c][ki][kj];
						}
					}
				}
			}
		}
	}
}
