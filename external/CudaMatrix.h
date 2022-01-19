#pragma once
#define THREADS_PER_BLOCK 1024
#include "cuda_runtime.h"
#include "cuda/std/cmath"
#include "device_launch_parameters.h"
#include "nvtx3/nvToolsExt.h"

#include "../external/matrix.h"
#include <vector>
#include <cassert>


class Cuda
{
public:
	Cuda() = default;

	__host__
		void memoryAllocationForTwoVectors(cudaStream_t& providedStream, int32_t size1, int32_t size2, cudaError_t cudaStatus)
	{
		cudaStatus = cudaMallocAsync((void**)&m_matrix1, size1 * sizeof(int32_t), providedStream);
		assert(cudaStatus == cudaSuccess && "cudaMalloc failed!");

		cudaStatus = cudaMallocAsync((void**)&m_matrix2, size2 * sizeof(int32_t), providedStream);
		assert(cudaStatus == cudaSuccess && "cudaMalloc failed!"); 

		cudaStatus = cudaMallocAsync((void**)&resultM, size2 * sizeof(int32_t), providedStream);
		assert(cudaStatus == cudaSuccess && "cudaMalloc failed!");
	}

	__host__
		int32_t uploadToDevice(cudaStream_t& providedStream, Matrix && first, Matrix && second, cudaError_t cudaStatus) noexcept
	{
		if (first.isCompatible(second.m_x, second.m_y)) { 
			FreeMemory(providedStream); 
			throw "now compatible matrixes"; 
		}
		intToDim v;
		v.dim.m_x = first.m_x;
		x = first.m_x;
		y = second.m_y;
		v.dim.m_y = first.m_y;
		v.dim.other_x = second.m_x;
		v.dim.other_y = second.m_y;

		cudaStatus = cudaMemcpyAsync(m_matrix1, first.m_matrix.data(), sizeof(int32_t) * first.sizeOfMatrix(), cudaMemcpyHostToDevice, providedStream);
		assert(cudaStatus == cudaSuccess && "not able to trainsfer data, between host and device");

		cudaStatus = cudaMemcpyAsync(m_matrix2, second.m_matrix.data(), sizeof(int32_t) * second.sizeOfMatrix(), cudaMemcpyHostToDevice, providedStream);
		assert(cudaStatus == cudaSuccess && "not able to trainsfer data, between host and device");
		return v.integer;
	}

	union intToDim
	{
		struct Dim
		{
			int8_t m_x;
			int8_t m_y;
			int8_t other_x;
			int8_t other_y;
		}dim;
		uint32_t integer;
	};

	__host__
		void MatrixMultiplication(cudaStream_t& providedStream, uint32_t values);
	__host__
		Matrix download(cudaStream_t& providedStream, cudaError_t cudaStatus)
	{
		Matrix result(std::vector<int32_t>(0), x, y);
		cudaStatus = cudaMemcpyAsync(result.m_matrix.data(), resultM, sizeof(int32_t) * result.sizeOfMatrix(), cudaMemcpyDeviceToHost, providedStream);
		assert(cudaStatus == cudaSuccess && "not able to transfer device to host!");
		return result;
	}
	
	__host__
		void sync(cudaStream_t& providedStream, cudaError_t cudaStatus)
	{
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

protected:
	int32_t* m_matrix1;
	int32_t* m_matrix2;
	int32_t* resultM; 
	int x, y;
};
