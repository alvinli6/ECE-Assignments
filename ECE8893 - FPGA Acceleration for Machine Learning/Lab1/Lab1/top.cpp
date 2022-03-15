#include "dcl.h"

void matrix_mul( FIX_TYPE MatA_DRAM[M][N], FIX_TYPE MatB_DRAM[N][K], FIX_TYPE MatC_DRAM[M][K])
{
#pragma HLS interface m_axi depth=100000 port=MatA_DRAM offset=slave bundle=mem
#pragma HLS interface m_axi depth=100000 port=MatB_DRAM offset=slave bundle=mem
#pragma HLS interface m_axi depth=100000 port=MatC_DRAM offset=slave bundle=mem

#pragma HLS interface s_axilite port=return
    
    FIX_TYPE MatA[M][N];
    FIX_TYPE MatB[N][K];
    FIX_TYPE MatC[M][K];

    // Read in the data from DRAM to BRAM
    for(int i = 0; i < M; i++) {
        for(int j = 0; j < N; j++) {
            MatA[i][j] = MatA_DRAM[i][j];
        }
    }

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < K; j++) {
            MatB[i][j] = MatB_DRAM[i][j];
        }
    }
#pragma HLS array_partition variable=MatA dim=2 complete
#pragma HLS array_partition variable=MatB dim=1 complete

    for(int i = 0; i < M; i++) {
        for(int j = 0; j < K; j++) {
            MatC[i][j] = 0;
        }
    }

    // Compute the matrix multiplication
    for(int i = 0; i < M; i++) {
        for(int j = 0; j < K; j++) {
#pragma HLS pipeline II=1
            for(int p = 0; p < N; p+=10) {
            	FIX_TYPE sum_local[10];
            	for (int pp = 0; pp < 10; pp++) {
            		sum_local[pp] = MatA[i][p+pp] * MatB[p+pp][j];
            	}
            	for (int k = 0; k < 10; k++) {
            		MatC_DRAM[i][j] += sum_local[k];
            	}
            }

        }
    }

}
