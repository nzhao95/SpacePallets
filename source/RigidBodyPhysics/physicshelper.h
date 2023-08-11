#pragma once

#include <cmath>
#include <iostream>
#include <algorithm>
#include <chrono>

namespace rigidbody
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4201)
#endif

using f = double;

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

    f3() : v{ 0.0, 0.0, 0.0 } {}
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
        if (scal == 0.0)
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

    friend f3 operator*(f scal, f3 v)
    {
        return f3(scal * v.x, scal * v.y, scal * v.z);
    }

	f norm() const
	{
		return std::sqrt(x * x + y * y + z * z);
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
            for (int j = 0; j < 3; ++j)
                result[i][j] = dot(m[i], f3(rhs[0][j], rhs[1][j], rhs[2][j]));
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
            result.m[i][i] = 1.0;
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
                result.m[i][j] = 0.0;
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

struct quat {
    f x, y, z, w;
    // Constructors
    quat() : w(1.0), x(0.0), y(0.0), z(0.0) {}
    quat(f w, f x, f y, f z) : w(w), x(x), y(y), z(z) {}
    quat(f w, f3 v) : w(w), x(v.x), y(v.y), z(v.z) {}

    f norm() const {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }

    quat conjugate() const {
        return quat(w, -x, -y, -z);
    }

    void normalize() {
        const f invNorm = 1.0 / this->norm();
        w = w * invNorm;
        x = x * invNorm;
        y = y * invNorm;
        z = z * invNorm;
    }

    quat normalized() const {
        quat result;
        const f invNorm = 1.0 / this->norm();
        return quat(w * invNorm, x * invNorm, y * invNorm, z * invNorm);
    }

    void operator+=(const quat& rhs) {
        w += rhs.w;
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
    }

    quat operator+(const quat& rhs) const {
        return quat(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
    }

    void operator-=(const quat& rhs) {
        w -= rhs.w;
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
    }

    quat operator-(const quat& rhs) const {
        return quat(w - rhs.w, x - rhs.x, y - rhs.y, z - rhs.z);
    }

    void operator*=(const quat& rhs) {
        w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
        x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
        y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
        z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
    }

    quat operator*(const quat& rhs) const {
        return quat(
            w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
            w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w
        );
    }

    quat operator*(f scalar) const {
        return quat(w * scalar, x * scalar, y * scalar, z * scalar);
    }

    f3 rotate(f3 vec) const
    {    // Extract the vector part of the quaternion
        f3 u(x, y, z);
        return 2.0 * dot(u, vec) * u
            + (w * w - dot(u, u)) * vec
            + 2.0 * w *cross(u, vec);
    }

    f3x3 rotate(f3x3 mat) const
    {
        return f3x3{ this->rotate(mat[0]), this->rotate(mat[1]), this->rotate(mat[2]) };
    }

    friend quat operator*(f scalar, const quat& quat) {
        return quat * scalar;
    };

    friend std::ostream& operator<<(std::ostream& os, const quat& quat);

    bool operator==(const quat& rhs) const {
        return (w == rhs.w && x == rhs.x && y == rhs.y && z == rhs.z);
    }

    void applyRotationStep(const f3& angularVelocity, f dt) {
		const f angle = angularVelocity.norm();
		const f3 axis = angularVelocity / angle;
        quat& orientation = *this;
		const f theta = angle * dt * 0.5f;

        orientation = (quat(cos(theta), 0.0, 0.0, 0.0) + quat(0.0, sin(theta) * axis)) * orientation;
        orientation.normalize();
    }
};

inline std::ostream& operator<<(std::ostream& os, const quat& quat) {
    os << quat.w << " + " << quat.x << "i + " << quat.y << "j + " << quat.z << "k";
    return os;
}

// Function to convert a quaternion to the final matrix
inline f3x3 quaternionToMatrix(const quat& q) {
    f3x3 matrix
    {
        f3(1.0f - 2.0f * (q.y * q.y + q.z * q.z), 2.0f * (q.x * q.y - q.z * q.w), 2.0f * (q.x * q.z + q.y * q.w)),
        f3(2.0f * (q.x * q.y + q.z * q.w), 1.0f - 2.0f * (q.x * q.x + q.z * q.z), 2.0f * (q.y * q.z - q.x * q.w)),
        f3(2.0f * (q.x * q.z - q.y * q.w), 2.0f * (q.y * q.z + q.x * q.w), 1.0f - 2.0f * (q.x * q.x + q.y * q.y))
    };
    return matrix;
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

	f3x3 ComputeInertiaTensor() const
	{
		const f mass = this->mass();
		return f3x3{ f3((pow(lengths[1], 2) + pow(lengths[2], 2))* mass / 12.0 , 0.0, 0.0),
			f3(0.0, (pow(lengths[0], 2) + pow(lengths[2], 2))* mass / 12.0 , 0.0),
			f3(0.0, 0.0, (pow(lengths[0], 2) + pow(lengths[1], 2))* mass / 12.0) };
	}

	f3x3 ComputeInvInertiaTensor() const
	{
		const f mass = this->mass();
		if (mass <= 0.0)
		{
			throw std::runtime_error("Null density is not allowed!");
		}

		return f3x3{ f3(12.0 / ((pow(lengths[1], 2) + pow(lengths[2], 2))* mass) , 0.0, 0.0),
			f3(0.0, 12.0 / ((pow(lengths[0], 2) + pow(lengths[2], 2))* mass) , 0.0),
			f3(0.0, 0.0, 12.0 / ((pow(lengths[0], 2) + pow(lengths[1], 2))* mass)) };
	}

    f3 ComputeInitialAngularVelocity(const f3x3& invInertiaTensor) const
    {
        const f mass = this->mass();
        if (mass <= 0.0)
        {
            throw std::runtime_error("Null density is not allowed!");
        }
        
        f3 torque = cross(initial_impulse_application_point, initial_impulse);

        return invInertiaTensor * torque;
    }

    f3 ComputeAngularAcceleration(const f3& eulerMotionVector, const f3& w) const
	{
        return f3{  eulerMotionVector[0] * w[1] * w[2],
                    eulerMotionVector[1] * w[0] * w[2],
                    eulerMotionVector[2] * w[0] * w[1] };
	}

    f3 UpdateAngularVelocity(const f3& eulerMotionVector, const f3& w, const f dt) const
    {
        const f3 k1 = ComputeAngularAcceleration(eulerMotionVector, w);
        const f3 k2 = ComputeAngularAcceleration(eulerMotionVector, w + 0.5 * dt * k1);
        const f3 k3 = ComputeAngularAcceleration(eulerMotionVector, w + 0.5 * dt * k2);
        const f3 k4 = ComputeAngularAcceleration(eulerMotionVector, w + dt * k3);

        return w + (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
    }
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif
} // namespace rigidbody