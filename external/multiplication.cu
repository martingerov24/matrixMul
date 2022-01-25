#include "../external/CudaMatrix.h"



__global__
void Kernel(int32_t* matrix1, int32_t * matrix2,int32_t* result, int32_t m_x, int32_t other_y)
{
	if ( m_x != other_y
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

	__shared__ int32_t s_matrix[100*100];
	//__shared__ int32_t s_matrix2[100 * 100];
	s_matrix[resultIndex] = matrix1[resultIndex];
	//s_matrix2[resultIndex] = matrix2[resultIndex];
	__syncthreads();
	uint8_t A = resultIndex / m_x;
	uint8_t B = resultIndex - A * m_x;

	int32_t res = 0;

	for (int i = 0; i < m_x; i++)
	{
		res += s_matrix[A * other_y+i] * matrix2[B + m_x * i];
	}

	result[resultIndex] = res;
}

__global__
void DivideAndConquer(int32_t* matrix1, int32_t* matrix2, int32_t* result, const int32_t m_x, const int32_t other_y)
{
	int resultIndex = blockIdx.x * blockDim.x + threadIdx.x;

	if (resultIndex >= m_x * other_y
		|| resultIndex < 0)
	{
		return;
	}

	int32_t wBord; 
	uint8_t A = resultIndex / m_x;
	uint8_t B = resultIndex - A * m_x;
	wBord = A / BORDER_SIZE + B / BORDER_SIZE;

	__shared__ int32_t A_matrixRow[36];
	__shared__ int32_t B_matrixCol[36];
	B_matrixCol[resultIndex] = matrix2[resultIndex];
	__shared__ int32_t Result[36];
	__syncthreads();


	for (int i = 0; i < m_x; i++)
	{
		A_matrixRow[resultIndex] = matrix1[resultIndex];

	}
	//__shared__ int32_t s_matrix2[100 * 100];
	//s_matrix2[resultIndex] = matrix2[resultIndex];

	result[resultIndex] = res;
}


__host__
void Cuda::MatrixMultiplication(cudaStream_t& providedStream, int32_t sharedMem)
{

	int8_t sizeOfBlock(((x * y + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK));
	NVPROF_SCOPE("kernel");
	Kernel << <sizeOfBlock, THREADS_PER_BLOCK, 0, providedStream >> > (m_matrix1, m_matrix2, resultM, x, y);
	auto status = cudaGetLastError();
}