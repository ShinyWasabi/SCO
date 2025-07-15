#pragma once

namespace rage
{
#pragma pack(push, 0x10)
	class alignas(16) vector3
	{
	public:
		float x, y, z;

	public:
		constexpr vector3(float x, float y, float z) :
			x(x),
			y(y),
			z(z)
		{
		}

		constexpr vector3() :
			x(),
			y(),
			z()
		{
		}

		bool operator==(const vector3& other) const
		{
			return this->x == other.x && this->y == other.y && this->z == other.z;
		}

		template<typename T>
		bool operator!=(const vector3& other) const
		{
			return this->x != other.x || this->y != other.y || this->z != other.z;
		}

		vector3 operator+(const vector3& other) const
		{
			vector3 vec;
			vec.x = this->x + other.x;
			vec.y = this->y + other.y;
			vec.z = this->z + other.z;
			return vec;
		}

		vector3 operator-(const vector3& other) const
		{
			vector3 vec;
			vec.x = this->x - other.x;
			vec.y = this->y - other.y;
			vec.z = this->z - other.z;
			return vec;
		}

		vector3 operator*(const vector3& other) const
		{
			vector3 vec;
			vec.x = this->x * other.x;
			vec.y = this->y * other.y;
			vec.z = this->z * other.z;
			return vec;
		}

		vector3 operator*(const float& other) const
		{
			vector3 vec;
			vec.x = this->x * other;
			vec.y = this->y * other;
			vec.z = this->z * other;
			return vec;
		}

		inline float GetMagnitude() const
		{
			return sqrt(x * x + y * y + z * z);
		}

		inline float GetDistance(const vector3& other) const
		{
			return (*this - other).GetMagnitude();
		}
	};
	static_assert(sizeof(rage::vector3) == 0x10);
#pragma pack(pop)

	class scrVector
	{
	public:
		scrVector() = default;

		scrVector(rage::vector3 vec) :
		    x(vec.x),
		    y(vec.y),
		    z(vec.z)
		{
		}

		scrVector(float x, float y, float z) :
		    x(x),
		    y(y),
		    z(z)
		{
		}

		scrVector operator+(const scrVector& other)
		{
			scrVector vec;
			vec.x = this->x + other.x;
			vec.y = this->y + other.y;
			vec.z = this->z + other.z;
			return vec;
		}

		scrVector operator-(const scrVector& other)
		{
			scrVector vec;
			vec.x = this->x - other.x;
			vec.y = this->y - other.y;
			vec.z = this->z - other.z;
			return vec;
		}

		scrVector operator*(const scrVector& other)
		{
			scrVector vec;
			vec.x = this->x * other.x;
			vec.y = this->y * other.y;
			vec.z = this->z * other.z;
			return vec;
		}

		scrVector operator*(const float& other)
		{
			scrVector vec;
			vec.x = this->x * other;
			vec.y = this->y * other;
			vec.z = this->z * other;
			return vec;
		}

		bool operator==(const scrVector& other)
		{
			return this->x == other.x && this->y == other.y && this->z == other.z;
		}

		bool operator!=(const scrVector& other)
		{
			return this->x != other.x || this->y != other.y || this->z != other.z;
		}

		operator rage::vector3()
		{
			return {x, y, z};
		}

		operator bool()
		{
			return this->x != 0.0f || this->y != 0.0f || this->z != 0.0f;
		}

		alignas(8) float x{};
		alignas(8) float y{};
		alignas(8) float z{};
	};
}