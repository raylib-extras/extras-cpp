/**********************************************************************************************
*
*   raylib-extrss * Utilities and Shared Components for Raylib
*
*   raymath_operators.h * CPP operator overloads for raymath methods
*
*   LICENSE: ZLiB
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

//------------------Vector2-----------------//
inline Vector2 operator+ (Vector2 lhs, const Vector2& rhs)
{
	return Vector2{ lhs.x + rhs.x, lhs.y + rhs.y };
}

inline Vector2 operator+ (Vector2 lhs, const float& rhs)
{
	return Vector2{ lhs.x + rhs, lhs.y + rhs };
}

inline Vector2 operator- (Vector2 lhs, const Vector2& rhs)
{
	return Vector2{ lhs.x - rhs.x, lhs.y - rhs.y };
}

inline Vector2 operator- (Vector2 lhs, const float& rhs)
{
	return Vector2{ lhs.x - rhs, lhs.y - rhs };
}

inline Vector2 operator* (Vector2 lhs, const float& rhs)
{
	return Vector2{ lhs.x  * rhs, lhs.y * rhs };
}

inline Vector2 operator* (Vector2 lhs, const Vector2& rhs)
{
	return Vector2{ lhs.x * rhs.x, lhs.y * rhs.y };
}

inline Vector2 operator/ (Vector2 lhs, const float& rhs)
{
	return Vector2{ lhs.x / rhs, lhs.y / rhs };
}

inline Vector2 operator/ (Vector2 lhs, const Vector2& rhs)
{
	return Vector2{ lhs.x / rhs.y, lhs.y / rhs.y };
}

static constexpr Vector2 Vector2Zero = { 0,0 };
static constexpr Vector2 Vector2X = { 1,0 };
static constexpr Vector2 Vector2Y = { 0,1 };

//------------------Vector3-----------------//
inline Vector3 operator+ (Vector3 lhs, const Vector3& rhs)
{
	return Vector3{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

inline Vector3 operator+ (Vector3 lhs, const float& rhs)
{
	return Vector3{ lhs.x + rhs, lhs.y + rhs, lhs.z + rhs };
}

inline Vector3 operator- (Vector3 lhs, const Vector3& rhs)
{
	return Vector3{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

inline Vector3 operator- (Vector3 lhs, const float& rhs)
{
	return Vector3{ lhs.x - rhs, lhs.y - rhs, lhs.z - rhs };
}

inline Vector3 operator* (Vector3 lhs, const float& rhs)
{
	return Vector3{ lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}

inline Vector3 operator* (Vector3 lhs, const Vector3& rhs)
{
	return Vector3{ lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}

inline Vector3 operator/ (Vector3 lhs, const float& rhs)
{
	return Vector3{ lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
}

inline Vector3 operator/ (Vector3 lhs, const Vector3& rhs)
{
	return Vector3{ lhs.x / rhs.y, lhs.y / rhs.y, lhs.z / rhs.z };
}

static constexpr Vector3 Vector3Zero = { 0, 0, 0 };
static constexpr Vector3 Vector3X = { 1, 0, 0 };
static constexpr Vector3 Vector3Y = { 0, 1, 0 };
static constexpr Vector3 Vector3Z = { 0, 0, 1 };
