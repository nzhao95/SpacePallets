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

    f det() const {
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
            m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
            m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }

    // Invert a 3x3 matrix
    bool inverse(f3x3& invertedMatrix) {
        f det = this->det();

        // Check if the matrix is invertible
        if (det == 0.0) {
            return false;
        }

        f invDet = 1.0 / det;

        // Compute the inverse matrix using the adjugate and the determinant
        invertedMatrix[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
        invertedMatrix[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]) * invDet;
        invertedMatrix[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;

        invertedMatrix[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]) * invDet;
        invertedMatrix[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
        invertedMatrix[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]) * invDet;

        invertedMatrix[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet;
        invertedMatrix[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]) * invDet;
        invertedMatrix[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet;
        return true;
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

inline f3x3 velocityTensor(const f3& w)
{
    return f3x3{ f3(0.f,  -w[2],  w[1]),
                 f3(w[2],  0.f , -w[0]),
                 f3(-w[1], w[0],  0.f)};
}

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


struct quat {
    f x, y, z, w;
    // Constructors
    quat() : w(1.f), x(0.f), y(0.f), z(0.f) {}
    quat(f w, f x, f y, f z) : w(w), x(x), y(y), z(z) {}
    quat(f w, f3 v) : w(w), x(v.x), y(v.y), z(v.z) {}

    f norm() const {
        return std::sqrtf(w * w + x * x + y * y + z * z);
    }

    quat conjugate() const {
        return quat(w, -x, -y, -z);
    }

    void normalize() {
        const f invNorm = 1.f / this->norm();
        w = w * invNorm;
        x = x * invNorm;
        y = y * invNorm;
        z = z * invNorm;
    }

    quat normalized() const {
        quat result;
        const f invNorm = 1.f / this->norm();
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

    friend quat operator*(f scalar, const quat& quat) {
        return quat * scalar;
    };

    friend std::ostream& operator<<(std::ostream& os, const quat& quat);

    bool operator==(const quat& rhs) const {
        return (w == rhs.w && x == rhs.x && y == rhs.y && z == rhs.z);
    }

    static quat computeOrientationDerivative(const quat& q, const f3& angularVelocity) {
        f halfAngle = angularVelocity.norm() * 0.5f;
        f3 rotationAxis = angularVelocity.normalized();
        quat rotationQuaternion(cos(halfAngle), rotationAxis.x * sin(halfAngle),
            rotationAxis.y * sin(halfAngle),
            rotationAxis.z * sin(halfAngle));
        return 0.5f * rotationQuaternion * q;
    }

    static quat quatIntegrationFirstOrder(const quat& q, const quat& angularVelocity, f dt) {
        return q + angularVelocity * 0.5f * dt * q;
    }

    quat quaternionPow(f exponent) const {
        double angle = std::acos(w);
        double newAngle = angle * exponent;
        double scale = std::sin(newAngle) / std::sin(angle);

        return quat(std::cos(newAngle), x * scale, y * scale, z * scale);
    }

    void applyRotationStep(const f3& angularVelocity, f dt) {
        quat& orientation = *this;
        quat k1 = computeOrientationDerivative(orientation, angularVelocity);
        quat k2 = computeOrientationDerivative(orientation + 0.5 * dt * k1, angularVelocity);
        quat k3 = computeOrientationDerivative(orientation + 0.5 * dt * k2, angularVelocity);
        quat k4 = computeOrientationDerivative(orientation + dt * k3, angularVelocity);

        orientation = orientation + (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        orientation.normalize();
    }
};

inline quat angleAxisToQuaternion(f angle, const f3& dir) {

    // Convert the angle to radians
    f theta = angle;

    // Calculate the quaternion components
    f cosHalfTheta = std::cos(theta / 2);
    f sinHalfTheta = std::sin(theta / 2);
    f qx = sinHalfTheta * dir[0];
    f qy = sinHalfTheta * dir[1];
    f qz = sinHalfTheta * dir[2];
    f qw = cosHalfTheta;

    return quat(qw, qx, qy, qz);
}

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

// Function to compute rotation axis from a unit quaternion
inline void quaternionToAxisAngle(const quat& q, f& angle, f3& axis) {
    // Ensure the quat is normalized
    quat normalized_q = q.normalized();

    // Calculate the rotation angle
    angle = 2 * std::acos(normalized_q.w);

    // Compute the rotation axis
    double sin_half_angle = std::sqrt(1.0 - normalized_q.w * normalized_q.w);
    if (sin_half_angle < 0.0001) {
        // Handle the case of small or zero angle to avoid division by zero
        axis.x = 1.0;
        axis.y = 0.0;
        axis.z = 0.0;
    }
    else {
        axis.x = normalized_q.x / sin_half_angle;
        axis.y = normalized_q.y / sin_half_angle;
        axis.z = normalized_q.z / sin_half_angle;
    }
}

// Function to convert a rotation matrix to 
inline quat matrixToQuaternion(const f3x3& matrix) {
    quat q;

    f trace = matrix.trace();
    f root;

    if (trace > 0.f) {
        root = std::sqrtf(trace + 1.f);
        q.w = 0.5f * root;
        root = 0.5f / root;
        q.x = (matrix[2][1] - matrix[1][2]) * root;
        q.y = (matrix[0][2] - matrix[2][0]) * root;
        q.z = (matrix[1][0] - matrix[0][1]) * root;
    }
    else {
        static size_t next[3] = { 1, 2, 0 };
        size_t i = 0;
        if (matrix[1][1] > matrix[0][0]) {
            i = 1;
        }
        if (matrix[2][2] > matrix[i][i]) {
            i = 2;
        }
        size_t j = next[i];
        size_t k = next[j];

        root = std::sqrtf(matrix[i][i] - matrix[j][j] - matrix[k][k] + 1.f);
        f* qArr[3] = { &q.x, &q.y, &q.z };
        *qArr[i] = 0.5f * root;
        root = 0.5f / root;
        q.w = (matrix[k][j] - matrix[j][k]) * root;
        *qArr[j] = (matrix[j][i] + matrix[i][j]) * root;
        *qArr[k] = (matrix[k][i] + matrix[i][k]) * root;
    }

    return q;
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

    f3 ComputeAngularVelocity(const f3x3& rotMat) const
    {
        const f mass = this->mass();
        if (mass <= 0.f)
        {
            throw std::runtime_error("Null density is not allowed!");
        }

        f3x3 I  { f3(powf(lengths[1], 2) + powf(lengths[2], 2) * mass / 12.f , 0.f, 0.f),
                   f3(0.f, powf(lengths[0], 2) + powf(lengths[2], 2) * mass / 12.f  , 0.f),
                   f3(0.f, 0.f, powf(lengths[0], 2) + powf(lengths[1], 2) * mass / 12.f) };
        I = rotMat * I;
        f3x3 invI;
        bool success = I.inverse(invI);

        f3 torque = cross(initial_impulse_application_point, initial_impulse);

        f3 angularVelocity = invI * torque;

        return angularVelocity;
    }
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif
} // namespace rigidbody