#include "external/CudaMatrix.h"
//#include "external/matrix.h"

void CudaMatrixMultiplication(Matrix && matrix, Matrix &&mat2)
{
	cudaError_t cudaStatus = cudaError_t(0);
	cudaStream_t stream;
	cudaDeviceSetCacheConfig(cudaFuncCachePreferShared);
	cudaStatus = cudaSetDevice(0);
	assert(cudaStatus == cudaSuccess && "you do not have cuda capable device!");
	cudaStatus = cudaStreamCreate(&stream);

	Cuda calc;
	calc.memoryAllocationForTwoVectors(stream, matrix.sizeOfMatrix(), mat2.sizeOfMatrix(), cudaStatus);
	uint32_t v = calc.uploadToDevice(stream, std::move(matrix), std::move(mat2), cudaStatus); // this returns the value of the matrixDimentions stored in int
	
	calc.MatrixMultiplication(stream, v);
	Matrix result = calc.download(stream, cudaStatus);
	calc.sync(stream, cudaStatus);
	for (size_t i = 0; i < result.sizeOfMatrix() ; i++)
	{
		printf("%d ", result.m_matrix[i] );
	}
}

int main()
{
	Matrix matrix(std::vector<int32_t> {1, 2, 3, 4, 5, 4, 3, 2, 1}, 3, 3, 10); // this is temporary vector for move ctor
	Matrix mat2(std::vector<int32_t>   {1, 0, 0, 0, 1, 0, 0, 0, 1}, 3, 3, 20);
	CudaMatrixMultiplication(std::move(matrix), std::move(mat2));
	
	return 0;
}