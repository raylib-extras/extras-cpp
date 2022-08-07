/**********************************************************************************************
*
*   raylib-extras-cpp * Utilities and Shared Components for Raylib
*
*   math_3d
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

#include "editor/utils/math_3d.h"

#include <math.h>
#include <limits>

void Matrix4x4::RotationAxis(const Vector4& axis, float angle)
{
    float length2 = Vector4LengthSq(axis);
    if (length2 < std::numeric_limits<float>::epsilon())
    {
        Mat = MatrixIdentity();
        return;
    }

    Vector4 n = axis;
    Vector4Scale(n, 1.f / sqrtf(length2));

    float s = sinf(angle);
    float c = cosf(angle);
    float k = 1.f - c;

    float xx = n.x * n.x * k + c;
    float yy = n.y * n.y * k + c;
    float zz = n.z * n.z * k + c;
    float xy = n.x * n.y * k;
    float yz = n.y * n.z * k;
    float zx = n.z * n.x * k;
    float xs = n.x * s;
    float ys = n.y * s;
    float zs = n.z * s;

    M[0][0] = xx;
    M[0][1] = xy + zs;
    M[0][2] = zx - ys;
    M[0][3] = 0.f;
    M[1][0] = xy - zs;
    M[1][1] = yy;
    M[1][2] = yz + xs;
    M[1][3] = 0.f;
    M[2][0] = zx + ys;
    M[2][1] = yz - xs;
    M[2][2] = zz;
    M[2][3] = 0.f;
    M[3][0] = 0.f;
    M[3][1] = 0.f;
    M[3][2] = 0.f;
    M[3][3] = 1.f;
}

namespace Math3D
{
    Vector3 GetBoundingBoxCenter(const BoundingBox& box)
    {
        return Vector3Add(box.min, Vector3Scale(Vector3Subtract(box.max, box.min), 0.5f));
    }

    Vector3 GetBoundingBoxSize(const BoundingBox& box)
    {
        return Vector3Subtract(box.max, box.min);
    }

    BoundingBox ScaleBoundingBox(BoundingBox& box, Vector3& scale)
    {
        Vector3 center = GetBoundingBoxCenter(box);
        Vector3 size = Vector3Multiply(Vector3Scale(GetBoundingBoxSize(box), 0.5f), scale);

        return BoundingBox{ { Vector3Subtract(center, Vector3Scale(size,-1)) } , { Vector3Subtract(center, size)} };
    }

    static const Vector4 directionUnary[3] = { MakeVec4(1.f, 0.f, 0.f), MakeVec4(0.f, 1.f, 0.f), MakeVec4(0.f, 0.f, 1.f) };

    Matrix4x4 TransformToMatrix(const Transform3D& transform)
    {
        Matrix4x4 mat;

        Vector3 rots = QuaternionToEuler(transform.Orientation);

        Matrix4x4 rot[3];
        for (int i = 0; i < 3; i++)
        {
            rot[i].RotationAxis(directionUnary[i], (&rots.x)[i]);
        }

        mat.Mat = MatrixMultiply(MatrixMultiply(rot[0].Mat, rot[1].Mat), rot[2].Mat);

        float validScale[3];
        for (int i = 0; i < 3; i++)
        {
            if (fabsf((&transform.Scale.x)[i]) < std::numeric_limits<float>::epsilon())
            {
                validScale[i] = 0.001f;
            }
            else
            {
                validScale[i] = (&transform.Scale.x)[i];
            }
        }
        Vector4Scale(mat.V.right, validScale[0]);
        Vector4Scale(mat.V.up, validScale[1]);
        Vector4Scale(mat.V.dir, validScale[2]);
        mat.V.position.x = transform.Position.x;
        mat.V.position.y = transform.Position.y;
        mat.V.position.z = transform.Position.z;

        return mat;
    }

    Transform3D MatrixToTransform(const Matrix4x4& matrix)
    {
        Transform3D transform = { 0 };

        Matrix4x4 mat = matrix;

        transform.Scale.x = Vector4Lenght(mat.V.right);
        transform.Scale.y = Vector4Lenght(mat.V.up);
        transform.Scale.z = Vector4Lenght(mat.V.dir);

        mat.OrthoNormalize();

        Vector3 eulerAngles;
        eulerAngles.x = atan2f(mat.M[1][2], mat.M[2][2]);
        eulerAngles.y = atan2f(-mat.M[0][2], sqrtf(mat.M[1][2] * mat.M[1][2] + mat.M[2][2] * mat.M[2][2]));
        eulerAngles.z = atan2f(mat.M[0][1], mat.M[0][0]);

        transform.Orientation = QuaternionFromEuler(eulerAngles.x, eulerAngles.y, eulerAngles.z);

        transform.Position.x = mat.V.position.x;
        transform.Position.y = mat.V.position.y;
        transform.Position.z = mat.V.position.z;

        return transform;
    }

    RayCollision GetRayCollisionBoxOriented(Ray ray, BoundingBox box, Transform3D& transform)
    {
        RayCollision hit = { 0 };

        BoundingBox bbox = ScaleBoundingBox(box, transform.Scale);

        // Intersection method from Real-Time Rendering and Essential Mathematics for Games
        Matrix rotMat = QuaternionToMatrix(transform.Orientation);

        Vector3 xAxis = Vector3Transform(Vector3{ 1,0,0 }, rotMat);
        Vector3 yAxis = Vector3Transform(Vector3{ 0,1,0 }, rotMat);
        Vector3 zAxis = Vector3Transform(Vector3{ 0,0,1 }, rotMat);

        float tMin = 0.0f;
        float tMax = 100000.0f;

        Vector3 worldPos = transform.Position;

        Vector3 delta = Vector3Subtract(worldPos, ray.position);

        // Test intersection with the 2 planes perpendicular to the OBB's X axis
        {
            //  Vector3 xAxis = { transform.m0, transform.m4, transform.m8 };
            float e = Vector3DotProduct(xAxis, delta);
            float f = Vector3DotProduct(ray.direction, xAxis);

            if (fabs(f) > 0.001f) // Standard case
            {

                float t1 = (e + bbox.min.x) / f; // Intersection with the "left" plane
                float t2 = (e + bbox.max.x) / f; // Intersection with the "right" plane
                // t1 and t2 now contain distances between ray origin and ray-plane intersections

                // We want t1 to represent the nearest intersection, 
                // so if it's not the case, invert t1 and t2
                if (t1 > t2)
                {
                    float w = t1; t1 = t2; t2 = w; // swap t1 and t2
                }

                // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
                if (t2 < tMax)
                    tMax = t2;
                // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
                if (t1 > tMin)
                    tMin = t1;

                // And here's the trick :
                // If "far" is closer than "near", then there is NO intersection.
                // See the images in the tutorials for the visual explanation.
                if (tMax < tMin)
                    return hit;

            }
            else // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            {
                if (-e + bbox.min.x > 0.0f || -e + bbox.max.x < 0.0f)
                    return hit;
            }
        }

        // Test intersection with the 2 planes perpendicular to the OBB's Y axis
        // Exactly the same thing than above.
        {
            //   Vector3 yaxis = { transform.m1, transform.m5, transform.m9 };
            float e = Vector3DotProduct(yAxis, delta);
            float f = Vector3DotProduct(ray.direction, yAxis);

            if (fabs(f) > 0.001f)
            {
                float t1 = (e + bbox.min.y) / f;
                float t2 = (e + bbox.max.y) / f;

                if (t1 > t2)
                {
                    float w = t1;
                    t1 = t2;
                    t2 = w;
                }

                if (t2 < tMax)
                    tMax = t2;
                if (t1 > tMin)
                    tMin = t1;
                if (tMin > tMax)
                    return hit;
            }
            else
            {
                if (-e + bbox.min.y > 0.0f || -e + bbox.max.y < 0.0f)
                    return hit;
            }
        }

        // Test intersection with the 2 planes perpendicular to the OBB's Z axis
        // Exactly the same thing than above.
        {
            //  Vector3 zaxis = { transform.m2, transform.m6, transform.m10 };
            float e = Vector3DotProduct(zAxis, delta);
            float f = Vector3DotProduct(ray.direction, zAxis);

            if (fabs(f) > 0.001f)
            {

                float t1 = (e + bbox.min.z) / f;
                float t2 = (e + bbox.max.z) / f;

                if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

                if (t2 < tMax)
                    tMax = t2;
                if (t1 > tMin)
                    tMin = t1;
                if (tMin > tMax)
                    return hit;
            }
            else
            {
                if (-e + bbox.min.z > 0.0f || -e + bbox.max.z < 0.0f)
                    return hit;
            }
        }

        hit.hit = true;
        hit.distance = tMin;
        hit.point = Vector3Add(ray.position, Vector3Scale(ray.direction, tMin));

        return hit;
        return hit;
    }

    RayCollision GetRayCollisionBoxOriented(Ray ray, BoundingBox box, Matrix4x4& matrix)
    {
        return GetRayCollisionBoxOriented(ray, box, MatrixToTransform(matrix));
    }

    RayCollision GetRayCollisionBoxOriented(Ray ray, BoundingBox box, Matrix& matrix)
    {
        return GetRayCollisionBoxOriented(ray, box, (Matrix4x4&)matrix);
    }
}