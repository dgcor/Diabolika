#pragma once

#include <compare>
#include <cstdint>

template <class T>
struct PairXY
{
	T x;
	T y;

	constexpr PairXY() noexcept : x(0), y(0) {}
	constexpr PairXY(T x_, T y_) noexcept : x(x_), y(y_) {}

	constexpr auto operator<=>(const PairXY&) const = default;
};

template <typename T>
inline PairXY<T>& operator+=(PairXY<T>& left, const PairXY<T>& right)
{
	left.x += right.x;
	left.y += right.y;
	return left;
}

template <typename T>
inline PairXY<T>& operator-=(PairXY<T>& left, const PairXY<T>& right)
{
	left.x -= right.x;
	left.y -= right.y;
	return left;
}

template <typename T>
inline PairXY<T> operator+(const PairXY<T>& left, const PairXY<T>& right)
{
	return PairXY<T>(left.x + right.x, left.y + right.y);
}

template <typename T>
inline PairXY<T> operator-(const PairXY<T>& left, const PairXY<T>& right)
{
	return PairXY<T>(left.x - right.x, left.y - right.y);
}

template <typename T>
inline PairXY<T> operator*(const PairXY<T>& left, T right)
{
	return PairXY<T>(left.x * right, left.y * right);
}

template <typename T>
inline PairXY<T> operator*(T left, const PairXY<T>& right)
{
	return PairXY<T>(right.x * left, right.y * left);
}

template <typename T>
inline PairXY<T>& operator*=(PairXY<T>& left, T right)
{
	left.x *= right;
	left.y *= right;
	return left;
}

template <typename T>
inline PairXY<T> operator/(const PairXY<T>& left, T right)
{
	return PairXY<T>(left.x / right, left.y / right);
}

template <typename T>
inline PairXY<T>& operator/=(PairXY<T>& left, T right)
{
	left.x /= right;
	left.y /= right;
	return left;
}

using PairFloat = PairXY<float>;
using PairInt8 = PairXY<int8_t>;
using PairInt16 = PairXY<int16_t>;
using PairInt32 = PairXY<int32_t>;
using PairUInt8 = PairXY<uint8_t>;
using PairUInt16 = PairXY<uint16_t>;
using PairUInt32 = PairXY<uint32_t>;
