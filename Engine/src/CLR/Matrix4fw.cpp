#include "Matrix4fw.h"

namespace Sentinel { namespace Math
{
	Matrix4fw::Matrix4fw()
	{
		mRef = new Matrix4f();
	}

	Matrix4fw::Matrix4fw( const Matrix4f% mat )
	{
		mRef = new Matrix4f( mat );
	}

	Matrix4fw::Matrix4fw( const Matrix4fw% mat )
	{
		mRef = new Matrix4f( *mat.mRef );
	}

	Matrix4fw::~Matrix4fw()
	{
		delete mRef;
	}

	const Matrix4f* Matrix4fw::GetRef()
	{
		return mRef;
	}
		
	float Matrix4fw::Get( int index )
	{
		return mRef->Get( index );
	}

	void Matrix4fw::Set( int index, float value )
	{
		mRef->Set( index, value );
	}

	Matrix4fw^ Matrix4fw::operator + ( Matrix4fw^ mat0, Matrix4fw^ mat1 )
	{
		return mat0->Add( mat1 );
	}

	Matrix4fw^ Matrix4fw::Add( const Matrix4fw^ mat )
	{
		return gcnew Matrix4fw( mRef->Add( *mat->mRef ));
	}

	Matrix4fw^ Matrix4fw::operator * ( Matrix4fw^ mat0, Matrix4fw^ mat1 )
	{
		return mat0->Mul( mat1 );
	}

	Matrix4fw^ Matrix4fw::Mul( const Matrix4fw^ mat )
	{
		return gcnew Matrix4fw( mRef->Mul( *mat->mRef ));
	}

	void Matrix4fw::Zero()
	{
		mRef->Zero();
	}

	void Matrix4fw::Identity()
	{
		mRef->Identity();
	}

	void Matrix4fw::Translate( Vector3fw^ v )
	{
		mRef->Translate( *v->GetRef() );
	}

	void Matrix4fw::Rotate( Quatfw^ q )
	{
		mRef->Rotate( *q->GetRef() );
	}

	void Matrix4fw::Rotate( Vector3fw^ v )
	{
		mRef->Rotate( *v->GetRef() );
	}

	void Matrix4fw::RotateX( float degrees )
	{
		mRef->RotateX( degrees );
	}

	void Matrix4fw::RotateY( float degrees )
	{
		mRef->RotateY( degrees );
	}

	void Matrix4fw::RotateZ( float degrees )
	{
		mRef->RotateZ( degrees );
	}
		
	void Matrix4fw::Scale( float _scale )
	{
		mRef->Scale( _scale );
	}

	void Matrix4fw::Scale( Vector3fw^ _scale )
	{
		mRef->Scale( *_scale->GetRef() );
	}
		
	void Matrix4fw::LookAtView( Vector3fw^ pos, Vector3fw^ lookAt, Vector3fw^ up )
	{
		mRef->LookAtView( *pos->GetRef(), *lookAt->GetRef(), *up->GetRef() );
	}

	void Matrix4fw::BillboardAxis( Vector3fw^ posBB, Vector3fw^ posCamera, Vector3fw^ up )
	{
		mRef->BillboardAxis( *posBB->GetRef(), *posCamera->GetRef(), *up->GetRef() );
	}

	void Matrix4fw::BillboardWorld( Vector3fw^ posBB, Vector3fw^ posCamera, Vector3fw^ up )
	{
		mRef->BillboardWorld( *posBB->GetRef(), *posCamera->GetRef(), *up->GetRef() );
	}
		
	float Matrix4fw::Det()
	{
		return mRef->Det();
	}

	void Matrix4fw::Transpose()
	{
		mRef->Transpose();
	}

	void Matrix4fw::InverseTrans()
	{
		mRef->InverseTrans();
	}

	Matrix4fw^ Matrix4fw::InverseView()
	{
		return gcnew Matrix4fw( mRef->InverseView() );
	}

	Matrix4fw^ Matrix4fw::Inverse()
	{
		return gcnew Matrix4fw( mRef->Inverse() );
	}

	void Matrix4fw::ProjectionOrthographic( float windowWidth, float windowHeight )
	{
		mRef->ProjectionOrthographic( windowWidth, windowHeight );
	}

	void Matrix4fw::ProjectionPerspective( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV )
	{
		mRef->ProjectionPerspective( windowWidth, windowHeight, nearZ, farZ, FOV );
	}

	Vector3fw^ Matrix4fw::Transform( Vector3fw^ v )
	{
		return gcnew Vector3fw( mRef->Transform( *v->GetRef() ));
	}

	Vector3fw^ Matrix4fw::Forward()
	{
		return gcnew Vector3fw( mRef->Forward() );
	}

	Vector3fw^ Matrix4fw::Up()
	{
		return gcnew Vector3fw( mRef->Up() );
	}

	Vector3fw^ Matrix4fw::Right()
	{
		return gcnew Vector3fw( mRef->Right() );
	}
}}
