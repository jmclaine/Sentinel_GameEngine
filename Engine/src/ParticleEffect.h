#pragma once

#include "Sentinel.h"
#include "Serializable.h"
#include "ColorRGBA.h"
#include "Vector3.h"

namespace Sentinel
{
	class Particle;

	////////////////////////////////////////////////////////////////////////////
	// Base particle effect.
	//
	class SENTINEL_DLL ParticleEffect : public Serializable
	{
	public:
		float mStartTime;

	protected:
		ParticleEffect();
		ParticleEffect(float startTime);

	public:
		virtual void Startup(Particle& particle);
		virtual void Update(Particle& particle);

		//////////////////////////////////////

		virtual void Save(Archive& archive);
		virtual void Load(Archive& archive);
	};

	////////////////////////////////////////////////////////////////////////////
	// Texture.
	// NormalParticle only.
	//
	class SENTINEL_DLL TextureEffect : public ParticleEffect
	{
		DECLARE_SERIAL();

	protected:
		UINT mFrame;

		//////////////////////////////////////

		TextureEffect();

	public:
		TextureEffect(float startTime, UINT frame);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	class SENTINEL_DLL RandomTextureEffect : public TextureEffect
	{
		DECLARE_SERIAL();

	protected:
		// uses mFrame as min
		UINT mMaxFrame;

		//////////////////////////////////////

		RandomTextureEffect();

	public:
		RandomTextureEffect(float startTime, UINT minFrame, UINT maxFrame);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	////////////////////////////////////////////////////////////////////////////
	// Color.
	// NormalParticle only.
	//
	class SENTINEL_DLL ColorEffect : public ParticleEffect
	{
		DECLARE_SERIAL();

	protected:
		ColorRGBA mColor;

		//////////////////////////////////////

		ColorEffect();

	public:
		ColorEffect(float startTime, const ColorRGBA& color);

		//////////////////////////////////////

		virtual void Startup(Particle& particle);
	};

	class SENTINEL_DLL RandomColorEffect : public ColorEffect
	{
		DECLARE_SERIAL();

	protected:
		// uses mColor as min
		ColorRGBA mMaxColor;

		//////////////////////////////////////

		RandomColorEffect();

	public:
		RandomColorEffect(float startTime, const ColorRGBA& minColor, const ColorRGBA& maxColor);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	class SENTINEL_DLL FadeToColorEffect : public ColorEffect
	{
		DECLARE_SERIAL();

	protected:
		float mEndTime;

		//////////////////////////////////////

		FadeToColorEffect();

	public:
		FadeToColorEffect(float startTime, float endTime, const ColorRGBA& color);

		//////////////////////////////////////

		void Startup(Particle& particle);
		void Update(Particle& particle);
	};

	////////////////////////////////////////////////////////////////////////////
	// Position.
	//
	class SENTINEL_DLL PositionEffect : public ParticleEffect
	{
		DECLARE_SERIAL();

	protected:
		Vector3 mPosition;

		//////////////////////////////////////

		PositionEffect();

	public:
		PositionEffect(float startTime, const Vector3& position);

		//////////////////////////////////////

		virtual void Startup(Particle& particle);
	};

	class SENTINEL_DLL AreaPositionEffect : public PositionEffect
	{
		DECLARE_SERIAL();

	protected:
		// uses mPosition as min
		Vector3 mMaxPosition;

		//////////////////////////////////////

		AreaPositionEffect();

	public:
		AreaPositionEffect(float startTime, const Vector3& minPosition, const Vector3& maxPosition);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	class SENTINEL_DLL SpherePositionEffect : public PositionEffect
	{
		DECLARE_SERIAL();

	protected:
		float mRadius;

		//////////////////////////////////////

		SpherePositionEffect();

	public:
		SpherePositionEffect(float startTime, const Vector3& position, float radius);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	////////////////////////////////////////////////////////////////////////////
	// Rotation.
	//
	class SENTINEL_DLL RotationEffect : public ParticleEffect
	{
		DECLARE_SERIAL();

	protected:
		Vector3 mRotation;

		//////////////////////////////////////

		RotationEffect();

	public:
		RotationEffect(float startTime, const Vector3& rotation);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	class SENTINEL_DLL RandomRotationEffect : public RotationEffect
	{
		DECLARE_SERIAL();

	protected:
		// uses mRotation as min
		Vector3 mMaxRotation;

		//////////////////////////////////////

		RandomRotationEffect();

	public:
		RandomRotationEffect(float startTime, const Vector3& minRotation, const Vector3& maxRotation);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	////////////////////////////////////////////////////////////////////////////
	// Scale.
	//
	class SENTINEL_DLL ScaleEffect : public ParticleEffect
	{
		DECLARE_SERIAL();

	protected:
		Vector3 mScale;

		//////////////////////////////////////

		ScaleEffect();

	public:
		ScaleEffect(float startTime, const Vector3& scale);

		//////////////////////////////////////

		virtual void Startup(Particle& particle);
	};

	class SENTINEL_DLL RandomScaleEffect : public ScaleEffect
	{
		DECLARE_SERIAL();

	protected:
		Vector3 mMaxScale;

		//////////////////////////////////////

		RandomScaleEffect();

	public:
		RandomScaleEffect(float startTime, const Vector3& minScale, const Vector3& maxScale);

		//////////////////////////////////////

		virtual void Startup(Particle& particle);
	};

	class SENTINEL_DLL FadeToScaleEffect : public ScaleEffect
	{
		DECLARE_SERIAL();

	protected:
		float mEndTime;

		//////////////////////////////////////

		FadeToScaleEffect();

	public:
		FadeToScaleEffect(float startTime, float endTime, float scale);
		FadeToScaleEffect(float startTime, float endTime, const Vector3& scale);

		//////////////////////////////////////

		void Startup(Particle& particle);
		void Update(Particle& particle);
	};

	////////////////////////////////////////////////////////////////////////////
	// Velocity.
	//
	class SENTINEL_DLL VelocityEffect : public ParticleEffect
	{
		DECLARE_SERIAL();

	protected:
		Vector3 mVelocity;

		//////////////////////////////////////

		VelocityEffect();

	public:
		VelocityEffect(float startTime, const Vector3& velocity);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	class SENTINEL_DLL RandomVelocityEffect : public VelocityEffect
	{
		DECLARE_SERIAL();

	protected:
		// uses mVelocity as min
		Vector3 mMaxVelocity;

		//////////////////////////////////////

		RandomVelocityEffect();

	public:
		RandomVelocityEffect(float startTime, const Vector3& minVelocity, const Vector3& maxVelocity);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	////////////////////////////////////////////////////////////////////////////
	// Angular Velocity.
	//
	class SENTINEL_DLL AngularVelocityEffect : public ParticleEffect
	{
		DECLARE_SERIAL();

	protected:
		Vector3 mAngularVelocity;

		//////////////////////////////////////

		AngularVelocityEffect();

	public:
		AngularVelocityEffect(float startTime, const Vector3& angularVelocity);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	class SENTINEL_DLL RandomAngularVelocityEffect : public AngularVelocityEffect
	{
		DECLARE_SERIAL();

	protected:
		// uses mAngularVelocity as min
		Vector3 mMaxAngularVelocity;

		//////////////////////////////////////

		RandomAngularVelocityEffect();

	public:
		RandomAngularVelocityEffect(float startTime, const Vector3& minAngularVelocity, const Vector3& maxAngularVelocity);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	////////////////////////////////////////////////////////////////////////////
	// Acceleration.
	//
	class SENTINEL_DLL AccelEffect : public ParticleEffect
	{
		DECLARE_SERIAL();

	protected:
		Vector3 mAccel;

		//////////////////////////////////////

		AccelEffect();

	public:
		AccelEffect(float startTime, const Vector3& accel);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	class SENTINEL_DLL RandomAccelEffect : public AccelEffect
	{
		DECLARE_SERIAL();

	protected:
		// uses mAccel as min
		Vector3 mMaxAccel;

		//////////////////////////////////////

		RandomAccelEffect();

	public:
		RandomAccelEffect(float startTime, const Vector3& minAccel, const Vector3& maxAccel);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	////////////////////////////////////////////////////////////////////////////
	// Angular Acceleration.
	//
	class SENTINEL_DLL AngularAccelEffect : public ParticleEffect
	{
		DECLARE_SERIAL();

	protected:
		Vector3 mAngularAccel;

		//////////////////////////////////////

		AngularAccelEffect();

	public:
		AngularAccelEffect(float startTime, const Vector3& angularAccel);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};

	class SENTINEL_DLL RandomAngularAccelEffect : public AngularAccelEffect
	{
		DECLARE_SERIAL();

	protected:
		// uses mAngularAccel as min
		Vector3 mMaxAngularAccel;

		//////////////////////////////////////

		RandomAngularAccelEffect();

	public:
		RandomAngularAccelEffect(float startTime, const Vector3& minAngularAccel, const Vector3& maxAngularAccel);

		//////////////////////////////////////

		void Startup(Particle& particle);
	};
}
