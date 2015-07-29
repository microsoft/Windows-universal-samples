//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

// This header defines math and matrix helper functions and structures used 
// by DirectX SDK samples.

// Common Constants

#define PI_F 3.1415927f

// Template Vector & Matrix Classes

template <class T> struct Vector2
{
    union
    {
        struct
        {
            T x;
            T y;
        };
        struct
        {
            T r;
            T g;
        };
        struct
        {
            T u;
            T v;
        };
    };

    T& operator[](unsigned int index)
    {
        return static_cast<T*>(this)[index];
    }

    Vector2(T _x = 0, T _y = 0) : x(_x), y(_y) { }
};

template <class T> struct Vector3
{
    union
    {
        struct
        {
            T x;
            T y;
            T z;
        };
        struct
        {
            T r;
            T g;
            T b;
        };
        struct
        {
            T u;
            T v;
            T w;
        };
    };

    T& operator[](unsigned int index)
    {
        return static_cast<T*>(this)[index];
    }

    Vector3(T _x = 0, T _y = 0, T _z = 0) : x(_x), y(_y), z(_z) { }
};

template <class T> struct Vector4
{
    union
    {
        struct
        {
            T x;
            T y;
            T z;
            T w;
        };
        struct
        {
            T r;
            T g;
            T b;
            T a;
        };
    };

    T& operator[](unsigned int index)
    {
        return static_cast<T*>(this)[index];
    }

    Vector4(T _x = 0, T _y = 0, T _z = 0, T _w = 0) : x(_x), y(_y), z(_z), w(_w) { }
};

template <class T> struct Matrix4x4
{
    union
    {
        struct
        {
            T _11; T _12; T _13; T _14;
            T _21; T _22; T _23; T _24;
            T _31; T _32; T _33; T _34;
            T _41; T _42; T _43; T _44;
        };
        struct
        {
            T _m00; T _m01; T _m02; T _m03;
            T _m10; T _m11; T _m12; T _m13;
            T _m20; T _m21; T _m22; T _m23;
            T _m30; T _m31; T _m32; T _m33;
        };
    };

    Matrix4x4(T value = 0)
    {
        _11 = _12 = _13 = _14 = value;
        _21 = _22 = _23 = _24 = value;
        _31 = _32 = _33 = _34 = value;
        _41 = _42 = _43 = _44 = value;
    }

    Matrix4x4(
        T i11, T i12, T i13, T i14,
        T i21, T i22, T i23, T i24,
        T i31, T i32, T i33, T i34,
        T i41, T i42, T i43, T i44
        )
    {
        _11 = i11; _12 = i12; _13 = i13; _14 = i14;
        _21 = i21; _22 = i22; _23 = i23; _24 = i24;
        _31 = i31; _32 = i32; _33 = i33; _34 = i34;
        _41 = i41; _42 = i42; _43 = i43; _44 = i44;
    }

    T* operator[](unsigned int index)
    {
        return &(reinterpret_cast<T*>(this)[index*4]);
    }
};

// Template Vector Operations

template <class T>
T dot(Vector2<T> a, Vector2<T> b)
{
    return a.x * b.x + a.y * b.y;
}

template <class T>
T dot(Vector3<T> a, Vector3<T> b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <class T>
T dot(Vector4<T> a, Vector4<T> b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w + b.w;
}

template <class T>
T length(Vector2<T> a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

template <class T>
T length(Vector3<T> a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

template <class T>
T length(Vector4<T> a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
}

template <class T>
Vector3<T> cross(Vector3<T> a, Vector3<T> b)
{
    return Vector3<T>((a.y*b.z)-(a.z*b.y), (a.z*b.x)-(a.x*b.z), (a.x*b.y)-(a.y*b.x));
}

template <class T>
Vector2<T> normalize(Vector2<T> a)
{
    T len = length(a);
    return Vector2<T>(a.x / len, a.y / len);
}

template <class T>
Vector3<T> normalize(Vector3<T> a)
{
    T len = length(a);
    return Vector3<T>(a.x / len, a.y / len, a.z / len);
}

template <class T>
Vector4<T> normalize(Vector4<T> a)
{
    T len = length(a);
    return Vector4<T>(a.x / len, a.y / len, a.z / len, a.w / len);
}

// Template Vector Operators

template <class T>
Vector2<T> operator-(Vector2<T> a, Vector2<T> b)
{
    return Vector2<T>(a.x - b.x, a.y - b.y);
}

template <class T>
Vector2<T> operator-(Vector2<T> a)
{
    return Vector2<T>(-a.x, -a.y);
}

template <class T>
Vector3<T> operator-(Vector3<T> a, Vector3<T> b)
{
    return Vector3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
}

template <class T>
Vector3<T> operator-(Vector3<T> a)
{
    return Vector3<T>(-a.x, -a.y, -a.z);
}

template <class T>
Vector4<T> operator-(Vector4<T> a, Vector4<T> b)
{
    return Vector4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

template <class T>
Vector4<T> operator-(Vector4<T> a)
{
    return Vector4<T>(-a.x, -a.y, -a.z, -a.w);
}

template <class T>
Vector2<T> operator+(Vector2<T> a, Vector2<T> b)
{
    return Vector2<T>(a.x + b.x, a.y + b.y);
}

template <class T>
Vector3<T> operator+(Vector3<T> a, Vector3<T> b)
{
    return Vector3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
}

template <class T>
Vector4<T> operator+(Vector4<T> a, Vector4<T> b)
{
    return Vector4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

template <class T>
Vector2<T> operator*(Vector2<T> a, T s)
{
    return Vector2<T>(a.x * s, a.y * s);
}

template <class T>
Vector2<T> operator*(T s, Vector2<T> a)
{
    return a * s;
}

template <class T>
Vector2<T> operator*(Vector2<T> a, Vector2<T> b)
{
    return Vector2<T>(a.x * b.x, a.y * b.y);
}

template <class T>
Vector2<T> operator/(Vector2<T> a, T s)
{
    return Vector2<T>(a.x / s, a.y / s);
}

template <class T>
Vector3<T> operator*(Vector3<T> a, T s)
{
    return Vector3<T>(a.x * s, a.y * s, a.z * s);
}

template <class T>
Vector3<T> operator*(T s, Vector3<T> a)
{
    return a * s;
}

template <class T>
Vector3<T> operator*(Vector3<T> a, Vector3<T> b)
{
    return Vector3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
}

template <class T>
Vector3<T> operator/(Vector3<T> a, T s)
{
    return Vector3<T>(a.x / s, a.y / s, a.z / s);
}

template <class T>
Vector4<T> operator*(Vector4<T> a, T s)
{
    return Vector4<T>(a.x * s, a.y * s, a.z * s, a.w * s);
}

template <class T>
Vector4<T> operator*(T s, Vector4<T> a)
{
    return a * s;
}

template <class T>
Vector4<T> operator*(Vector4<T> a, Vector4<T> b)
{
    return Vector4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

template <class T>
Vector4<T> operator/(Vector4<T> a, T s)
{
    return Vector4<T>(a.x / s, a.y / s, a.z / s, a.w / s);
}



// Template Matrix Operations

template <class T>
Matrix4x4<T> transpose(Matrix4x4<T> m)
{
    return Matrix4x4<T>(
        m._11, m._21, m._31, m._41,
        m_.12, m._22, m._32, m._42,
        m._13, m._23, m._33, m._43,
        m._14, m._24, m._34, m._44
        );
}

template <class T>
Matrix4x4<T> mul(Matrix4x4<T> m1, Matrix4x4<T> m2)
{
    Matrix4x4<T> mOut;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                mOut[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }

    return mOut;
}

// Common HLSL-compatible vector typedefs

typedef unsigned int uint;

typedef Vector2<float> float2;
typedef Vector3<float> float3;
typedef Vector4<float> float4;

typedef Matrix4x4<float> float4x4;

// Standard Matrix Intializers

inline float4x4 identity()
{
    float4x4 mOut;

    mOut._11 = 1.0f; mOut._12 = 0.0f; mOut._13 = 0.0f; mOut._14 = 0.0f;
    mOut._21 = 0.0f; mOut._22 = 1.0f; mOut._23 = 0.0f; mOut._24 = 0.0f;
    mOut._31 = 0.0f; mOut._32 = 0.0f; mOut._33 = 1.0f; mOut._34 = 0.0f;
    mOut._41 = 0.0f; mOut._42 = 0.0f; mOut._43 = 0.0f; mOut._44 = 1.0f;

    return mOut;
}

inline float4x4 translation(float x, float y, float z)
{
    float4x4 mOut;

    mOut._11 = 1.0f; mOut._12 = 0.0f; mOut._13 = 0.0f; mOut._14 = x;
    mOut._21 = 0.0f; mOut._22 = 1.0f; mOut._23 = 0.0f; mOut._24 = y;
    mOut._31 = 0.0f; mOut._32 = 0.0f; mOut._33 = 1.0f; mOut._34 = z;
    mOut._41 = 0.0f; mOut._42 = 0.0f; mOut._43 = 0.0f; mOut._44 = 1.0f;

    return mOut;
}

inline float4x4 scale(float x, float y, float z)
{
    float4x4 mOut;

    mOut._11 = x;    mOut._12 = 0.0f; mOut._13 = 0.0f; mOut._14 = 0.0f;
    mOut._21 = 0.0f; mOut._22 = y;    mOut._23 = 0.0f; mOut._24 = 0.0f;
    mOut._31 = 0.0f; mOut._32 = 0.0f; mOut._33 = z;    mOut._34 = 0.0f;
    mOut._41 = 0.0f; mOut._42 = 0.0f; mOut._43 = 0.0f; mOut._44 = 1.0f;

    return mOut;
}

inline float4x4 rotationX(float degreeX)
{
    float angleInRadians = degreeX * (PI_F / 180.0f);

    float sinAngle = sinf(angleInRadians);
    float cosAngle = cosf(angleInRadians);

    float4x4 mOut;

    mOut._11 = 1.0f; mOut._12 = 0.0f;     mOut._13 = 0.0f;      mOut._14 = 0.0f;
    mOut._21 = 0.0f; mOut._22 = cosAngle; mOut._23 = -sinAngle; mOut._24 = 0.0f;
    mOut._31 = 0.0f; mOut._32 = sinAngle; mOut._33 = cosAngle;  mOut._34 = 0.0f;
    mOut._41 = 0.0f; mOut._42 = 0.0f;     mOut._43 = 0.0f;      mOut._44 = 1.0f;

    return mOut;
}

inline float4x4 rotationY(float degreeY)
{
    float angleInRadians = degreeY * (PI_F / 180.0f);

    float sinAngle = sinf(angleInRadians);
    float cosAngle = cosf(angleInRadians);

    float4x4 mOut;

    mOut._11 = cosAngle;  mOut._12 = 0.0f; mOut._13 = sinAngle; mOut._14 = 0.0f;
    mOut._21 = 0.0f;      mOut._22 = 1.0f; mOut._23 = 0.0f;     mOut._24 = 0.0f;
    mOut._31 = -sinAngle; mOut._32 = 0.0f; mOut._33 = cosAngle; mOut._34 = 0.0f;
    mOut._41 = 0.0f;      mOut._42 = 0.0f; mOut._43 = 0.0f;     mOut._44 = 1.0f;

    return mOut;
}

inline float4x4 rotationZ(float degreeZ)
{
    float angleInRadians = degreeZ * (PI_F / 180.0f);

    float sinAngle = sinf(angleInRadians);
    float cosAngle = cosf(angleInRadians);

    float4x4 mOut;

    mOut._11 = cosAngle; mOut._12 = -sinAngle; mOut._13 = 0.0f; mOut._14 = 0.0f;
    mOut._21 = sinAngle; mOut._22 = cosAngle;  mOut._23 = 0.0f; mOut._24 = 0.0f;
    mOut._31 = 0.0f;     mOut._32 = 0.0f;      mOut._33 = 1.0f; mOut._34 = 0.0f;
    mOut._41 = 0.0f;     mOut._42 = 0.0f;      mOut._43 = 0.0f; mOut._44 = 1.0f;

    return mOut;
}

// 3D Rotation matrix for an arbitrary axis specified by x, y and z
inline float4x4 rotationArbitrary(float3 axis, float degree)
{
    axis = normalize(axis);

    float angleInRadians = degree * (PI_F / 180.0f);

    float sinAngle = sinf(angleInRadians);
    float cosAngle = cosf(angleInRadians);
    float oneMinusCosAngle = 1 - cosAngle;

    float4x4 mOut;

    mOut._11 = 1.0f + oneMinusCosAngle * (axis.x * axis.x - 1.0f);
    mOut._12 = axis.z * sinAngle + oneMinusCosAngle * axis.x * axis.y;
    mOut._13 = -axis.y * sinAngle + oneMinusCosAngle * axis.x * axis.z;
    mOut._41 = 0.0f;

    mOut._21 = -axis.z * sinAngle + oneMinusCosAngle * axis.y * axis.x;
    mOut._22 = 1.0f + oneMinusCosAngle * (axis.y * axis.y - 1.0f);
    mOut._23 = axis.x * sinAngle + oneMinusCosAngle * axis.y * axis.z;
    mOut._24 = 0.0f;

    mOut._31 = axis.y * sinAngle + oneMinusCosAngle * axis.z * axis.x;
    mOut._32 = -axis.x * sinAngle + oneMinusCosAngle * axis.z * axis.y;
    mOut._33 = 1.0f + oneMinusCosAngle * (axis.z * axis.z - 1.0f);
    mOut._34 = 0.0f;

    mOut._41 = 0.0f;
    mOut._42 = 0.0f;
    mOut._43 = 0.0f;
    mOut._44 = 1.0f;

    return mOut;
}
