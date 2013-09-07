#include "WMatrix4f.h"

namespace Sentinel { namespace Math
{
	DEFINE_REF( Matrix4f );

	WMatrix4f::WMatrix4f()
	{
		mRef = new Matrix4f();
	}

	WMatrix4f::WMatrix4f( const Matrix4f% mat )
	{
		mRef = new Matrix4f( mat );
	}

	WMatrix4f::WMatrix4f( const WMatrix4f% mat )
	{
		mRef = new Matrix4f( *mat.mRef );
	}

	WMatrix4f::WMatrix4f( WQuatf^ q )
	{
		mRef = new Matrix4f( q );
	}

	////////////////////////////////

	float WMatrix4f::default::get( int index )
	{
		return (*mRef)[ index ];
	}

	void WMatrix4f::default::set( int index, float value )
	{
		(*mRef)[ index ] = value;
	}

	////////////////////////////////

	DEFINE_OP_DEREF( Matrix4f );

	WMatrix4f^ WMatrix4f::operator + ( WMatrix4f^ mat0, WMatrix4f^ mat1 )
	{
		return mat0->Add( mat1 );
	}

	WMatrix4f^ WMatrix4f::Add( WMatrix4f^ mat )
	{
		return gcnew WMatrix4f( mRef->Add( mat ));
	}

	WMatrix4f^ WMatrix4f::operator * ( WMatrix4f^ mat0, WMatrix4f^ mat1 )
	{
		return mat0->Mul( mat1 );
	}

	WMatrix4f^ WMatrix4f::Mul( WMatrix4f^ mat )
	{
		return gcnew WMatrix4f( mRef->Mul( mat ));
	}

	void WMatrix4f::Zero()
	{
		mRef->Zero();
	}

	void WMatrix4f::Identity()
	{
		mRef->Identity();
	}

	void WMatrix4f::Translate( WVector3f^ v )
	{
		mRef->Translate( v );
	}

	void WMatrix4f::Rotate( WQuatf^ q )
	{
		mRef->Rotate( q );
	}

	void WMatrix4f::Rotate( WVector3f^ v )
	{
		mRef->Rotate( v );
	}

	void WMatrix4f::RotateX( float degrees )
	{
		mRef->RotateX( degrees );
	}

	void WMatrix4f::RotateY( float degrees )
	{
		mRef->RotateY( degrees );
	}

	void WMatrix4f::RotateZ( float degrees )
	{
		mRef->RotateZ( degrees );
	}
		
	void WMatrix4f::Scale( float _scale )
	{
		mRef->Scale( _scale );
	}

	void WMatrix4f::Scale( WVector3f^ _scale )
	{
		mRef->Scale( _scale );
	}
		
	void WMatrix4f::LookAtView( WVector3f^ pos, WVector3f^ lookAt, WVector3f^ up )
	{
		mRef->LookAtView( pos, lookAt, up );
	}

	void WMatrix4f::BillboardAxis( WVector3f^ posBB, WVector3f^ posCamera, WVector3f^ up )
	{
		mRef->BillboardAxis( posBB, posCamera, up );
	}

	void WMatrix4f::BillboardWorld( WVector3f^ posBB, WVector3f^ posCamera, WVector3f^ up )
	{
		mRef->BillboardWorld( posBB, posCamera, up );
	}
		
	float WMatrix4f::Det()
	{
		return mRef->Det();
	}

	void WMatrix4f::Transpose()
	{
		mRef->Transpose();
	}

	void WMatrix4f::InverseTrans()
	{
		mRef->InverseTrans();
	}

	WMatrix4f^ WMatrix4f::InverseView()
	{
		return gcnew WMatrix4f( mRef->InverseView() );
	}

	WMatrix4f^ WMatrix4f::Inverse()
	{
		return gcnew WMatrix4f( mRef->Inverse() );
	}

	void WMatrix4f::ProjectionOrthographic( float windowWidth, float windowHeight )
	{
		mRef->ProjectionOrthographic( windowWidth, windowHeight );
	}

	void WMatrix4f::ProjectionPerspective( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV )
	{
		mRef->ProjectionPerspective( windowWidth, windowHeight, nearZ, farZ, FOV );
	}

	WVector3f^ WMatrix4f::Transform( WVector3f^ v )
	{
		return gcnew WVector3f( mRef->Transform( v ));
	}

	WVector3f^ WMatrix4f::Transform( WVector4f^ v )
	{
		return gcnew WVector3f( mRef->Transform( v ));
	}

	WVector3f^ WMatrix4f::Forward()
	{
		return gcnew WVector3f( mRef->Forward() );
	}

	WVector3f^ WMatrix4f::Up()
	{
		return gcnew WVector3f( mRef->Up() );
	}

	WVector3f^ WMatrix4f::Right()
	{
		return gcnew WVector3f( mRef->Right() );
	}

	////////////////////////////////

	DEFINE_CLASS_REF( Matrix4f );
}}
