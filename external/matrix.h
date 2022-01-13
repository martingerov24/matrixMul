#include <cstdint>
#include <vector>

class Matrix
{
public:
	Matrix(std::vector<int32_t>& matrix, uint8_t x, uint8_t y, int id) noexcept
		:m_matrix(matrix), m_x(x), m_y(y), id(id)
	{
		printf("constructor\n");
	}
	~Matrix() 
	{
		printf("destructor\n");
	}
	Matrix(const Matrix && other) noexcept
		: m_matrix(std::move(other.m_matrix))
		, m_x(other.m_x)
		, m_y(other.m_y)
		,id(other.id+1)
	{
		printf("move\n");
	}
	
	Matrix(const Matrix& other) noexcept
		: m_matrix(other.m_matrix)
		, m_x(other.m_x)
		, m_y(other.m_y)

	{
		printf("BOOM");
	}
	
	Matrix& operator=(Matrix& other) 
	{
		printf("assigment\n");
		return *this;
	}

	Matrix& operator=( Matrix && other) noexcept
	{
		printf("Move Ass");
		if (other.m_matrix == m_matrix)
		{
			return *this;
		}
		m_matrix = std::move(other.m_matrix);
		return *this;
	}

	const Matrix operator*(const Matrix& other) noexcept
	{
		if (m_matrix == other.m_matrix
			|| m_y != other.m_x
			|| m_x != other.m_y)
		{return *this;}

		Matrix result(std::vector<int32_t>(0), m_x, other.m_y, 30);
		result.m_matrix.resize(other.m_y * m_x);
		
		int32_t res = 0;
		uint8_t count = 0;
		uint8_t otherCount = 0;
		for (int i = 0; i < result.m_matrix.size(); ++i)
		{
			if (i % result.m_x == 0 && i != 0)
				{ count++; otherCount = 0; }
			uint8_t cur = count * m_x;
			res += m_matrix[cur] *	   other.m_matrix[otherCount];
			res += m_matrix[cur + 1] * other.m_matrix[otherCount + m_x];
			res += m_matrix[cur + 2] * other.m_matrix[otherCount + m_x * 2];
			result.m_matrix[i] = res;
			otherCount++;
			res = 0;
		}

		return std::move(result);
	}

	uint8_t m_x;
	uint8_t m_y;
	std::vector<int32_t> m_matrix;
	int id = 0;
};