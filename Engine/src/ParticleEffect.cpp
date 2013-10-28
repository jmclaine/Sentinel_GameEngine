#pragma once

#include "ParticleEffect.h"
#include "Particle.h"
#include "RandomValue.h"
#include "Util.h"
#include "Timing.h"

namespace Sentinel
{
	////////////////////////////////////////////////////////////////////////////
	// Base particle effect.
	//
	ParticleEffect::ParticleEffect( float startTime ) :
		mStartTime( startTime )
	{}

	void ParticleEffect::Startup( Particle* particle )
	{}

	void ParticleEffect::Update( Particle* particle )
	{}

	////////////////////////////////////////////////////////////////////////////
	// Texture.
	// NormalParticle only.
	//
	TextureEffect::TextureEffect( float startTime, UINT frame ) :
		ParticleEffect( startTime ),
		mFrame( frame )
	{}

	void TextureEffect::Startup( Particle* particle )
	{
		static_cast< NormalParticle* >(particle)->mFrame = mFrame;
	}

	////////////////////////////////////////////////////////////////////////////
	// Color.
	// NormalParticle only.
	//
	ColorEffect::ColorEffect( float startTime, const ColorRGBA& color ) :
		ParticleEffect( startTime ),
		mColor( color )
	{}

	void ColorEffect::Startup( Particle* particle )
	{
		static_cast< NormalParticle* >(particle)->mColor = mColor;
	}

	////////////////////////////////////////////////////////////////////////////

	RandomColorEffect::RandomColorEffect( float startTime, const ColorRGBA& minColor, const ColorRGBA& maxColor ) :
		ColorEffect( startTime, minColor ),
		mMaxColor( maxColor )
	{}

	void RandomColorEffect::Startup( Particle* particle )
	{
		static_cast< NormalParticle* >(particle)->mColor = RandomValue( mColor, mMaxColor );
	}

	////////////////////////////////////////////////////////////////////////////

	FadeToColorEffect::FadeToColorEffect( float startTime, float endTime, const ColorRGBA& color ) :
		ColorEffect( startTime, color ),
		mEndTime( endTime )
	{
		_ASSERT( endTime > 0 );
		_ASSERT( startTime < endTime );
	}

	void FadeToColorEffect::Startup( Particle* particle )
	{}

	void FadeToColorEffect::Update( Particle* particle )
	{
		if( particle->mElapsedTime < mEndTime + Timing::DESIRED_FRAME_RATE )
			static_cast< NormalParticle* >(particle)->mColor = lerp( static_cast< NormalParticle* >(particle)->mColor, mColor, particle->mElapsedTime / mEndTime );
	}

	////////////////////////////////////////////////////////////////////////////
	// Position.
	//
	PositionEffect::PositionEffect( float startTime, const Vector3f& position ) :
		ParticleEffect( startTime ),
		mPosition( position )
	{}

	void PositionEffect::Startup( Particle* particle )
	{
		particle->mPosition = mPosition;
	}

	////////////////////////////////////////////////////////////////////////////

	AreaPositionEffect::AreaPositionEffect( float startTime, const Vector3f& minPosition, const Vector3f& maxPosition ) :
		PositionEffect( startTime, minPosition ),
		mMaxPosition( maxPosition )
	{}

	void AreaPositionEffect::Startup( Particle* particle )
	{
		particle->mPosition = RandomValue( mPosition, mMaxPosition );
	}

	////////////////////////////////////////////////////////////////////////////

	SpherePositionEffect::SpherePositionEffect( float startTime, const Vector3f& position, float radius ) :
		PositionEffect( startTime, position ),
		mRadius( radius )
	{}

	void SpherePositionEffect::Startup( Particle* particle )
	{
		particle->mPosition = mPosition + RandomValue( Vector3f( -mRadius, -mRadius, -mRadius ), Vector3f( mRadius, mRadius, mRadius ));
	}

	////////////////////////////////////////////////////////////////////////////
	// Rotation.
	//
	RotationEffect::RotationEffect( float startTime, const Vector3f& rotation ) :
		ParticleEffect( startTime ),
		mRotation( rotation )
	{}

	void RotationEffect::Startup( Particle* particle )
	{
		particle->mRotation = mRotation;
	}

	////////////////////////////////////////////////////////////////////////////

	RandomRotationEffect::RandomRotationEffect( float startTime, const Vector3f& minRotation, const Vector3f& maxRotation ) :
		RotationEffect( startTime, minRotation ),
		mMaxRotation( maxRotation )
	{}

		
	void RandomRotationEffect::Startup( Particle* particle )
	{
		particle->mRotation = RandomValue( mRotation, mMaxRotation );
	}

	////////////////////////////////////////////////////////////////////////////
	// Scale.
	//
	ScaleEffect::ScaleEffect( float startTime, const Vector3f& scale ) :
		ParticleEffect( startTime ),
		mScale( scale )
	{}

	void ScaleEffect::Startup( Particle* particle )
	{
		particle->mScale = mScale;
	}

	////////////////////////////////////////////////////////////////////////////

	RandomScaleEffect::RandomScaleEffect( float startTime, const Vector3f& minScale, const Vector3f& maxScale ) :
		ScaleEffect( startTime, minScale ),
		mMaxScale( maxScale )
	{}

	void RandomScaleEffect::Startup( Particle* particle )
	{
		particle->mScale = RandomValue( mScale, mMaxScale );
	}

	////////////////////////////////////////////////////////////////////////////

	FadeToScaleEffect::FadeToScaleEffect( float startTime, float endTime, float scale ) :
		ScaleEffect( startTime, Vector3f( scale, scale, scale )),
		mEndTime( endTime )
	{
		_ASSERT( endTime > 0 );
		_ASSERT( startTime < endTime );
	}

	FadeToScaleEffect::FadeToScaleEffect( float startTime, float endTime, const Vector3f& scale ) :
		ScaleEffect( startTime, scale ),
		mEndTime( endTime )
	{
		_ASSERT( endTime > 0 );
		_ASSERT( startTime < endTime );
	}

	void FadeToScaleEffect::Startup( Particle* particle )
	{}

	void FadeToScaleEffect::Update( Particle* particle )
	{
		if( particle->mElapsedTime < mEndTime + Timing::DESIRED_FRAME_RATE )
			particle->mScale = lerp( particle->mScale, mScale, particle->mElapsedTime / mEndTime );
	}

	////////////////////////////////////////////////////////////////////////////
	// Velocity.
	//
	VelocityEffect::VelocityEffect( float startTime, const Vector3f& velocity ) :
		ParticleEffect( startTime ),
		mVelocity( velocity )
	{}

	void VelocityEffect::Startup( Particle* particle )
	{
		particle->mVelocity = mVelocity;
	}

	////////////////////////////////////////////////////////////////////////////

	RandomVelocityEffect::RandomVelocityEffect( float startTime, const Vector3f& minVelocity, const Vector3f& maxVelocity ) :
		VelocityEffect( startTime, minVelocity ),
		mMaxVelocity( maxVelocity )
	{}

	void RandomVelocityEffect::Startup( Particle* particle )
	{
		particle->mVelocity = RandomValue( mVelocity, mMaxVelocity );
	}
	
	////////////////////////////////////////////////////////////////////////////
	// Angular Velocity.
	//
	AngularVelocityEffect::AngularVelocityEffect( float startTime, const Vector3f& angularVelocity ) :
		ParticleEffect( startTime ),
		mAngularVelocity( angularVelocity )
	{}

	void AngularVelocityEffect::Startup( Particle* particle )
	{
		particle->mAngularVelocity = mAngularVelocity;
	}

	////////////////////////////////////////////////////////////////////////////
	// Acceleration.
	//
	AccelEffect::AccelEffect( float startTime, const Vector3f& accel ) :
		ParticleEffect( startTime ),
		mAccel( accel )
	{}

	void AccelEffect::Startup( Particle* particle )
	{
		particle->mAccel = mAccel;
	}

	////////////////////////////////////////////////////////////////////////////
	// Angular Acceleration.
	//
	AngularAccelEffect::AngularAccelEffect( float startTime, const Vector3f& angularAccel ) :
		ParticleEffect( startTime ),
		mAngularAccel( angularAccel )
	{}

	void AngularAccelEffect::Startup( Particle* particle )
	{
		particle->mAngularAccel = mAngularAccel;
	}
}
