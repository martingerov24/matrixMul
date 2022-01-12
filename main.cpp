#include <iostream>
#include "external/matrix.h"
int main()
{
	Matrix matrix(std::vector<int32_t> {1, 2, 3, 4, 5, 4, 3, 2, 1}, 3, 3); // this is temporary vector for move ctor
	Matrix reuslt = matrix.operator*( Matrix
											(std::vector<int32_t>{1, 0, 0, 0, 1, 0, 0, 0, 1}, 3, 3));
	return 0;
}