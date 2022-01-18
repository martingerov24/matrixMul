#pragma once
#define THREADS_PER_BLOCK 1024
#include "../external/matrix.h"
#include "cuda_runtime.h"
#include "cuda/std/cmath"
#include "device_launch_parameters.h"
#include "nvtx3/nvToolsExt.h"

#include <vector>
#include <cassert>


class Cuda
{
public:
	Cuda() = default;

	__host__
		void memoryAllocationForTwoVectors(cudaStream_t& providedStream, int32_t size1, int32_t size2)
	{
		cudaStatus = cudaMallocAsync((void**)&m_matrix1, size1 * sizeof(int32_t), providedStream);
		assert(cudaStatus == cudaSuccess && "cudaMalloc failed!");

		cudaStatus = cudaMallocAsync((void**)&m_matrix2, size2 * sizeof(int32_t), providedStream);
		assert(cudaStatus == cudaSuccess && "cudaMalloc failed!");
	}

	__host__
		void uploadToDevice(cudaStream_t& providedStream, Matrix && first, Matrix && second) noexcept
	{
		if (first.isCompatible(second.m_x, second.m_y)) { return; }

		cudaStatus = cudaMemcpyAsync(m_matrix1, first.m_matrix.data(), sizeof(int32_t) * first.sizeOfMatrix(), cudaMemcpyHostToDevice, providedStream);
		assert(cudaStatus == cudaSuccess && "not able to trainsfer data, between host and device");

		cudaStatus = cudaMemcpyAsync(m_matrix2, second.m_matrix.data(), sizeof(int32_t) * second.sizeOfMatrix(), cudaMemcpyHostToDevice, providedStream);
		assert(cudaStatus == cudaSuccess && "not able to trainsfer data, between host and device");
	}
	struct dimentionsOfMatrix
	{
		dimentionsOfMatrix(uint8_t m_x, uint8_t m_y, uint8_t other_x, uint8_t other_y)
			:m_x(m_x), m_y(m_y), other_x(other_x), other_y(other_y)
		{}
		int32_t m_x : 8;
		int32_t m_y : 8;
		int32_t other_x : 8;
		int32_t other_y : 8;
	};

	__host__
		void MatrixMultiplication(cudaStream_t& providedStream, dimentionsOfMatrix values);
	__host__
		void download(cudaStream_t& providedStream, int32_t x, int32_t y)
	{
		Matrix result(std::vector<int32_t>(0), x, y);
		cudaStatus = cudaMemcpyAsync(result.m_matrix.data(), m_matrix1, sizeof(int32_t) * result.sizeOfMatrix(), cudaMemcpyDeviceToHost, providedStream);
		assert(cudaStatus == cudaSuccess && "not able to transfer device to host!");
	}
	
	__host__
		void sync(cudaStream_t& providedStream)
	{
		cudaStatus = cudaStreamSynchronize(providedStream);
	}

	__host__
		~Cuda()
	{
		cudaFree(m_matrix1);
		cudaFree(m_matrix2);
	}

protected:
	int m_x, m_y;
	int32_t* m_matrix1;
	int32_t* m_matrix2;
	cudaError_t cudaStatus;
};
