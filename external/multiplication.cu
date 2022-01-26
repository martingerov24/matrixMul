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

	if (resultIndex >= m_x 
		|| resultIndex < 0)
	{
		return;
	}

	int32_t wBord; 
	uint8_t A = resultIndex / m_x;
	uint8_t B = resultIndex - A * m_x;
	wBord = A / BORDER_SIZE + B / BORDER_SIZE;
	
	__shared__ int32_t A_matrixRow[BORDER_SIZE * BORDER_SIZE];
	__shared__ int32_t B_matrixCol[BORDER_SIZE * BORDER_SIZE];

	A_matrixRow[resultIndex] = matrix1[resultIndex];
	__syncwarp();									// this might make a problem
		for (int i = 0; i < m_x ; i+=BORDER_SIZE)
		{
#pragma unroll
			for (int j = 0; j < BORDER_SIZE; j++)
			{
				B_matrixCol[j] = matrix2[B + m_x * i + j];
				__syncwarp();
			}
			int32_t m[7] =
			{
				(A_matrixRow[0] +  A_matrixRow[3]) *(B_matrixCol[0] + B_matrixCol[3]),
				(A_matrixRow[1] +  A_matrixRow[3]) *(B_matrixCol[0]),
				(A_matrixRow[0])* (B_matrixCol[2]  - B_matrixCol[3]),
				(A_matrixRow[3])* (B_matrixCol[1]  - B_matrixCol[0]),
				(A_matrixRow[0] +  A_matrixRow[2]) * B_matrixCol[3],
				(A_matrixRow[1] -  A_matrixRow[1]) *(B_matrixCol[0] + B_matrixCol[2]),
				(A_matrixRow[2] -  A_matrixRow[3]) *(B_matrixCol[0] + B_matrixCol[2]),
			};
			result[0] += m[0] + m[3] - m[4] + m[6];
			result[1] += m[2] + m[4];
			result[2] += m[1] + m[3];
			result[3] += m[0] - m[1] + m[2] + m[5];

		}
	//__shared__ int32_t s_matrix2[100 * 100];
	//s_matrix2[resultIndex] = matrix2[resultIndex];

}


__host__
void Cuda::MatrixMultiplication(cudaStream_t& providedStream, int32_t sharedMem)
{

	int8_t sizeOfBlock(((x * y + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK));
	NVPROF_SCOPE("kernel");
	Kernel << <sizeOfBlock, THREADS_PER_BLOCK, 0, providedStream >> > (m_matrix1, m_matrix2, resultM, x, y);
	auto status = cudaGetLastError();
}

//__host__
//void Cuda::DivideAndConquer(cudaStream_t& providedStream)
//{
//	int8_t sizeOfBlock(((x + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK));
//	NVPROF_SCOPE("kernel");
//	DivideAndConquer<< <sizeOfBlock, THREADS_PER_BLOCK, 0, providedStream >> > (m_matrix1, m_matrix2, resultM, x, y);
//	auto status = cudaGetLastError();
//}