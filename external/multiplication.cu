#include "../external/CudaMatrix.h"



__global__
void Kernel(int32_t* matrix1, int32_t * matrix2,int32_t* result, uint32_t dim)
{
	const uint8_t other_y = dim & 0xff; dim = dim >> 8;
	const uint8_t other_x = dim & 0xff; dim = dim >> 8;
	const uint8_t m_y = dim & 0xff; dim = dim >> 8;
	const uint8_t m_x = dim & 0xff;

	if (   m_y != other_x
		|| m_x != other_y
		|| m_x == 0 
		|| other_y == 0)
	{
		return;
	}

	int resultIndex = blockIdx.x * blockDim.x + threadIdx.x;

	if (resultIndex >= m_x * other_y
		|| resultIndex < 0)
	{
		return;
	}

	uint8_t A = resultIndex / m_x;
	uint8_t B = resultIndex - A * m_x;

	int32_t res = 0;
	for (int i = 0; i < m_x; i++)
	{
		res += matrix1[A * other_y+i] * matrix2[B + m_x * i];
	}

	result[resultIndex] = res;
}

__host__
void Cuda::MatrixMultiplication(cudaStream_t& providedStream, uint32_t values)
{
	uint8_t other_y = values & 0xff;
	uint8_t m_x = values >> 24;
	int8_t sizeOfBlock(((m_x * other_y + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK));
	NVPROF_SCOPE("kernel");
	Kernel << <sizeOfBlock, THREADS_PER_BLOCK, 0, providedStream >> > (m_matrix1, m_matrix2, resultM, values);
	auto status = cudaGetLastError();
}