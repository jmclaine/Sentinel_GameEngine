#pragma once

#include <cmath>
#include <utility>
#include <cfloat>
#include <string.h>

#ifdef WIN32
	#include <windows.h>
#else
	#define _ASSERT( x );
	typedef unsigned int UINT;
#endif

namespace Sentinel
{
	#define PI						3.1415926535897932384626433832795
	#define RADIANS_TO_DEGREES		57.295779513082320876798154814105
	#define DEGREES_TO_RADIANS		0.0174532925199432957692369076849
	#define EPSILON					1e-4

	inline bool QuadraticFormula( float a, float b, float c, float& r1, float& r2 )
	{
		float q = b*b - 4.0f*a*c;

		if( q >= 0.0f )
		{
			float sq = sqrt(q);
			float d = 1.0f / (2.0f*a);
			r1 = (-b + sq) * d;
			r2 = (-b - sq) * d;

			return true;
		}
		
		return false;
	}

	#define MATRIX_MULTIPLY( type, size )\
	{\
		type result;\
		for( UINT row = 0; row < size; ++row )\
		{\
			for( UINT col = 0; col < size; ++col )\
			{\
				UINT index = row * size + col;\
				for( UINT i = 0; i < size; ++i )\
				{\
					result[ index ] += m[ i*size+col ] * mat[ row*size+i ];\
				}\
			}\
		}\
		return result;\
	}

	// Quake III Arena version.
	//
	inline float invsqrt( float number )
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5F;
 
		x2 = number * 0.5F;
		y  = number;
		i  = * ( long * ) &y;                       // evil floating point bit level hacking
		i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
		y  = * ( float * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
 
		return y;
	}

	// Vector with 2 values.
	//
	#define CONST_VEC2	   const vec2< Real >&

	template< typename Real >
	struct vec2
	{
		Real x, y;

		vec2();
		vec2( Real _x, Real _y );
		vec2( Real* v );

		Real  operator [] ( int i ) const;
		Real& operator [] ( int i );
		vec2& operator += ( CONST_VEC2 v );
		vec2  operator +  ( CONST_VEC2 v ) const;
		vec2& operator -= ( CONST_VEC2 v );
		vec2  operator -  ( CONST_VEC2 v ) const;
		vec2  operator -  () const;
		vec2& operator *= ( const Real scalar );
		vec2  operator *  ( const Real scalar ) const;
		vec2& operator /= ( const Real scalar );
		vec2  operator /  ( const Real scalar ) const;
		bool  operator == ( CONST_VEC2 v );
		bool  operator != ( CONST_VEC2 v );

		Real* Ptr();

		Real Length() const;
		Real LengthSquared() const;
		Real LengthManhattan() const;

		vec2 Normalize() const;
	};

	typedef vec2< float >  vec2f;
	typedef vec2< double > vec2d;
	typedef vec2< int >	   vec2i;
	typedef vec2< short >  vec2s;


	// Vector with 3 values.
	//
	#define CONST_VEC3	   const vec3< Real >&

	template< typename Real >
	struct vec3
	{
		Real x, y, z;

		vec3();
		vec3( Real _x, Real _y, Real _z );
		vec3( Real* v );

		Real  operator [] ( int i ) const;
		Real& operator [] ( int i );

		vec3& operator += ( CONST_VEC3 v );
		vec3  operator +  ( CONST_VEC3 v ) const;
		vec3& operator -= ( CONST_VEC3 v );
		vec3  operator -  ( CONST_VEC3 v ) const;
		vec3  operator -  () const;
		vec3& operator *= ( const Real scalar );
		vec3  operator *  ( const Real scalar ) const;
		vec3& operator *= ( CONST_VEC3 v );
		vec3  operator *  ( CONST_VEC3 v ) const;
		vec3& operator /= ( const Real scalar );
		vec3  operator /  ( const Real scalar ) const;
		bool  operator == ( CONST_VEC3 v );
		bool  operator != ( CONST_VEC3 v );

		Real* Ptr();

		Real Length() const;
		Real LengthSquared() const;
		Real LengthManhattan() const;

		vec3 Normalize() const;

		Real Dot( CONST_VEC3 v ) const;
		vec3 Cross( CONST_VEC3 v ) const;

		void Scale( Real num );

		Real Angle( CONST_VEC3 v ) const;
	};

	typedef vec3< float >  vec3f;
	typedef vec3< double > vec3d;
	typedef vec3< int >    vec3i;
	typedef vec3< short >  vec3s;


	// Vector with 4 values.
	//
	#define CONST_VEC4	   const vec4< Real >&

	template< typename Real >
	struct vec4
	{
		Real x, y, z, w;

		vec4();
		vec4( Real _x, Real _y, Real _z, Real _w );
		vec4( Real* _v );

		Real  operator [] ( int i ) const;
		Real& operator [] ( int i );

		vec4& operator += ( CONST_VEC4 v );
		vec4  operator +  ( CONST_VEC4 v ) const;
		vec4& operator -= ( CONST_VEC4 v );
		vec4  operator -  ( CONST_VEC4 v ) const;
		vec4  operator -  () const;
		vec4& operator *= ( const Real scalar );
		vec4  operator *  ( const Real scalar ) const;
		vec4& operator /= ( const Real scalar );
		vec4  operator /  ( const Real scalar ) const;
		bool  operator == ( CONST_VEC4 v );
		bool  operator != ( CONST_VEC4 v );
		
		Real* Ptr();

		Real Length() const;
		Real LengthSquared() const;
		Real LengthManhattan() const;

		vec4 Normalize() const;

		Real Dot( CONST_VEC4 v ) const;
		vec4 Cross( CONST_VEC4 v ) const;

		void Scale( Real num );
	};

	typedef vec4< float >  vec4f;
	typedef vec4< double > vec4d;
	typedef vec4< int >    vec4i;
	typedef vec4< short >  vec4s;


	// 3x3 Matrix.
	//
	#define CONST_MAT3	   const mat3< Real >&

	template< typename Real >
	struct mat3
	{
		Real m[ 3*3 ];

		mat3();

		Real  operator [] ( unsigned int i ) const;
		Real& operator [] ( unsigned int i );
		mat3  operator *  ( CONST_MAT3 mat ) const;

		void Zero();
		void Identity();
		void Rotate( CONST_VEC3 v );

		Real Det();
		mat3 Inverse();

		vec3< Real > Transform( CONST_VEC3 v );
	};

	typedef mat3< float >  mat3f;
	typedef mat3< double > mat3d;


	// 4x4 Matrix.
	//
	#define CONST_MAT4	   const mat4< Real >&

	template< typename Real >
	struct mat4
	{
	public:

		Real m[ 4*4 ];

		mat4();

		Real  operator [] ( unsigned int i ) const;
		Real& operator [] ( unsigned int i );
		mat4  operator +  ( CONST_MAT4 mat ) const;
		mat4  operator *  ( CONST_MAT4 mat ) const;
		
		void Zero();
		void Identity();
		
		void Translate( CONST_VEC3 v );
		
		void Rotate( CONST_VEC3 v );
		void RotateX( Real degrees );
		void RotateY( Real degrees );
		void RotateZ( Real degrees );
		
		void Scale( Real _scale );
		void Scale( CONST_VEC3 _scale );
		
		void LookAtView( CONST_VEC3 pos, CONST_VEC3 lookAt, CONST_VEC3 up = vec3< Real >((Real)0, (Real)1, (Real)0) );
		void BillboardAxis( CONST_VEC3 posBB, CONST_VEC3 posCamera, CONST_VEC3 up = vec3< Real >((Real)0, (Real)1, (Real)0) );
		void BillboardWorld( CONST_VEC3 posBB, CONST_VEC3 posCamera, CONST_VEC3 up = vec3< Real >((Real)0, (Real)1, (Real)0) );
		
	private:

		void Billboard( CONST_VEC3 i, CONST_VEC3 j, CONST_VEC3 k, CONST_VEC3 pos, bool transpose );

	public:

		Real Det();
		void Transpose();
		void InverseTrans();
		mat4 InverseView();
		mat4 Inverse() const;

		void ProjectionOrthographic( HWND hWnd );
		void ProjectionPerspective( HWND hWnd, float nearZ = 0.1f, float farZ = 10000.0f, float FOV = 45.0f );

		vec3< Real > Transform( CONST_VEC3 v );

		vec3< Real > Forward();
		vec3< Real > Up();
		vec3< Real > Right();
	};

	typedef mat4< float >  mat4f;
	typedef mat4< double > mat4d;


	// Quaternion.
	//
	template< typename Real >
	struct quat
	{
		Real x, y, z, w;

		quat();
		quat( Real _pitch, Real _yaw, Real _roll );
		quat( Real _x, Real _y, Real _z, Real _w );

		Real  operator [] ( int i ) const;
		Real& operator [] ( int i );
		quat  operator *  ( const quat& q );
		quat  operator /  ( const Real scalar ) const;

		Real* Ptr();

		Real  Length() const;
		Real  LengthSquared() const;
		Real  LengthManhattan() const;

		quat  Normalize() const;

		quat& AxisAngle( Real _x, Real _y, Real _z, Real _degrees );
		quat& AxisAngle();

		quat& Euler( Real _pitch, Real _yaw, Real _roll );
		quat& Euler();

		mat4< Real > Matrix();
	};

	typedef quat< float >  quatf;
	typedef quat< double > quatd;


	//////////////////////////////////////////////////////////////////////////
	// Vector with 2 values.
	//
	template< typename Real >
	vec2< Real >::vec2() : x(0), y(0) {}

	template< typename Real >
	vec2< Real >::vec2( Real _x, Real _y ) : x(_x), y(_y) {}

	template< typename Real >
	vec2< Real >::vec2( Real* v ) : x(v[0]), y(v[1]) {}

	template< typename Real >
	Real vec2< Real >::operator [] ( int i ) const
	{
		_ASSERT( 0 <= i && i < 2 );

		return (&x)[i];
	}

	template< typename Real >
	Real& vec2< Real >::operator [] ( int i )
	{
		_ASSERT( 0 <= i && i < 2 );

		return (&x)[i];
	}

	template< typename Real >
	vec2< Real >& vec2< Real >::operator += ( const vec2& v )
	{
		x += v.x;
		y += v.y;

		return *this;
	}

	template< typename Real >
	vec2< Real > vec2< Real >::operator + ( const vec2& v ) const
	{
		vec2 result( *this );
		result += v;
		return result;
	}

	template< typename Real >
	vec2< Real >& vec2< Real >::operator -= ( const vec2& v )
	{
		x -= v.x;
		y -= v.y;

		return *this;
	}

	template< typename Real >
	vec2< Real > vec2< Real >::operator - () const
	{
		return vec2( -x, -y );
	}

	template< typename Real >
	vec2< Real > vec2< Real >::operator - ( const vec2& v ) const
	{
		vec2 result( *this );
		result -= v;
		return result;
	}

	template< typename Real >
	vec2< Real >& vec2< Real >::operator *= ( const Real scalar )
	{
		x *= scalar;
		y *= scalar;

		return *this;
	}

	template< typename Real >
	vec2< Real > vec2< Real >::operator * ( const Real scalar ) const
	{
		vec2 result( *this );
		result *= scalar;
		return result;
	}

	template< typename Real >
	vec2< Real >& vec2< Real >::operator /= ( const Real scalar )
	{
		x /= scalar;
		y /= scalar;

		return *this;
	}

	template< typename Real >
	vec2< Real > vec2< Real >::operator / ( const Real scalar ) const
	{
		vec2 result( *this );
		result /= scalar;
		return result;
	}

	template< typename Real >
	bool vec2< Real >::operator == ( const vec2& v )
	{
		return (x == v.x && y == v.y);
	}

	template< typename Real >
	bool vec2< Real >::operator != ( const vec2& v )
	{
		return (x != v.x && y != v.y);
	}

	template< typename Real >
	Real* vec2< Real >::Ptr()
	{
		return &x;
	}

	template< typename Real >
	Real vec2< Real >::Length() const
	{
		return sqrt( LengthSquared() );
	}

	template< typename Real >
	Real vec2< Real >::LengthSquared() const
	{
		return x*x + y*y;
	}

	template< typename Real >
	Real vec2< Real >::LengthManhattan() const
	{
		return abs(x) + abs(y);
	}

	template< typename Real >
	vec2< Real > vec2< Real >::Normalize() const
	{
		Real len = LengthSquared();

		if( len > 0 )
			return *this / sqrt( len );

		return vec2< Real >( 0, 0 );
	}


	//////////////////////////////////////////////////////////////////////////
	// Vector with 3 values.
	//
	template< typename Real >
	vec3< Real >::vec3() : x(0), y(0), z(0) {}

	template< typename Real >
	vec3< Real >::vec3( Real _x, Real _y, Real _z ) : x(_x), y(_y), z(_z) {}

	template< typename Real >
	vec3< Real >::vec3( Real* v ) : x(v[0]), y(v[1]), z(v[2]) {}

	template< typename Real >
	Real vec3< Real >::operator [] ( int i ) const
	{
		_ASSERT( 0 <= i && i < 3 );

		return (&x)[i];
	}

	template< typename Real >
	Real& vec3< Real >::operator [] ( int i )
	{
		_ASSERT( 0 <= i && i < 3 );

		return (&x)[i];
	}

	template< typename Real >
	vec3< Real >& vec3< Real >::operator += ( CONST_VEC3 v )
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	template< typename Real >
	vec3< Real > vec3< Real >::operator + ( CONST_VEC3 v ) const
	{
		vec3 result( *this );
		result += v;
		return result;
	}

	template< typename Real >
	vec3< Real >& vec3< Real >::operator -= ( CONST_VEC3 v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	template< typename Real >
	vec3< Real > vec3< Real >::operator - ( CONST_VEC3 v ) const
	{
		vec3 result( *this );
		result -= v;
		return result;
	}

	template< typename Real >
	vec3< Real > vec3< Real >::operator - () const
	{
		return vec3( -x, -y, -z );
	}

	template< typename Real >
	vec3< Real >& vec3< Real >::operator *= ( const Real scalar )
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;

		return *this;
	}

	template< typename Real >
	vec3< Real > vec3< Real >::operator * ( const Real scalar ) const
	{
		vec3 result( *this );
		result *= scalar;
		return result;
	}

	template< typename Real >
	vec3< Real >& vec3< Real >::operator *= ( CONST_VEC3 v )
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;
	}

	template< typename Real >
	vec3< Real > vec3< Real >::operator * ( CONST_VEC3 v ) const
	{
		vec3 result( *this );
		result *= v;
		return result;
	}

	template< typename Real >
	vec3< Real >& vec3< Real >::operator /= ( const Real scalar )
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;

		return *this;
	}

	template< typename Real >
	vec3< Real > vec3< Real >::operator / ( const Real scalar ) const
	{
		vec3 result( *this );
		result /= scalar;
		return result;
	}

	template< typename Real >
	bool vec3< Real >::operator == ( CONST_VEC3 v )
	{
		return (x == v.x && y == v.y && z == v.z);
	}

	template< typename Real >
	bool vec3< Real >::operator != ( CONST_VEC3 v )
	{
		return (x != v.x && y != v.y && z != v.z);
	}

	template< typename Real >
	Real* vec3< Real >::Ptr()
	{
		return &x;
	}

	template< typename Real >
	Real vec3< Real >::Length() const
	{
		return sqrt( LengthSquared() );
	}

	template< typename Real >
	Real vec3< Real >::LengthSquared() const
	{
		return x*x + y*y + z*z;
	}

	template< typename Real >
	Real vec3< Real >::LengthManhattan() const
	{
		return abs(x) + abs(y) + abs(z);
	}

	template< typename Real >
	vec3< Real > vec3< Real >::Normalize() const
	{
		Real len = LengthSquared();

		if( len > 0 )
			return *this / sqrt( len );

		return vec3< Real >( 0, 0, 0 );
	}

	template< typename Real >
	Real vec3< Real >::Dot( CONST_VEC3 v ) const
	{
		return x*v.x + y*v.y + z*v.z;
	}

	template< typename Real >
	vec3< Real > vec3< Real >::Cross( CONST_VEC3 v ) const
	{
		return vec3< Real >( y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x );
	}

	template< typename Real >
	void vec3< Real >::Scale( Real num )
	{
		x *= num;
		y *= num;
		z *= num;
	}

	template< typename Real >
	Real vec3< Real >::Angle( CONST_VEC3 v ) const
	{
		Real len0 = Length();
		Real len1 = v.Length();
		Real a = 0;

		if( len0 > 0 && len1 > 0 )
		{
			float d = (*this / len0).Dot( v / len1 );

			// Prevent floating point errors.
			//
			if( d < -1 )
			{
				d = -1;
			}
			else
			if( d > 1 )
			{
				d = 1;
			}
			a = acos( d );

			// Determine sign.
			// Assume up is 0, 1, 0
			//
			vec3< Real > sign = Cross( v );
			if( vec3< Real >( (Real)0, (Real)1, (Real)0 ).Dot( sign ) < 0 )
				a = -a;
		}

		return a;
	}


	//////////////////////////////////////////////////////////////////////////
	// Vector with 4 values.
	//
	template< typename Real >
	vec4< Real >::vec4() : x(0), y(0), z(0), w(0) {}

	template< typename Real >
	vec4< Real >::vec4( Real _x, Real _y, Real _z, Real _w ) : x(_x), y(_y), z(_z), w(_w) {}

	template< typename Real >
	vec4< Real >::vec4( Real* v ) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}

	template< typename Real >
	Real vec4< Real >::operator [] ( int i ) const
	{
		_ASSERT( 0 <= i && i < 4 );

		return (&x)[i];
	}

	template< typename Real >
	Real& vec4< Real >::operator [] ( int i )
	{
		_ASSERT( 0 <= i && i < 4 );

		return (&x)[i];
	}

	template< typename Real >
	vec4< Real >& vec4< Real >::operator += ( const vec4& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return *this;
	}

	template< typename Real >
	vec4< Real > vec4< Real >::operator + ( const vec4& v ) const
	{
		vec4 result( *this );
		result += v;
		return result;
	}

	template< typename Real >
	vec4< Real >& vec4< Real >::operator -= ( const vec4& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;
	}

	template< typename Real >
	vec4< Real > vec4< Real >::operator - ( const vec4& v ) const
	{
		vec4 result( *this );
		result -= v;
		return result;
	}

	template< typename Real >
	vec4< Real > vec4< Real >::operator - () const
	{
		return vec4( -x, -y, -z, -w );
	}

	template< typename Real >
	vec4< Real >& vec4< Real >::operator *= ( const Real scalar )
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;

		return *this;
	}

	template< typename Real >
	vec4< Real > vec4< Real >::operator * ( const Real scalar ) const
	{
		vec4 result( *this );
		result *= scalar;
		return result;
	}

	template< typename Real >
	vec4< Real >& vec4< Real >::operator /= ( const Real scalar )
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;

		return *this;
	}

	template< typename Real >
	vec4< Real > vec4< Real >::operator / ( const Real scalar ) const
	{
		vec4 result( *this );
		result /= scalar;
		return result;
	}

	template< typename Real >
	bool vec4< Real >::operator == ( const vec4& v )
	{
		return (x == v.x && y == v.y && z == v.z && w == v.w);
	}

	template< typename Real >
	bool vec4< Real >::operator != ( const vec4& v )
	{
		return (x != v.x && y != v.y && z != v.z && w != v.w);
	}

	template< typename Real >
	Real* vec4< Real >::Ptr()
	{
		return &x;
	}

	template< typename Real >
	Real vec4< Real >::Length() const
	{
		return sqrt( LengthSquared() );
	}

	template< typename Real >
	Real vec4< Real >::LengthSquared() const
	{
		return x*x + y*y + z*z + w*w;
	}

	template< typename Real >
	Real vec4< Real >::LengthManhattan() const
	{
		return abs(x) + abs(y) + abs(z) + abs(w);
	}

	template< typename Real >
	vec4< Real > vec4< Real >::Normalize() const
	{
		Real len = LengthSquared();

		if( len > 0 )
			return *this / sqrt( len );

		return vec4< Real >( 0, 0, 0, 0 );
	}

	template< typename Real >
	Real vec4< Real >::Dot( CONST_VEC4 v ) const
	{
		return x*v.x+y*v.y+z*v.z+w*v.w;
	}

	template< typename Real >
	vec4< Real > vec4< Real >::Cross( CONST_VEC4 v ) const
	{
		return vec4< Real >( y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x, (Real)0 );
	}

	template< typename Real >
	void vec4< Real >::Scale( Real num )
	{
		x *= num;
		y *= num;
		z *= num;
		w *= num;
	}


	//////////////////////////////////////////////////////////////////////////
	// 3x3 Matrix.
	//
	template< typename Real >
	mat3< Real >::mat3()
	{
		Zero();
	}

	template< typename Real >
	Real mat3< Real >::operator [] ( unsigned int i ) const
	{
		_ASSERT( i < 9 );
		return m[i];
	}

	template< typename Real >
	Real& mat3< Real >::operator [] ( unsigned int i )
	{
		_ASSERT( i < 9 );
		return m[i];
	}

	template< typename Real >
	mat3< Real > mat3< Real >::operator * ( const mat3& mat ) const
	{
		MATRIX_MULTIPLY( mat3, 3 );
	}

	template< typename Real >
	void mat3< Real >::Zero()
	{
		memset( m, 0, sizeof(Real)*9 );
	}

	template< typename Real >
	void mat3< Real >::Identity()
	{
		Zero();
		m[0] = 1;
		m[4] = 1;
		m[8] = 1;
	}

	template< typename Real >
	void mat3< Real >::Rotate( CONST_VEC3 v )
	{
		mat4< Real > matAngles;
		matAngles.Rotate( v.x, v.y, v.z );

		mat3< Real > mat;
		for( UINT x = 0; x < 3; ++x )
		{
			UINT index3 = x * 3;
			UINT index4 = x << 2;
			for( UINT y = 0; y < 3; ++y )
			{
				mat.m[ index3+y ] = matAngles[ index4+y ];
			}
		}

		*this = mat;
	}

	template< typename Real >
	Real mat3< Real >::Det()
	{
		return m[0]*(m[4]*m[8]-m[5]*m[7]) - m[1]*(m[3]*m[8]-m[5]*m[6]) + m[2]*(m[3]*m[7]-m[4]*m[6]);
	}

	template< typename Real >
	mat3< Real > mat3< Real >::Inverse()
	{
		return this / Det();
	}

	template< typename Real >
	vec3< Real > mat3< Real >::Transform( CONST_VEC3 v )
	{
		return vec3< Real >(v.x*m[0]+v.y*m[3]+v.z*m[6], 
							v.x*m[1]+v.y*m[4]+v.z*m[7], 
							v.x*m[2]+v.y*m[5]+v.z*m[8]);
	}


	//////////////////////////////////////////////////////////////////////////
	// 4x4 Matrix.
	//
	template< typename Real >
	mat4< Real >::mat4()
	{
		Zero();
	}

	template< typename Real >
	Real mat4< Real >::operator [] ( unsigned int i ) const { return m[i]; }

	template< typename Real >
	Real& mat4< Real >::operator [] ( unsigned int i ) { return m[i]; }

	template< typename Real >
	mat4< Real > mat4< Real >::operator + ( CONST_MAT4 mat ) const
	{
		mat4 result;

		for( UINT i = 0; i < 16; ++i )
		{
			result[ i ] = m[ i ] + mat[ i ];
		}

		return result;
	}

	template< typename Real >
	mat4< Real > mat4< Real >::operator * ( CONST_MAT4 mat ) const
	{
		MATRIX_MULTIPLY( mat4< Real >, 4 );
	}

	template< typename Real >
	void mat4< Real >::Zero()
	{
		memset( m, 0, sizeof(Real)*16 );
	}

	template< typename Real >
	void mat4< Real >::Identity()
	{
		Zero();
		m[0]  = 1.0f;
		m[5]  = 1.0f;
		m[10] = 1.0f;
		m[15] = 1.0f;
	};

	template< typename Real >
	void mat4< Real >::Translate( CONST_VEC3 v )
	{
		Identity();
		m[12] = v.x;
		m[13] = v.y;
		m[14] = v.z;
	}

	template< typename Real >
	void mat4< Real >::Rotate( CONST_VEC3 v )
	{
		*this = (quat< Real >( (Real)0, (Real)1, (Real)0, v.y ).AxisAngle() * 
			    (quat< Real >( (Real)1, (Real)0, (Real)0, v.x ).AxisAngle() * 
				 quat< Real >( (Real)0, (Real)0, (Real)1, v.z ).AxisAngle())).Matrix();
	}

	template< typename Real >
	void mat4< Real >::RotateX( Real degrees )
	{
		*this = (quat< Real >( (Real)1, (Real)0, (Real)0, degrees ).AxisAngle()).Matrix();

		/*
		degrees *= (Real)DEGREES_TO_RADIANS;
		
		Zero();
		m[0]  = 1;
		m[5]  = cos( degrees );
		m[6]  = sin( degrees );

		m[9]  = -sin( degrees );
		m[10] = cos( degrees );

		m[15] = 1;
		*/
	}

	template< typename Real >
	void mat4< Real >::RotateY( Real degrees )
	{
		*this = (quat< Real >( (Real)0, (Real)1, (Real)0, degrees ).AxisAngle()).Matrix();

		/*
		degrees *= (Real)DEGREES_TO_RADIANS;

		Zero();
		m[0]  = cos( degrees );
		m[2]  = -sin( degrees );
		m[5]  = 1;

		m[8]  = sin( degrees );
		m[10] = cos( degrees );

		m[15] = 1;
		*/
	}

	template< typename Real >
	void mat4< Real >::RotateZ( Real degrees )
	{
		*this = (quat< Real >( (Real)0, (Real)0, (Real)1, degrees ).AxisAngle()).Matrix();

		/*
		degrees *= (Real)DEGREES_TO_RADIANS;

		Zero();
		m[0]  = cos( degrees );
		m[1]  = sin( degrees );

		m[4]  = -sin( degrees );
		m[5]  = cos( degrees );

		m[10] = 1;
		m[15] = 1;
		*/
	}

	template< typename Real >
	void mat4< Real >::Scale( Real _scale )
	{
		Scale( vec3< Real >( _scale, _scale, _scale ));
	}

	template< typename Real >
	void mat4< Real >::Scale( CONST_VEC3 _scale )
	{
		Zero();
		m[0]  = _scale.x;
		m[5]  = _scale.y;
		m[10] = _scale.z;
		m[15] = 1.0f;
	}

	template< typename Real >
	void mat4< Real >::Billboard( CONST_VEC3 i, CONST_VEC3 j, CONST_VEC3 k, CONST_VEC3 pos, bool transpose )
	{
		mat4< Real > matLookAt;
		
		matLookAt.m[0]  = i.x;
		matLookAt.m[1]  = i.y;
		matLookAt.m[2]  = i.z;

		matLookAt.m[4]  = j.x;
		matLookAt.m[5]  = j.y;
		matLookAt.m[6]  = j.z;

		matLookAt.m[8]  = k.x;
		matLookAt.m[9]  = k.y;
		matLookAt.m[10] = k.z;

		matLookAt.m[15] = 1.0f;

		if( transpose )
		{
			matLookAt.Transpose();
		}

		mat4< Real > matTrans;
		matTrans.Translate( pos );

		*this = matLookAt * matTrans;
	}

	template< typename Real >
	void mat4< Real >::LookAtView( CONST_VEC3 pos, CONST_VEC3 lookAt, CONST_VEC3 up )
	{
		vec3< Real > i, j, k;

		// Forward
		k = vec3< Real >(pos.x-lookAt.x, pos.y-lookAt.y, pos.z-lookAt.z).Normalize();

		// Side
		j = up;
		i = j.Cross( k ).Normalize();

		// Up
		j = k.Cross( i );

		Billboard( i, j, k, pos*-1.0f, true );
	}

	template< typename Real >
	void mat4< Real >::BillboardAxis( CONST_VEC3 posBB, CONST_VEC3 posCamera, CONST_VEC3 up )
	{
		vec3< Real > i, j, k;

		j = up.Normalize();

		i = j.Cross( posCamera-posBB ).Normalize();

		k = i.Cross( j ).Normalize();

		Billboard( i, j, k, posBB, false );
	}

	template< typename Real >
	void mat4< Real >::BillboardWorld( CONST_VEC3 posBB, CONST_VEC3 posCamera, CONST_VEC3 up )
	{
		vec3< Real > i, j, k;

		// Forward
		k = (posCamera - posBB).Normalize();

		// Side
		j = up.Normalize();
		i = j.Cross( k ).Normalize();

		// Up
		j = k.Cross( i ).Normalize();

		Billboard( i, j, k, posBB, false );
	}

	template< typename Real >
	void mat4< Real >::ProjectionOrthographic( HWND hWnd )
	{
		RECT clientRect;
		GetClientRect( hWnd, &clientRect );

		Zero();

		m[0]  =  2.0f / (float)(clientRect.right - clientRect.left);
		m[5]  = -2.0f / (float)(clientRect.bottom - clientRect.top);
		m[10] =  1.0f;

		m[12] = -1.0f;
		m[13] =  1.0f;
		m[14] =  1.0f;
		m[15] =  1.0f;
	}

	template< typename Real >
	void mat4< Real >::ProjectionPerspective( HWND hWnd, float nearZ = 0.1f, float farZ = 10000.0f, float FOV = 45.0f )
	{
		// Calculate aspect ratio.
		//
		RECT clientRect;
		GetClientRect( hWnd, &clientRect );
		float aspectRatio = (clientRect.right - clientRect.left) / static_cast< float >( clientRect.bottom - clientRect.top );

		// Setup perspective projection.
		//
		float size = 1.0f / tan( (float)DEGREES_TO_RADIANS * FOV / 2.0f );
		float depth = nearZ - farZ;

		Zero();

		m[0]  = size / aspectRatio;
		m[5]  = size;
		m[10] = (farZ + nearZ) / depth;
		m[11] = -1.0f;
		m[14] = 2.0f * (nearZ * farZ) / depth;
		m[15] = 1.0f;
	}

	template< typename Real >
	Real mat4< Real >::Det()
	{
		Real left1 = m[0] * m[5]  - m[4]  * m[1];
		Real left2 = m[0] * m[9]  - m[8]  * m[1];
		Real left3 = m[0] * m[13] - m[12] * m[1];
		Real left4 = m[0] * m[9]  - m[8]  * m[5];
		Real left5 = m[4] * m[13] - m[12] * m[5];
		Real left6 = m[8] * m[13] - m[12] * m[9];

		Real right1 = m[2]  * m[7]  - m[6]  * m[3];
		Real right2 = m[2]  * m[11] - m[10] * m[3];
		Real right3 = m[2]  * m[15] - m[14] * m[3];
		Real right4 = m[6]  * m[11] - m[10] * m[7];
		Real right5 = m[6]  * m[15] - m[14] * m[7];
		Real right6 = m[10] * m[15] - m[14] * m[11];

		return (left1 * right6 - left2 * right5 + \
			    left3 * right4 + left4 * right3 - \
			    left5 * right2 + left6 * right1);
	}

	template< typename Real >
	void mat4< Real >::Transpose()
	{
		std::swap( m[1],  m[4]  );
		std::swap( m[2],  m[8]  );
		std::swap( m[3],  m[12] );
		std::swap( m[6],  m[9]  );
		std::swap( m[7],  m[13] );
		std::swap( m[11], m[14] );
	}

	template< typename Real >
	void mat4< Real >::InverseTrans()
	{
		m[12] *= -1;
		m[13] *= -1;
		m[14] *= -1;
	}

	template< typename Real >
	mat4< Real > mat4< Real >::InverseView()
	{
		vec3< Real > v( m[12], m[13], m[14] );
		vec3< Real > w( v.Dot( vec3< Real >( m[0], m[1], m[2] )), 
						v.Dot( vec3< Real >( m[4], m[5], m[6] )), 
						v.Dot( vec3< Real >( m[8], m[9], m[10] )));
		mat4< Real > temp( m[0], m[4], m[8],  0,
						   m[1], m[5], m[9],  0,
						   m[2], m[6], m[10], 0,
						   -w.x, -w.y, -w.z,  1 );
		return temp;
	}

	template< typename Real >
	mat4< Real > mat4< Real >::Inverse() const
	{
		mat4< Real > inv;

		Real m10_15 = m[10]*m[15];	Real m11_14	= m[11]*m[14];	Real m4_1	= m[4]*m[1];
		Real m4_9   = m[4]*m[9];	Real m11_13 = m[11]*m[13];	Real m2_15	= m[2]*m[15];
		Real m0_9	= m[0]*m[9];	Real m10_13 = m[10]*m[13];	Real m8_5	= m[8]*m[5];
		Real m6_15	= m[6]*m[15];	Real m0_7	= m[0]*m[7];	Real m8_1	= m[8]*m[1];
		Real m0_5	= m[0]*m[5];	Real m1_7	= m[1]*m[7];	Real m2_11	= m[2]*m[11];
		Real m6_11	= m[6]*m[11];	Real m0_6	= m[0]*m[6];	Real m8_13	= m[8]*m[13];
																Real m8_3	= m[8]*m[3];

		Real m4_3	= m[4]*m[3];	Real m12_5	= m[12]*m[5];	Real m7_10	= m[7]*m[10];
		Real m9_14	= m[9]*m[14];	Real m12_1	= m[12]*m[1];	Real m12_9	= m[12]*m[9];
		Real m5_3	= m[5]*m[3];	Real m2_7	= m[2]*m[7];	Real m3_10	= m[3]*m[10];
		Real m4_2	= m[4]*m[2];								Real m3_6	= m[3]*m[6];
		Real m8_7	= m[8]*m[7];

		inv[0]  =  m[5]*m10_15 - m[5]*m11_14 - m[9]*m6_15 + m9_14*m[7] + m[13]*m6_11 - m[13]*m7_10;
		inv[4]  = -m[4]*m10_15 + m[4]*m11_14 + m[8]*m6_15 - m8_7*m[14] - m[12]*m6_11 + m[12]*m7_10;
		inv[8]  =  m4_9*m[15]  - m[4]*m11_13 - m8_5*m[15] + m8_7*m[13] + m12_5*m[11] - m12_9*m[7];
		inv[12] = -m4_9*m[14]  + m[4]*m10_13 + m8_5*m[14] - m8_13*m[6] - m12_5*m[10] + m12_9*m[6];
		inv[1]  = -m[1]*m10_15 + m[1]*m11_14 + m[9]*m2_15 - m9_14*m[3] - m[13]*m2_11 + m[13]*m3_10;
		inv[5]  =  m[0]*m10_15 - m[0]*m11_14 - m[8]*m2_15 + m8_3*m[14] + m[12]*m2_11 - m[12]*m3_10;
		inv[9]  = -m0_9*m[15]  + m[0]*m11_13 + m8_1*m[15] - m8_13*m[3] - m12_1*m[11] + m12_9*m[3];
		inv[13] =  m0_9*m[14]  - m[0]*m10_13 - m8_1*m[14] + m8_13*m[2] + m12_1*m[10] - m12_9*m[2];
		inv[2]  =  m[1]*m6_15  - m1_7*m[14]  - m[5]*m2_15 + m5_3*m[14] + m[13]*m2_7  - m[13]*m3_6;
		inv[6]  = -m[0]*m6_15  + m0_7*m[14]  + m[4]*m2_15 - m4_3*m[14] - m[12]*m2_7  + m[12]*m3_6;
		inv[10] =  m0_5*m[15]  - m0_7*m[13]  - m4_1*m[15] + m4_3*m[13] + m12_1*m[7]  - m12_5*m[3];
		inv[14] = -m0_5*m[14]  + m0_6*m[13]  + m4_1*m[14] - m4_2*m[13] - m12_1*m[6]  + m12_5*m[2];
		inv[3]  = -m[1]*m6_11  + m1_7*m[10]  + m[5]*m2_11 - m5_3*m[10] - m[9]*m2_7   + m[9]*m3_6;
		inv[7]  =  m[0]*m6_11  - m0_7*m[10]  - m[4]*m2_11 + m4_3*m[10] + m[8]*m2_7   - m[8]*m3_6;
		inv[11] = -m0_5*m[11]  + m0_7*m[9]   + m4_1*m[11] - m4_3*m[9]  - m8_1*m[7]   + m8_5*m[3];
		inv[15] =  m0_5*m[10]  - m0_6*m[9]   - m4_1*m[10] + m4_2*m[9]  + m8_1*m[6]   - m8_5*m[2];

		Real det = m[0]*inv.m[0] + m[1]*inv.m[4] + m[2]*inv.m[8] + m[3]*inv.m[12];

		if( det == 0 )
		{
			inv.Identity();
			return inv;
		}

		det = (Real)1.0 / det;

		for( UINT i = 0; i < 16; i++ )
		{
			inv.m[i] *= det;
		}

		return inv;
	}

	template< typename Real >
	vec3< Real > mat4< Real >::Transform( CONST_VEC3 v )
	{
		return vec3< Real >( v.x*m[0]+v.y*m[4]+v.z*m[8]+m[12], 
							 v.x*m[1]+v.y*m[5]+v.z*m[9]+m[13], 
							 v.x*m[2]+v.y*m[6]+v.z*m[10]+m[14] );
	}

	template< typename Real >
	vec3< Real > mat4< Real >::Right()
	{
		return vec3< Real >( m[0], m[1], m[2] );
	}

	template< typename Real >
	vec3< Real > mat4< Real >::Up()
	{
		return vec3< Real >( m[4], m[5], m[6] );
	}

	template< typename Real >
	vec3< Real > mat4< Real >::Forward()
	{
		return -vec3< Real >( m[8], m[9], m[10] );
	}


	//////////////////////////////////////////////////////////////////////////
	// Quaternion.
	//
	template< typename Real >
	quat< Real >::quat() : x(0), y(0), z(0), w(1) {}

	template< typename Real >
	quat< Real >::quat( Real _pitch, Real _yaw, Real _roll ) { Euler( _pitch, _yaw, _roll ); }

	template< typename Real >
	quat< Real >::quat( Real _x, Real _y, Real _z, Real _w ) : x(_x), y(_y), z(_z), w(_w) {}

	template< typename Real >
	Real quat< Real >::operator [] ( int i ) const
	{
		_ASSERT( i >= 0 && i < 4 );
		return (&x)[i];
	}

	template< typename Real >
	Real& quat< Real >::operator [] ( int i )
	{
		_ASSERT( i >= 0 && i < 4 );
		return (&x)[i];
	}

	template< typename Real >
	quat< Real > quat< Real >::operator * ( const quat< Real >& q )
	{
		//	(Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
		//	(Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
		//	(Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
		//	(Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2)
		return quat< Real >( w*q.x + x*q.w + y*q.z - z*q.y,
							 w*q.y - x*q.z + y*q.w + z*q.x,
							 w*q.z + x*q.y - y*q.x + z*q.w,
							 w*q.w - x*q.x - y*q.y - z*q.z );
	}

	template< typename Real >
	quat< Real > quat< Real >::operator /  ( const Real scalar ) const
	{
		return quat< Real >( x / scalar, y / scalar, z / scalar, w / scalar );
	}

	template< typename Real >
	Real* quat< Real >::Ptr()
	{
		return &x;
	}

	template< typename Real >
	Real quat< Real >::Length() const
	{
		return sqrt( LengthSquared() );
	}

	template< typename Real >
	Real quat< Real >::LengthSquared() const
	{
		return x*x + y*y + z*z + w*w;
	}

	template< typename Real >
	Real quat< Real >::LengthManhattan() const
	{
		return abs(x) + abs(y) + abs(z) + abs(w);
	}

	template< typename Real >
	quat< Real > quat< Real >::Normalize() const
	{
		Real len = LengthSquared();

		if( len > 0 )
			return *this / sqrt( len );

		return quat< Real >( 0, 0, 0, 0 );
	}

	template< typename Real >
	quat< Real >& quat< Real >::AxisAngle( Real _x, Real _y, Real _z, Real _degrees )
	{
		Real a = (Real)DEGREES_TO_RADIANS * _degrees * (Real)0.5;
		Real r = ::sin((Real)a);

		x = _x * r;
		y = _y * r;
		z = _z * r;
		w = ::cos((Real)a);

		return *this;
	}

	template< typename Real >
	quat< Real >& quat< Real >::AxisAngle()
	{
		return AxisAngle( x, y, z, w );
	}

	template< typename Real >
	quat< Real >& quat< Real >::Euler( Real _pitch, Real _yaw, Real _roll )
	{
		Real DR = (Real)DEGREES_TO_RADIANS / (Real)2.0;

		Real ex = DR * _pitch;
		Real ey = DR * _yaw;
		Real ez = DR * _roll;

		Real cx = cos(ex);
		Real cy = cos(ey);
		Real cz = cos(ez);

		Real sx = sin(ex);
		Real sy = sin(ey);
		Real sz = sin(ez);

		Real cycz = cy * cz;
		Real sysz = sy * sz;
		Real cysz = cy * sz;
		Real sycz = sy * cz;

		x = sx * cycz - cx * sysz;
		y = cx * sycz + sx * cysz;
		z = cx * cysz - sx * sycz;
		w = cx * cycz + sx * sysz;

		return *this;
	}

	template< typename Real >
	quat< Real >& quat< Real >::Euler()
	{
		return Euler( x, y, z );
	}

	// Based on code from Hamouras @ 
	// www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/hamourus.htm
	//
	template< typename Real >
	mat4< Real > quat< Real >::Matrix()
	{
		mat4< Real > mat;

		Real x_x = x * x;
		Real y_y = y * y;
		Real z_z = z * z;
		Real w_w = w * w;

		Real inv = (Real)1 / (x_x + y_y + z_z + w_w);
 
		mat[0]  = ( x_x - y_y - z_z + w_w) * inv;
		mat[5]  = (-x_x + y_y - z_z + w_w) * inv;
		mat[10] = (-x_x - y_y + z_z + w_w) * inv;
 
		Real tmp1 = x * y;
		Real tmp2 = z * w;
		mat[1] = (Real)2 * (tmp1 + tmp2) * inv;
		mat[4] = (Real)2 * (tmp1 - tmp2) * inv;
 
		tmp1 = x * z;
		tmp2 = y * w;
		mat[2] = (Real)2 * (tmp1 - tmp2) * inv;
		mat[8] = (Real)2 * (tmp1 + tmp2) * inv;

		tmp1 = y * z;
		tmp2 = x * w;
		mat[6] = (Real)2 * (tmp1 + tmp2) * inv;
		mat[9] = (Real)2 * (tmp1 - tmp2) * inv;

		mat[15] = (Real)1;

		return mat;
	}
}
