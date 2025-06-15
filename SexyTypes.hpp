#pragma once
#include <cstdint>
#include <string>

#ifdef __arm__
#define UINT uint
#define UINTPTR uintptr_t
#define INT int
#define SHORT short
#define USHORT unsigned short
#define LONG long
#define ULONG unsigned long

#elif __aarch64__
#define UINT uint64_t
#define UINTPTR uint64_t
#define INT int64_t
#define SHORT long short
#define USHORT unsigned SHORT
#define LONG long long
#define ULONG unsigned LONG

#endif

namespace Sexy
{
	struct SexyVector2 {
		float mX, mY;
		SexyVector2() : mX(0), mY(0) {};
		SexyVector2(float x, float y) : mX(x), mY(y) {};
	};

	struct SexyVector3
	{
		float mX, mY, mZ;

		SexyVector3() : mX(0), mY(0), mZ(0) {};

		SexyVector3(float x, float y, float z)
		{
			mX = x;
			mY = y;
			mZ = z;
		}
	};

	struct Point {
		INT x;
		INT y;

		Point() {};

		Point(INT x, INT y) : x(x), y(y) {};
	};

	struct FPoint {
		float x;
		float y;

		FPoint() {};

		FPoint(float x, float y) : x(x), y(y) {};
	};
}

struct Rect
{
	INT mX, mY, mWidth, mHeight;

	Rect() {
		this->mX = 0;
		this->mY = 0;
		this->mWidth = 0;
		this->mHeight = 0;
	}

	Rect(INT mX, INT mY, INT mWidth, INT mHeight) {
		this->mX = mX;
		this->mY = mY;
		this->mWidth = mWidth;
		this->mHeight = mHeight;
	}
};

struct FRect {
	float mX, mY, mWidth, mHeight;

	FRect() {
		this->mX = 0;
		this->mY = 0;
		this->mWidth = 0;
		this->mHeight = 0;
	}

	FRect(float mX, float mY, float mWidth, float mHeight) {
		this->mX = mX;
		this->mY = mY;
		this->mWidth = mWidth;
		this->mHeight = mHeight;
	}
};

struct CornerRect {
	Sexy::FPoint topLeft;
	Sexy::FPoint topRight;
	Sexy::FPoint bottomLeft;
	Sexy::FPoint bottomRight;

	CornerRect() {}

	CornerRect(Sexy::FPoint&& topLeft, Sexy::FPoint&& topRight, Sexy::FPoint&& bottomLeft, Sexy::FPoint&& bottomRight) :
		topLeft(topLeft), topRight(topRight), bottomLeft(bottomLeft), bottomRight(bottomRight) {
	};
};

struct ValueRange
{
	float Min;
	float Max;

	ValueRange() : Min(0), Max(0) {};

	ValueRange(float Min, float Max) : Min(Min), Max(Max) {};
};