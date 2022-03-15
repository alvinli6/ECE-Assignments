//--------------------------------------------------------------------------
// Perform software-level convolution to ensure functional correctness.
//--------------------------------------------------------------------------
#include "conv.h"

void model_conv (
    fm_t input_feature_map[64][184][320],
    wt_t layer_weights[64][64][3][3],
    wt_t layer_bias[64],
    fm_t output_feature_map[64][184][320]
)
{
//--------------------------------------------------------------------------
// Your code for TASK A goes here 
//
// Implement the specified convolution layer at the software level 
// without any hardware level features. Refer to the article on Sharc Lab 
// Knowledge Pool site to learn more.
//
// Hint: Do not forget to add bias and apply ReLU while writing your
//       convolution code!
//--------------------------------------------------------------------------
	/*
	// Loop for leftmost column of input map
	for (int f = 0; f < 64; f++) {	// Filter Size (Output Depth)
		for (int i = 1; i < 183; i++) {	// loop through rows of leftmost column
			for (int c = 0; c < 64; c++) {	// channels
				for (int ki = 0; ki < 3; ki++) {	// kernel row
					for (int kj = 1; kj < 3; kj++) {	// kernel column
						if (c == 0 && ki == 0 && kj == 1)
							output_feature_map[f][i][j] = input_feature_map[c][i+ki][j+kj]*
					}
				}
			}
		}
	}
	*/
	for (int f = 0; f < 64; f++) {	// Filter Size (output depth)
		for (int i = 0; i < 184; i++) {	// Output Height
			for (int j = 0; j < 320; j++) {	// Output Width
				output_feature_map[f][i][j] = layer_bias[f];
				for (int c = 0; c < 64; c++) {	// Input Depth
					for (int ki = 0; ki < 3; ki++) {	// Kernel Height
						for (int kj = 0; kj < 3; kj++) {	// Kernel Width
							/*if (c == 0 && ki == 0 && kj == 0) {
								if ((i+ki-1 < 0) || (i+ki-1 > 183) || (j+kj-1 < 0) || (j+kj-1 > 319)) {
									continue;
								} else {
									output_feature_map[f][i][j] = input_feature_map[c][i+ki-1][j+kj-1] * layer_weights[f][c][ki][kj] + layer_bias[f];
								}*/

							if ((i+ki-1 < 0) || (i+ki-1 > 183) || (j+kj-1 < 0) || (j+kj-1 > 319)) {
								continue;
							} else {
								output_feature_map[f][i][j] += input_feature_map[c][i+ki-1][j+kj-1] * layer_weights[f][c][ki][kj];
							}

						}
					}
				}
				if (output_feature_map[f][i][j] < 0)
					output_feature_map[f][i][j] = 0;
			}
		}
	}



}
