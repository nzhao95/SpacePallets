#pragma once

#include <cmath>
#include <iostream>
#include <algorithm>

namespace rigidbody
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4201)
#endif

using f = float;

struct f3
{
    union {
        struct {
            f x;
            f y;
            f z;
        };
        f v[3];
    };

    f3() : v{ 0.f, 0.f, 0.f } {}
    f3(f x, f y, f z) : v{ x, y, z } {}
    f3(f const v[3]) : v{ v[0], v[1], v[2] } {}

    f3 operator+(f3 const& rhs) const
    {
        return f3(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    f3 operator-(f3 const& rhs) const
    {
        return f3(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    f3 operator*(f3 const& rhs) const
    {
        return f3(x * rhs.x, y * rhs.y, z * rhs.z);
    }
    bool operator==(const f3 rhs)
    {
        return v[0] == rhs[0] && v[1] == rhs[1] && v[2] == rhs[2];
    }
    f3 operator*(f scal) const
    {
        return f3(scal * x, scal * y, scal * z);
    }
    f3 operator/(f scal) const
    {
        if (scal == 0.f)
        {
            throw std::runtime_error("Division by zero");
        }
        return f3(x / scal, y / scal, z / scal);
    }
    f3 operator-() const
    {
        return f3(-x, -y, -z);
    }
    f operator[](int const a) const
    {
        return v[a];
    }
    f& operator[](int const a)
    {
        return v[a];
    }

	f norm() const
	{
		return std::sqrtf(x * x + y * y + z * z);
	}

    f3 normalized() const
    {
        f norm = this->norm();
        return norm == 0 ? v : *this / norm;
    }

    void normalize()
    {
        *this = *this/this->norm();
    }
};

// Stream insertion for printing
inline std::ostream& operator<<(std::ostream& os, const f3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

inline f3 operator*(f scal, f3 v)
{
    return f3(scal * v.x, scal * v.y, scal * v.z);
}

inline f dot(f3 a, f3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline f3 cross(f3 a, f3 b)
{
    return f3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

struct f3x3
{
    f3 m[3];

    f3x3() { f3(), f3(), f3(); }

    f3x3(f3 row0, f3 row1, f3 row2)
    {
        m[0][0] = row0[0];
        m[0][1] = row0[1];
        m[0][2] = row0[2];

        m[1][0] = row1[0];
        m[1][1] = row1[1];
        m[1][2] = row1[2];

        m[2][0] = row2[0];
        m[2][1] = row2[1];
        m[2][2] = row2[2];
    }

    f3x3 transpose() const
    {
        f3x3 result;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                result.m[i][j] = m[j][i];
        return result;
    }

    f3x3 operator*(f const& scal) const
    {
        return f3x3(m[0] * scal, m[1] * scal, m[2] * scal);
    }

    f3x3 operator*(f3x3 const& rhs) const
    {
        f3x3 result;
        for (int i = 0; i < 3; ++i)
            result[i] = m[i] * f3(rhs[0][i], rhs[1][i], rhs[2][i]);
        return result;
    }

    bool operator==(const f3x3 rhs)
    {
        return m[0] == rhs[0] && m[1] == rhs[1] && m[2] == rhs[2];
    }

    f3 operator*(f3 const& rhs) const
    {
        f3 result;
        for (int i = 0; i < 3; ++i)
        {
            result[i] = dot(m[i], rhs);
        }
        return result;
    }

    static f3x3 id()
    {
        f3x3 result;
        for (int i = 0; i < 3; ++i)
            result.m[i][i] = 1.f;
        return result;
    }

    static f3x3 diagonal(f3 d)
    {
        f3x3 result;
        for (int i = 0; i < 3; ++i)
            result.m[i][i] = d.v[i];
        return result;
    }

    static f3x3 zero()
    {
        f3x3 result;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                result.m[i][j] = 0.f;
        return result;
    }

    f3x3 operator+(f3x3 const& rhs) const
    {
        f3x3 result;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                result.m[i][j] = m[i][j] + rhs.m[i][j];
        return result;
    }

    f3x3 operator-(f3x3 const& rhs) const
    {
        f3x3 result;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                result.m[i][j] = m[i][j] - rhs.m[i][j];
        return result;
    }

    f3 operator[](int const a) const
    {
        return m[a];
    }

    f3& operator[](int const a)
    {
        return m[a];
    }

    f trace() const
    {
        return m[0][0] + m[1][1] + m[2][2];
    }
};

inline std::ostream& operator<<(std::ostream& os, const f3x3& m) {
    os << "[" << m[0] << ",\n" << m[1] << ",\n" << m[2] << "]";
    return os;
}

inline f3x3 operator*(f scal, f3x3 m)
{
    return m*scal;
};

inline f3x3 matrixFromAxisAngle(f angle, f3 axis) {

	const f cosinus = cos(angle);
	const f sinus = sin(angle);
	const f t = 1.f - cosinus;

	f3x3 mat;
	mat[0][0] = cosinus + axis[0]*axis[0]*t;
	mat[1][1] = cosinus + axis[1]*axis[1]*t;
	mat[2][2] = cosinus + axis[2]*axis[2]*t;


	const f v0 = axis[0]*axis[1]*t;
	const f v1 = axis[2]*sinus;
	const f v2 = axis[0] * axis[2] * t;
	const f v3 = axis[1] * sinus;
	const f v4 = axis[1] * axis[2] * t;
	const f v5 = axis[0] * sinus;

	mat[1][0] = v0 + v1;
	mat[0][1] = v0 - v1;
	mat[2][0] = v2 - v3;
	mat[0][2] = v2 + v3;    
	mat[2][1] = v4 + v5;
	mat[1][2] = v4 - v5;
	return mat;
}

struct SimulationContext
{
    f density{};                            // Density of the rectangular parallelepiped. The density is uniform.
    f3 lengths{};                           // Lengths of the three axes of the rectangular parallelepiped
    f3 initial_impulse{};                   // Initial impulse value
    f3 initial_impulse_application_point{}; // Point on the body where the impulse is applied

    f final_time;                           // Final time of the simulation

    f mass() const
    {
        return lengths[0] * lengths[1] * lengths[2] * density;
    }

    f3 ComputeInitialAngularVelocity() const
    {
        const f mass = this->mass();
        if (mass <= 0.f)
        {
            throw std::runtime_error("Null density is not allowed!");
        }

        f3x3 invI{ f3(12.f / (powf(lengths[1], 2) + powf(lengths[2], 2)* mass) , 0.f, 0.f),
                   f3(0.f, 12.f / (powf(lengths[0], 2) + powf(lengths[2], 2)* mass) , 0.f),
                   f3(0.f, 0.f, 12.f / (powf(lengths[0], 2) + powf(lengths[1], 2)* mass) ) };
        
        f3 torque = cross(initial_impulse_application_point, initial_impulse);

        f3 angularVelocity = invI * torque ;

        return angularVelocity;
    }
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif
} // namespace rigidbody