#pragma once

#define THREADS_PER_BLOCK 128
#include "cuda_runtime.h"
#include "cuda/std/cmath"
#include "device_launch_parameters.h"
#include "nvtx3/nvToolsExt.h"

#include "../external/matrix.h"
#include <cassert>
#include <vector>


struct NVProf {
	NVProf(const char* name) {
		nvtxRangePush(name);
	}
	~NVProf() {
		nvtxRangePop();
	}
};
#define NVPROF_SCOPE(X) NVProf __nvprof(X);


class Cuda
{
public:
	Cuda() = default;

	__host__
		void memoryAllocationForTwoVectors(cudaStream_t& providedStream, int32_t size1, int32_t size2, cudaError_t cudaStatus)
	{
		NVPROF_SCOPE("memory allocation async");
		cudaStatus = cudaMallocAsync((void**)&m_matrix1, size1 * sizeof(int32_t), providedStream);
		assert(cudaStatus == cudaSuccess && "cudaMalloc failed!");

		cudaStatus = cudaMallocAsync((void**)&m_matrix2, size2 * sizeof(int32_t), providedStream);
		assert(cudaStatus == cudaSuccess && "cudaMalloc failed!");

		cudaStatus = cudaMallocAsync((void**)&resultM, size1 * sizeof(int32_t), providedStream);
		assert(cudaStatus == cudaSuccess && "cudaMalloc failed!");
	}

	__host__
		void uploadToDevice(cudaStream_t& providedStream, mat::Matrix& first, mat::Matrix& second, cudaError_t cudaStatus) noexcept
	{
		if (first.isCompatible(second)) {
			FreeMemory(providedStream);
			throw "now compatible matrixes";
		}
		x = first.x();
		y = second.y();

		NVPROF_SCOPE("memcpy async");
		cudaStatus = cudaMemcpyAsync(m_matrix1, first.dataPointer(), sizeof(int32_t) * first.sizeOfMatrix(), cudaMemcpyHostToDevice, providedStream);
		assert(cudaStatus == cudaSuccess && "not able to trainsfer data, between host and device");

		cudaStatus = cudaMemcpyAsync(m_matrix2, second.dataPointer(), sizeof(int32_t) * second.sizeOfMatrix(), cudaMemcpyHostToDevice, providedStream);
		assert(cudaStatus == cudaSuccess && "not able to trainsfer data, between host and device");
	}

		
	__host__
		void MatrixMultiplication(cudaStream_t& providedStream, int32_t sharedMem = 0);
	__host__
		mat::Matrix download(cudaStream_t& providedStream, cudaError_t cudaStatus)
	{
		NVPROF_SCOPE("dowload async");
		mat::Matrix result(std::vector<int32_t>(x * y, 0), x, y);
		int a = sizeof(int32_t) * result.sizeOfMatrix();
		cudaStatus = cudaMemcpyAsync(result.dataPointer(), resultM, sizeof(int32_t) * result.sizeOfMatrix(), cudaMemcpyDeviceToHost, providedStream);
		assert(cudaStatus == cudaSuccess && "not able to transfer device to host!");
		return result;
	}

	__host__
		void sync(cudaStream_t& providedStream, cudaError_t cudaStatus)
	{
		NVPROF_SCOPE("synchronize");
		cudaStatus = cudaStreamSynchronize(providedStream);
	}
	__host__
		void FreeMemory(cudaStream_t& providedStream)
	{
		cudaFreeAsync(m_matrix1, providedStream);
		cudaFreeAsync(m_matrix2, providedStream);
		cudaFreeAsync(resultM, providedStream);
	}

	__host__
		~Cuda() = default;

private:
	int32_t* m_matrix1;
	int32_t* m_matrix2;
	int32_t* resultM;
	int x, y;
};
