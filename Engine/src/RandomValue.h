#pragma once

#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	inline int RandomValue( int minValue, int maxValue )
	{
		if( maxValue > minValue )
			std::swap( maxValue, minValue );

		return minValue + rand()%(maxValue - minValue);
	}

	inline unsigned int RandomValue( unsigned int minValue, unsigned int maxValue )
	{
		if( maxValue > minValue )
			std::swap( maxValue, minValue );

		return minValue + rand()%(maxValue - minValue);
	}

	inline float RandomValue( float minValue, float maxValue )
	{
		if( maxValue > minValue )
			std::swap( maxValue, minValue );

		return minValue + (maxValue - minValue) * (rand() / static_cast< float >( RAND_MAX ));
	}

	inline double RandomValue( double minValue, double maxValue )
	{
		if( maxValue > minValue )
			std::swap( maxValue, minValue );

		return minValue + (maxValue - minValue) * (rand() / static_cast< double >( RAND_MAX ));
	}

	inline Vector2f RandomValue( const Vector2f& minValue, const Vector2f& maxValue )
	{
		return Vector2f( RandomValue( minValue.x, maxValue.x ), \
						 RandomValue( minValue.y, maxValue.y ));
	}

	inline Vector3f RandomValue( const Vector3f& minValue, const Vector3f& maxValue )
	{
		return Vector3f( RandomValue( minValue.x, maxValue.x ), \
						 RandomValue( minValue.y, maxValue.y ), \
						 RandomValue( minValue.z, maxValue.z ));
	}

	inline Vector4f RandomValue( const Vector4f& minValue, const Vector4f& maxValue )
	{
		return Vector4f( RandomValue( minValue.x, maxValue.x ), \
						 RandomValue( minValue.y, maxValue.y ), \
						 RandomValue( minValue.z, maxValue.z ),
						 RandomValue( minValue.w, maxValue.w ));
	}

	inline ColorRGBA RandomValue( const ColorRGBA& minColor, const ColorRGBA& maxColor )
	{
		return ColorRGBA( RandomValue( minColor.r, maxColor.r ), \
						  RandomValue( minColor.g, maxColor.g ), \
						  RandomValue( minColor.b, maxColor.b ), \
						  RandomValue( minColor.a, maxColor.a ));
	}
}
