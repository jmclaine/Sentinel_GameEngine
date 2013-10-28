#pragma once

#include "Common.h"
#include "ColorRGBA.h"
#include "Vector3f.h"

namespace Sentinel
{
	class Particle;
	
	////////////////////////////////////////////////////////////////////////////
	// Base particle effect.
	//
	class SENTINEL_DLL ParticleEffect
	{
	public:

		float		mStartTime;

	protected:

		ParticleEffect( float startTime );

	public:

		virtual void Startup( Particle* particle );

		virtual void Update( Particle* particle );
	};

	////////////////////////////////////////////////////////////////////////////
	// Texture.
	// NormalParticle only.
	//
	class SENTINEL_DLL TextureEffect : public ParticleEffect
	{
	private:

		UINT		mFrame;
		
	public:

		TextureEffect( float startTime, UINT frame );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};

	////////////////////////////////////////////////////////////////////////////
	// Color.
	// NormalParticle only.
	//
	class SENTINEL_DLL ColorEffect : public ParticleEffect
	{
	protected:

		ColorRGBA	mColor;
		
	public:

		ColorEffect( float startTime, const ColorRGBA& color );

		//////////////////////////////////////

		virtual void Startup( Particle* particle );
	};

	class SENTINEL_DLL RandomColorEffect : public ColorEffect
	{
	private:

		// uses mColor as min
		ColorRGBA	mMaxColor;
		
	public:

		RandomColorEffect( float startTime, const ColorRGBA& minColor, const ColorRGBA& maxColor );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};

	class SENTINEL_DLL FadeToColorEffect : public ColorEffect
	{
	private:

		float			mEndTime;

	public:

		FadeToColorEffect( float startTime, float endTime, const ColorRGBA& color );

		//////////////////////////////////////

		void		Startup( Particle* particle );

		void		Update( Particle* particle );
	};

	////////////////////////////////////////////////////////////////////////////
	// Position.
	//
	class SENTINEL_DLL PositionEffect : public ParticleEffect
	{
	protected:

		Vector3f	mPosition;

	public:

		PositionEffect( float startTime, const Vector3f& position );

		//////////////////////////////////////

		virtual void Startup( Particle* particle );
	};

	class SENTINEL_DLL AreaPositionEffect : public PositionEffect
	{
	private:

		// uses mPosition as min
		Vector3f	mMaxPosition;

	public:

		AreaPositionEffect( float startTime, const Vector3f& minPosition, const Vector3f& maxPosition );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};

	class SENTINEL_DLL SpherePositionEffect : public PositionEffect
	{
	private:

		float		mRadius;

	public:

		SpherePositionEffect( float startTime, const Vector3f& position, float radius );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};

	////////////////////////////////////////////////////////////////////////////
	// Rotation.
	//
	class SENTINEL_DLL RotationEffect : public ParticleEffect
	{
	protected:

		Vector3f	mRotation;

	public:

		RotationEffect( float startTime, const Vector3f& rotation );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};

	class SENTINEL_DLL RandomRotationEffect : public RotationEffect
	{
	private:

		// uses mRotation as min
		Vector3f	mMaxRotation;

	public:

		RandomRotationEffect( float startTime, const Vector3f& minRotation, const Vector3f& maxRotation );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};

	////////////////////////////////////////////////////////////////////////////
	// Scale.
	//
	class SENTINEL_DLL ScaleEffect : public ParticleEffect
	{
	protected:

		Vector3f	mScale;

	public:

		ScaleEffect( float startTime, const Vector3f& scale );

		//////////////////////////////////////

		virtual void Startup( Particle* particle );
	};

	class SENTINEL_DLL RandomScaleEffect : public ScaleEffect
	{
	protected:

		Vector3f	mMaxScale;

	public:

		RandomScaleEffect( float startTime, const Vector3f& minScale, const Vector3f& maxScale );

		//////////////////////////////////////

		virtual void Startup( Particle* particle );
	};

	class SENTINEL_DLL FadeToScaleEffect : public ScaleEffect
	{
	private:

		float		mEndTime;

	public:

		FadeToScaleEffect( float startTime, float endTime, float scale );
		FadeToScaleEffect( float startTime, float endTime, const Vector3f& scale );

		//////////////////////////////////////

		void		Startup( Particle* particle );

		void		Update( Particle* particle );
	};

	////////////////////////////////////////////////////////////////////////////
	// Velocity.
	//
	class SENTINEL_DLL VelocityEffect : public ParticleEffect
	{
	protected:

		Vector3f	mVelocity;

	public:

		VelocityEffect( float startTime, const Vector3f& velocity );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};

	class SENTINEL_DLL RandomVelocityEffect : public VelocityEffect
	{
	private:

		// uses mVelocity as min
		Vector3f	mMaxVelocity;

	public:

		RandomVelocityEffect( float startTime, const Vector3f& minVelocity, const Vector3f& maxVelocity );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};

	////////////////////////////////////////////////////////////////////////////
	// Angular Velocity.
	//
	class SENTINEL_DLL AngularVelocityEffect : public ParticleEffect
	{
	protected:

		Vector3f	mAngularVelocity;

	public:

		AngularVelocityEffect( float startTime, const Vector3f& angularVelocity );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};

	////////////////////////////////////////////////////////////////////////////
	// Acceleration.
	//
	class SENTINEL_DLL AccelEffect : public ParticleEffect
	{
	protected:

		Vector3f	mAccel;

	public:

		AccelEffect( float startTime, const Vector3f& accel );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};

	////////////////////////////////////////////////////////////////////////////
	// Angular Acceleration.
	//
	class SENTINEL_DLL AngularAccelEffect : public ParticleEffect
	{
	private:

		Vector3f	mAngularAccel;

	public:

		AngularAccelEffect( float startTime, const Vector3f& angularAccel );

		//////////////////////////////////////

		void		Startup( Particle* particle );
	};
}
