#include <cstdint>
#include <vector>

class Matrix
{
public:
	Matrix(std::vector<int32_t> && matrix, uint8_t x, uint8_t y) noexcept
		:m_matrix(std::move(matrix)), m_x(x), m_y(y)
	{
	}

	Matrix& operator*(Matrix& other)
	{
		if (m_matrix == other.m_matrix
			|| m_y != other.m_x
			|| m_x != other.m_y)
		{return *this;}
		Matrix result(std::vector<int32_t> {0}, m_x, other.m_y);

		for (int i = 0; i < m_x*m_y; ++i)
		{
			for (int j = 0; j < other.m_x; ++j)
			{
				
			}
		}
	}
private:
	uint8_t m_x;
	uint8_t m_y;
	std::vector<int32_t> m_matrix;
};