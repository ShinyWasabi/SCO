#pragma once
#include "Vector3.hpp"

namespace rage
{
	class scrVector
	{
	public:
		scrVector() = default;

		scrVector(rage::Vector3 vec) :
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

		operator rage::Vector3()
		{
			return { x, y, z };
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