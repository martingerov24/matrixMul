#include "external/CudaMatrix.h"
//#include "external/matrix.h"



int main()
{
	Matrix matrix(std::vector<int32_t> {1, 2, 3, 4, 5, 4, 3, 2, 1}, 3, 3, 10); // this is temporary vector for move ctor
	Matrix mat2(std::vector<int32_t>{1, 0, 0, 0, 1, 0, 0, 0, 1}, 3, 3, 20);
	
	int size = sizeof(values);
	printf("%d", size);
	//mat2 = matrix * mat2;
	//matrix = std::move(mat2);
	
	//Matrix equ = matrix * mat2;
	const Matrix result(matrix * mat2);
	//const Matrix result(createTmp());

	for (int i = 0; i < result.m_x * result.m_y; i++)
	{
		printf("%d ", result.m_matrix[i]);
	}
	return 0;
}