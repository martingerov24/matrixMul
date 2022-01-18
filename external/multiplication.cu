#include "../external/CudaMatrix.h"

__device__
union intToDim
{
	struct dimentionsOfMatrix {
		int32_t m_x : 8;
		int32_t m_y : 8;
		int32_t other_x: 8;
		int32_t other_y : 8;
	};

	int32_t value;
};

__global__
void Kernel(int32_t* matrix1, int32_t * matrix2, int32_t dim)
{
	
}

__host__
void Cuda::MatrixMultiplication(cudaStream_t& providedStream, intToDim values)
{
	dim3 sizeOfBlock(((values.dim.m_x + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK), values.dim.m_y);
	int32_t v = values.integer;
	Kernel << <sizeOfBlock, THREADS_PER_BLOCK, 0, providedStream >> > (m_matrix1, m_matrix2, v);
	auto status = cudaGetLastError();
}