/**********************************************************************************************
*
*   raylib-extras-cpp * Utilities and Shared Components for Raylib
*
*   math_3d * Math tools for use with raylib
*
*   LICENSE: MIT
*
*   Copyright (c) 2022 Jeffery Myers
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/

#pragma once

#include "raylib.h"
#include "raymath.h"

// A representation of an orthonormal transform
struct Transform3D 
{
    Vector3 Position = { 0,0,0 };
    Quaternion Orientation = QuaternionIdentity();
    Vector3 Scale = { 1,1,1 };
};

// Vector4 lenght functions
inline float Vector4LengthSq(const Vector4 &vec)
{
    return (vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

inline float Vector4Lenght(const Vector4& vec)
{
    return sqrtf(Vector4LengthSq(vec));
}

// Vector4 normailzie
inline void Vector4Normalize(Vector4& vec)
{
    float len = Vector4Lenght(vec);
    if (len == 0)
        return;

    vec.x /= len;
    vec.y /= len;
    vec.z /= len;
    vec.w /= len;
}

// Vector4 scale
inline void Vector4Scale(Vector4& vec, float scale)
{
    vec.x *= scale;
    vec.y *= scale;
    vec.z *= scale;
    vec.w *= scale;
}

// Construct a Vector4 from 3 components
inline Vector4 MakeVec4(float x, float y, float z)
{
    return Vector4{ x,y,z,0 };
}

// union wrapper around a matrix, allows easier access to rows, and float values
struct Matrix4x4
{
    union
    {
        float M[4][4];
        float M16[16];
        struct
        {
            Vector4 right, up, dir, position;
        } V;
        Vector4 Component[4];
        Matrix Mat;
    };

    Matrix4x4() { Mat = MatrixIdentity(); }

    inline void OrthoNormalize()
    {
        Vector4Normalize(V.right);
        Vector4Normalize(V.up);
        Vector4Normalize(V.dir);
    }

    void RotationAxis(const Vector4& axis, float angle);
};

// math uitls
namespace Math3D
{
	// Convert a transform to a matrix
    Matrix4x4 TransformToMatrix(const Transform3D& transform);
	
	// Convert a matrix to a transform
    Transform3D MatrixToTransform(const Matrix4x4& matrix);

	// collision between a transfomred bounding box and a ray
    RayCollision GetRayCollisionBoxOriented(Ray ray, BoundingBox box, Transform3D& transform);
    RayCollision GetRayCollisionBoxOriented(Ray ray, BoundingBox box, Matrix4x4& transform);
    RayCollision GetRayCollisionBoxOriented(Ray ray, BoundingBox box, Matrix& transform);

	// scales a bounding box by a non uniform scale
    BoundingBox ScaleBoundingBox(BoundingBox& box, Vector3& scale);

	// utils to get components of a bounding box
    Vector3 GetBoundingBoxCenter(const BoundingBox& box);
    Vector3 GetBoundingBoxSize(const BoundingBox& box);
}