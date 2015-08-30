#pragma once

// FixedPoint works the same as float.
// sin and cos have lookup tables for speed.
//
#include <iostream>

#include "MathUtil.h"

namespace Sentinel
{
	// p = float precision (0-32)
	//
	template< int p >
	class FixedPoint
	{
	public:

		int value;

		FixedPoint() { value = 0; }
		FixedPoint(int num) { *this = num; }
		FixedPoint(unsigned int num) { *this = num; }
		FixedPoint(float num) { *this = num; }
		FixedPoint(double num) { *this = num; }

		~FixedPoint() {}

		FixedPoint& operator = (int num) { value = num << p; return *this; }
		FixedPoint& operator = (unsigned int num) { value = (int)num << p; return *this; }
		FixedPoint& operator = (float num) { value = FloatToFixed(num); return *this; }
		FixedPoint& operator = (double num) { value = FloatToFixed((float)num); return *this; }

		FixedPoint& operator += (FixedPoint fp)	{ value += fp.value; return *this; }
		FixedPoint& operator -= (FixedPoint fp)	{ value -= fp.value; return *this; }

		FixedPoint& operator *= (FixedPoint fp)	{ value = MathMul(fp.value); return *this; }
		FixedPoint& operator /= (FixedPoint fp)	{ value = MathDiv(fp.value); return *this; }

		FixedPoint& operator *= (int i) { value *= i; return *this; }
		FixedPoint& operator /= (int i) { value /= i; return *this; }
		FixedPoint& operator *= (unsigned int i) { value *= i; return *this; }
		FixedPoint& operator /= (unsigned int i) { value /= i; return *this; }

		FixedPoint& operator *= (float f) { value = MathMul(FloatToFixed(f)); return *this; }
		FixedPoint& operator /= (float f) { value = MathDiv(FloatToFixed(f)); return *this; }
		FixedPoint& operator *= (double f) { value = MathMul(FloatToFixed((float)f)); return *this; }
		FixedPoint& operator /= (double f) { value = MathDiv(FloatToFixed((float)f)); return *this; }

		FixedPoint  operator <<  (unsigned int i) { FixedPoint fp2 = *this; fp2 <<= i; return fp2; }
		FixedPoint  operator >>  (unsigned int i) { FixedPoint fp2 = *this; fp2 >>= i; return fp2; }
		FixedPoint& operator <<= (unsigned int i) { value <<= i; return *this; }
		FixedPoint& operator >>= (unsigned int i) { value >>= i; return *this; }

		operator float() { return (float)FixedToDouble(*this); }

		FixedPoint& operator %= (FixedPoint fp)
		{
			value = FloatToFixed((float)fmod(FixedToDouble(*this), FixedToDouble(fp)));
			return *this;
		}

		// Math operations.
		//
#define FIXED_POINT_MATH(op)\
		template< class T >\
		friend FixedPoint operator op (T num, const FixedPoint& fp)\
				{ FixedPoint fp2 = num; fp2 op= fp; return fp2; }

		//FIXED_POINT_MATH(+)
		//FIXED_POINT_MATH(-)
		//FIXED_POINT_MATH(*)
		//FIXED_POINT_MATH(/)
		FIXED_POINT_MATH(%)

			// Logical operations.
			//
#define FIXED_POINT_BOOL(op)\
		template< class T >\
		bool operator op (T fp)\
				{ FixedPoint fp2 = fp; return value op fp2.value; }

			FIXED_POINT_BOOL(==)
			FIXED_POINT_BOOL(!=)
			FIXED_POINT_BOOL(>=)
			FIXED_POINT_BOOL(<=)
			FIXED_POINT_BOOL(>)
			FIXED_POINT_BOOL(<)
			FIXED_POINT_BOOL(&&)
			FIXED_POINT_BOOL(||)
			bool operator ! () const { return (value == 0) ? true : false; }

#define FIXED_POINT_LOGIC(op)\
		template< class T >\
		FixedPoint operator op (T fp) const\
				{ FixedPoint fp2 = fp; fp2.value = value op fp2.value; return fp2; }

		FIXED_POINT_LOGIC(&)
			FIXED_POINT_LOGIC(|)
			FIXED_POINT_LOGIC(^)

#define FIXED_POINT_UNARY(op)\
		FixedPoint operator op () const { FixedPoint fp; fp.value = op value; return fp; }

			FIXED_POINT_UNARY(~)
			FIXED_POINT_UNARY(-)
			FIXED_POINT_UNARY(+)

#define FIXED_POINT_UNARY_EQ(op)\
		template< class T >\
		FixedPoint& operator op (T fp) { FixedPoint fp2 = fp; value op fp2.value; return *this; }

			FIXED_POINT_UNARY_EQ(&=)
			FIXED_POINT_UNARY_EQ(|=)

			friend std::ostream& operator << (std::ostream& out, const FixedPoint& fp)
		{
			return out << FixedToDouble(fp);
		}

	private:

		static inline double FixedToDouble(FixedPoint< p > fp)
		{
			return (double)fp.value / (double)(1 << p);
		}

		static inline int FloatToFixed(float f)
		{
			return (int)(f * (float)(1 << p));
		}

		inline int MathMul(int i)
		{
			return (int)(((long long)value * i) >> p);
		}

		inline int MathDiv(int i)
		{
			return (int)((((long long)value) << p) / i);
		}

		friend void CreateTrigTable16();
		friend void CreateTrigTable8();
	};

	typedef FixedPoint<16> Fixed16; // 16/16-bit precision
	typedef FixedPoint<8>  Fixed8;  // 24/8-bit precision

	static int SinTable_Fixed16[] = {
#include "sin16.inl"
	};

	static int SinTable_Fixed8[] = {
#include "sin8.inl"
	};

	inline void CreateTrigTable16()
	{
		FILE* fp;
		fopen_s(&fp, "sin16.inl", "w+");

		Fixed16 deg90 = PI / 2.0;
		Fixed16 num = 0;
		Fixed16 temp = 0;

		while (num.value < deg90.value)
		{
			temp = sin(Fixed16::FixedToDouble(num));

			fprintf(fp, "%d,\n", temp.value);

			num.value += 1;
		}
		temp = 1;
		fprintf(fp, "%d", temp.value);	// final value should always be 1

		fclose(fp);
	}

	inline void CreateTrigTable8()
	{
		FILE* fp;
		fopen_s(&fp, "sin8.inl", "w+");

		Fixed8 deg90 = PI / 2.0;
		Fixed8 num = 0;
		Fixed8 temp = 0;

		while (num.value < deg90.value)
		{
			temp = sin(Fixed8::FixedToDouble(num));

			fprintf(fp, "%d,\n", temp.value);

			num.value += 1;
		}
		temp = 1;
		fprintf(fp, "%d", temp.value);	// final value should always be 1

		fclose(fp);
	}

#define DEG_HELPER(type, name)\
	const type name##_DEG_90  = (type)PI / 2.0;\
	const type name##_DEG_180 = (type)PI;\
	const type name##_DEG_270 = (type)PI * 3.0 / 2.0;\
	const type name##_DEG_360 = 2.0 * (type)PI;

#define SIN_HELPER(type, name)\
	inline type sin(type num)\
		{\
		type fp = 0;\
				while(num < 0)\
						{\
			num += name##_DEG_360;\
						}\
								while(num > name##_DEG_360)\
										{\
			num -= name##_DEG_360;\
										}\
		if(num <= name##_DEG_90)\
				{\
			fp.value = SinTable_##type[num.value];\
				}\
				else\
		if(num < name##_DEG_180)\
				{\
			fp.value = SinTable_##type[name##_DEG_180.value - num.value];\
				}\
				else\
		if(num < name##_DEG_270)\
				{\
			fp.value = -SinTable_##type[num.value - name##_DEG_180.value];\
				}\
				else\
			{\
			fp.value = -SinTable_##type[name##_DEG_360.value - num.value];\
			}\
		return fp;\
		}

#define COS_HELPER(type, name)\
	inline type cos(type num)\
		{\
		type fp = 0;\
				while(num < 0)\
						{\
			num += name##_DEG_360;\
						}\
								while(num > name##_DEG_360)\
										{\
			num -= name##_DEG_360;\
										}\
		if(num <= name##_DEG_90)\
				{\
			fp.value = SinTable_##type[name##_DEG_90.value - num.value];\
				}\
				else\
		if(num < name##_DEG_180)\
				{\
			fp.value = -SinTable_##type[num.value - name##_DEG_90.value];\
				}\
				else\
		if(num < name##_DEG_270)\
				{\
			fp.value = -SinTable_##type[name##_DEG_270.value - num.value];\
				}\
				else\
			{\
			fp.value = SinTable_##type[name##_DEG_90.value - (name##_DEG_360.value - num.value)];\
			}\
		return fp;\
		}

	DEG_HELPER(Fixed16, FIXED16)
	SIN_HELPER(Fixed16, FIXED16)
	COS_HELPER(Fixed16, FIXED16)

	DEG_HELPER(Fixed8, FIXED8)
	SIN_HELPER(Fixed8, FIXED8)
	COS_HELPER(Fixed8, FIXED8)
};