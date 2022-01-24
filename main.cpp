#include "external/CudaMatrix.h"

int main()
{
	std::vector<int32_t> vec1(10000);
	for (int i = 0; i < vec1.size(); i++)
	{
		vec1[i] = rand();
	}
	std::vector<int32_t> vec2(10000, 0);
	for (int i = 0; i < 100; i++)
	{
		vec2[i * 100 + i] = 1;
	}
	
	mat::Matrix matrix(std::move(vec1),  100, 100); // this is temporary vector for move ctor
	mat::Matrix matrix2(std::move(vec2), 100, 100);

	cudaError_t cudaStatus = cudaError_t(0);
	cudaStream_t stream;
	//cudaDeviceSetCacheConfig(cudaFuncCachePreferShared);
	cudaStatus = cudaSetDevice(0);
	assert(cudaStatus == cudaSuccess && "you do not have cuda capable device!");
	cudaStatus = cudaStreamCreate(&stream);

	{
		NVPROF_SCOPE("whole GPU code");
		Cuda calc;
		calc.memoryAllocationForTwoVectors(stream, matrix.sizeOfMatrix(), matrix2.sizeOfMatrix(), cudaStatus);
		uint32_t v = calc.uploadToDevice(stream, matrix, matrix2, cudaStatus); // this returns the value of the matrixDimentions stored in int

		calc.MatrixMultiplication(stream, v);
		mat::Matrix result = calc.download(stream, cudaStatus);
		calc.sync(stream, cudaStatus);
		if (matrix == result)
		{
			printf("ok");
		}
	}
	return 0;
}