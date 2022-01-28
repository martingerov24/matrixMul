#include "external/CudaMatrix.h"

int main()
{
	std::vector<int32_t> vec1(4);
	for (int i = 0; i < vec1.size(); i++)
	{
		vec1[i] = rand();
	}
	std::vector<int32_t> vec2(4, 0);
	for (int i = 0; i < 2; i++)
	{
		vec2[i * 2 + i] = 1;
	}
	
	mat::Matrix matrix(std::move(vec1),  2, 2); // this is temporary vector for move ctor
	mat::Matrix matrix2(std::move(vec2), 2, 2);

	cudaError_t cudaStatus = cudaError_t(0);
	cudaStream_t stream;
	//cudaDeviceSetCacheConfig(cudaFuncCachePreferShared);
	cudaStatus = cudaSetDevice(0);
	assert(cudaStatus == cudaSuccess && "you do not have cuda capable device!");
	cudaStatus = cudaStreamCreate(&stream);
	cudaDeviceProp prop;
	cudaGetDeviceProperties(&prop, 0);
	int32_t sharedMem = static_cast<int32_t>(prop.sharedMemPerMultiprocessor);
	
	{
		NVPROF_SCOPE("whole GPU code");
		Cuda calc;
		calc.memoryAllocationForTwoVectors(stream, matrix.sizeOfMatrix(), matrix2.sizeOfMatrix(), cudaStatus);
		calc.uploadToDevice(stream, matrix, matrix2, cudaStatus); // this returns the value of the matrixDimentions stored in int
		calc.MatrixMultiplication(stream, sharedMem);
		mat::Matrix result = calc.download(stream, cudaStatus);
		calc.sync(stream, cudaStatus);
		if (matrix == result)
		{
			printf("ok\n");
		}
	}
	mat::Matrix resultCpu = matrix * matrix2;
	if (resultCpu == matrix)
	{
		printf("ok\n");
	}
	return 0;
}