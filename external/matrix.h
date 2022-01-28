#include <cstdint>
#include <vector>
#include <tuple>
namespace mat
{
class Matrix
{
private: 
	std::vector<int32_t>& twoByTwo(const Matrix &r, int x, int y) noexcept
	{	
		//this represents the little matrix created from left and right input matrices
		const int f[]=
		{
			{m_matrix[y * m_x + x]}, 		{m_matrix[y * m_x + x + 1]},
			{m_matrix[(y + 1) * m_x + x]},  {m_matrix[(y + 1) * m_x + x + 1] }
		};
		const int s[]=
		{
			{r.m_matrix[y		* r.m_x + x]}, 	{r.m_matrix[y		* r.m_x + x + 1]},
			{r.m_matrix[(y + 1) * r.m_x + x]},  {r.m_matrix[(y + 1) * r.m_x + x + 1] }
		};
		//strassen's algorithm
		const int32_t m[] = {(f[0] + f[3]) * (s[0] + s[3]),
			(f[1] +  f[3]) * (s[0]),
			(f[0]) *(s[2] - s[3]),
			(f[3]) *(s[1] - s[0]),
			(f[0] +  f[2]) * s[3],
			(f[1] -  f[0]) * (s[0] + s[2]),
			(f[2] -  f[3]) * (s[1] + s[3])
		};
		
		return std::vector<int32_t> {
			{ m[0] + m[3] - m[4] + m[6]},
			{ m[1] + m[3] },
			{ m[2] + m[4] },
			{ m[0] - m[1] + m[2] + m[5] }
		};
	}


public:
	
	Matrix(std::vector<int32_t>& matrix, uint8_t x, uint8_t y) noexcept
		:m_matrix(matrix), m_x(x), m_y(y)
	{
		printf("constructor\n");
	}


	~Matrix() = default;
		
	Matrix(const Matrix&& other) noexcept
		: m_matrix(std::move(other.m_matrix))
		, m_x(other.m_x)
		, m_y(other.m_y)
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

	uint32_t sizeOfMatrix() { return m_x * m_y; }

	Matrix& operator=(Matrix&& other) noexcept
	{
		printf("Move Ass");
		if (other.m_matrix == m_matrix)
		{
			return *this;
		}
		m_matrix = std::move(other.m_matrix);
		return *this;
	}

	bool isCompatible(const Matrix & other)
	{
		if (m_y != other.m_x
			|| m_x != other.m_y)
		{
			return 1;
		}
		return 0;
	}

	bool operator==(Matrix& other_m)
	{
		return other_m.m_matrix == m_matrix ? 1 : 0;
	}

	const Matrix operator*(Matrix& other)
	{
		if ((m_x & 0x1) == 1
			|| (other.m_y & 0x1) == 1)
		{
			return linearMultiplication(other);
		}

		Matrix result(std::vector<int32_t>(m_x * other.m_y / 4), m_x, other.m_y);
		std::vector<int32_t> temp_resultOfTwoByTwo(4, 0);

		for (int i = 0; i < other.m_y; i += 2)
		{
			for (int j = 0; j < m_x; j += 2)
			{
				temp_resultOfTwoByTwo = twoByTwo(other, i, j);
				result.m_matrix[result.m_y * i + j] += temp_resultOfTwoByTwo[0];
				result.m_matrix[result.m_y * i + j + 1] += temp_resultOfTwoByTwo[1];
				result.m_matrix[result.m_y * (i + 1) + j] += temp_resultOfTwoByTwo[2];
				result.m_matrix[result.m_y * (i + 1) + j + 1] += temp_resultOfTwoByTwo[3];
			}
		}
		return result;
	}
	const Matrix linearMultiplication(const Matrix& other) noexcept
	{
		if (m_matrix == other.m_matrix
			|| m_y != other.m_x
			|| m_x != other.m_y)
		{
			return *this;
		}

		Matrix result(std::vector<int32_t>(other.m_y * m_x, 0), m_x, other.m_y);

		int32_t res = 0;
		uint8_t count = 0;
		uint8_t otherCount = 0;
		for (int i = 0; i < result.m_matrix.size(); ++i)
		{
			if (i % result.m_x == 0 && i != 0)
			{
				count++; otherCount = 0;
			}
			uint8_t cur = count * m_x;
			res += m_matrix[cur] * other.m_matrix[otherCount];
			res += m_matrix[cur + 1] * other.m_matrix[otherCount + m_x];
			result.m_matrix[i] = res;
			otherCount++;
			res = 0;
		}

		return result;
	}

	int32_t* dataPointer()
	{
		return m_matrix.data();
	}

	uint8_t x()
	{
		return m_x;
	}

	uint8_t y()
	{
		return m_x;
	}
private:
	uint8_t m_x;
	uint8_t m_y;
	std::vector<int32_t> m_matrix;
};
}
