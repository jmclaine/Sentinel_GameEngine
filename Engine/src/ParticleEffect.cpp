#pragma once

#include "ParticleEffect.h"
#include "Particle.h"
#include "RandomValue.h"
#include "Timing.h"
#include "Archive.h"
#include "MathUtil.h"

namespace Sentinel
{
	////////////////////////////////////////////////////////////////////////////
	// Base particle effect.
	//
	ParticleEffect::ParticleEffect()
	{ }

	ParticleEffect::ParticleEffect(float startTime) :
		mStartTime(startTime)
	{ }

	void ParticleEffect::Startup(Particle& particle)
	{ }

	void ParticleEffect::Update(Particle& particle)
	{ }

	void ParticleEffect::Save(Archive& archive)
	{
		archive.Write(&mStartTime);
	}

	void ParticleEffect::Load(Archive& archive)
	{
		archive.Read(&mStartTime);
	}

	////////////////////////////////////////////////////////////////////////////
	// Texture.
	// NormalParticle only.
	//
	DEFINE_SERIAL_REGISTER(TextureEffect);
	DEFINE_SERIAL_CLONE(TextureEffect);

	TextureEffect::TextureEffect()
	{ }

	TextureEffect::TextureEffect(float startTime, UINT frame) :
		ParticleEffect(startTime),
		mFrame(frame)
	{ }

	void TextureEffect::Startup(Particle& particle)
	{
		static_cast<SpriteParticle&>(particle).mFrame = mFrame;
	}

	void TextureEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mFrame);
	}

	void TextureEffect::Load(Archive& archive)
	{
		ParticleEffect::Load(archive);

		archive.Read(&mFrame);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(RandomTextureEffect);
	DEFINE_SERIAL_CLONE(RandomTextureEffect);

	RandomTextureEffect::RandomTextureEffect()
	{ }

	RandomTextureEffect::RandomTextureEffect(float startTime, UINT minFrame, UINT maxFrame) :
		TextureEffect(startTime, minFrame),
		mMaxFrame(maxFrame)
	{ }

	void RandomTextureEffect::Startup(Particle& particle)
	{
		static_cast<SpriteParticle&>(particle).mFrame = RandomValue(mFrame, mMaxFrame);
	}

	void RandomTextureEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mFrame);
		archive.Write(&mMaxFrame);
	}

	void RandomTextureEffect::Load(Archive& archive)
	{
		TextureEffect::Load(archive);

		archive.Read(&mMaxFrame);
	}

	////////////////////////////////////////////////////////////////////////////
	// Color.
	// NormalParticle only.
	//
	DEFINE_SERIAL_REGISTER(ColorEffect);
	DEFINE_SERIAL_CLONE(ColorEffect);

	ColorEffect::ColorEffect()
	{ }

	ColorEffect::ColorEffect(float startTime, const ColorRGBA& color) :
		ParticleEffect(startTime),
		mColor(color)
	{ }

	void ColorEffect::Startup(Particle& particle)
	{
		static_cast<SpriteParticle&>(particle).mColor = mColor;
	}

	void ColorEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mColor);
	}

	void ColorEffect::Load(Archive& archive)
	{
		ParticleEffect::Load(archive);

		archive.Read(&mColor);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(RandomColorEffect);
	DEFINE_SERIAL_CLONE(RandomColorEffect);

	RandomColorEffect::RandomColorEffect()
	{ }

	RandomColorEffect::RandomColorEffect(float startTime, const ColorRGBA& minColor, const ColorRGBA& maxColor) :
		ColorEffect(startTime, minColor),
		mMaxColor(maxColor)
	{ }

	void RandomColorEffect::Startup(Particle& particle)
	{
		static_cast<SpriteParticle&>(particle).mColor = RandomValue(mColor, mMaxColor);
	}

	void RandomColorEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mColor);
		archive.Write(&mMaxColor);
	}

	void RandomColorEffect::Load(Archive& archive)
	{
		ColorEffect::Load(archive);

		archive.Read(&mMaxColor);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(FadeToColorEffect);
	DEFINE_SERIAL_CLONE(FadeToColorEffect);

	FadeToColorEffect::FadeToColorEffect()
	{ }

	FadeToColorEffect::FadeToColorEffect(float startTime, float endTime, const ColorRGBA& color) :
		ColorEffect(startTime, color),
		mEndTime(endTime)
	{
		_ASSERT(endTime > 0);
		_ASSERT(startTime < endTime);
	}

	void FadeToColorEffect::Startup(Particle& particle)
	{
		_ASSERT(mEndTime > 0);
		_ASSERT(mStartTime < mEndTime);
	}

	void FadeToColorEffect::Update(Particle& particle)
	{
		if (particle.mElapsedTime < mEndTime + Timing::DESIRED_FRAME_RATE)
			static_cast<SpriteParticle&>(particle).mColor = LERP(static_cast<SpriteParticle&>(particle).mColor, mColor, (particle.mElapsedTime / mEndTime));
	}

	void FadeToColorEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mColor);
		archive.Write(&mEndTime);
	}

	void FadeToColorEffect::Load(Archive& archive)
	{
		ColorEffect::Load(archive);

		archive.Read(&mEndTime);
	}

	////////////////////////////////////////////////////////////////////////////
	// Position.
	//
	DEFINE_SERIAL_REGISTER(PositionEffect);
	DEFINE_SERIAL_CLONE(PositionEffect);

	PositionEffect::PositionEffect()
	{ }

	PositionEffect::PositionEffect(float startTime, const Vector3& position) :
		ParticleEffect(startTime),
		mPosition(position)
	{ }

	void PositionEffect::Startup(Particle& particle)
	{
		particle.mPosition = mPosition;
	}

	void PositionEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mPosition);
	}

	void PositionEffect::Load(Archive& archive)
	{
		ParticleEffect::Load(archive);

		archive.Read(&mPosition);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(AreaPositionEffect);
	DEFINE_SERIAL_CLONE(AreaPositionEffect);

	AreaPositionEffect::AreaPositionEffect()
	{ }

	AreaPositionEffect::AreaPositionEffect(float startTime, const Vector3& minPosition, const Vector3& maxPosition) :
		PositionEffect(startTime, minPosition),
		mMaxPosition(maxPosition)
	{ }

	void AreaPositionEffect::Startup(Particle& particle)
	{
		particle.mPosition = RandomValue(mPosition, mMaxPosition);
	}

	void AreaPositionEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mPosition);
		archive.Write(&mMaxPosition);
	}

	void AreaPositionEffect::Load(Archive& archive)
	{
		PositionEffect::Load(archive);

		archive.Read(&mMaxPosition);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(SpherePositionEffect);
	DEFINE_SERIAL_CLONE(SpherePositionEffect);

	SpherePositionEffect::SpherePositionEffect()
	{ }

	SpherePositionEffect::SpherePositionEffect(float startTime, const Vector3& position, float radius) :
		PositionEffect(startTime, position),
		mRadius(radius)
	{ }

	void SpherePositionEffect::Startup(Particle& particle)
	{
		particle.mPosition = mPosition + RandomValue(Vector3(-mRadius, -mRadius, -mRadius), Vector3(mRadius, mRadius, mRadius));
	}

	void SpherePositionEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mPosition);
		archive.Write(&mRadius);
	}

	void SpherePositionEffect::Load(Archive& archive)
	{
		PositionEffect::Load(archive);

		archive.Read(&mRadius);
	}

	////////////////////////////////////////////////////////////////////////////
	// Rotation.
	//
	DEFINE_SERIAL_REGISTER(RotationEffect);
	DEFINE_SERIAL_CLONE(RotationEffect);

	RotationEffect::RotationEffect()
	{ }

	RotationEffect::RotationEffect(float startTime, const Vector3& rotation) :
		ParticleEffect(startTime),
		mRotation(rotation)
	{ }

	void RotationEffect::Startup(Particle& particle)
	{
		particle.mRotation = mRotation;
	}

	void RotationEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mRotation);
	}

	void RotationEffect::Load(Archive& archive)
	{
		ParticleEffect::Load(archive);

		archive.Read(&mRotation);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(RandomRotationEffect);
	DEFINE_SERIAL_CLONE(RandomRotationEffect);

	RandomRotationEffect::RandomRotationEffect()
	{ }

	RandomRotationEffect::RandomRotationEffect(float startTime, const Vector3& minRotation, const Vector3& maxRotation) :
		RotationEffect(startTime, minRotation),
		mMaxRotation(maxRotation)
	{ }

	void RandomRotationEffect::Startup(Particle& particle)
	{
		particle.mRotation = RandomValue(mRotation, mMaxRotation);
	}

	void RandomRotationEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mRotation);
		archive.Write(&mMaxRotation);
	}

	void RandomRotationEffect::Load(Archive& archive)
	{
		RotationEffect::Load(archive);

		archive.Read(&mMaxRotation);
	}

	////////////////////////////////////////////////////////////////////////////
	// Scale.
	//
	DEFINE_SERIAL_REGISTER(ScaleEffect);
	DEFINE_SERIAL_CLONE(ScaleEffect);

	ScaleEffect::ScaleEffect()
	{ }

	ScaleEffect::ScaleEffect(float startTime, const Vector3& scale) :
		ParticleEffect(startTime),
		mScale(scale)
	{ }

	void ScaleEffect::Startup(Particle& particle)
	{
		particle.mScale = mScale;
	}

	void ScaleEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mScale);
	}

	void ScaleEffect::Load(Archive& archive)
	{
		ParticleEffect::Load(archive);

		archive.Read(&mScale);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(RandomScaleEffect);
	DEFINE_SERIAL_CLONE(RandomScaleEffect);

	RandomScaleEffect::RandomScaleEffect()
	{ }

	RandomScaleEffect::RandomScaleEffect(float startTime, const Vector3& minScale, const Vector3& maxScale) :
		ScaleEffect(startTime, minScale),
		mMaxScale(maxScale)
	{ }

	void RandomScaleEffect::Startup(Particle& particle)
	{
		particle.mScale = RandomValue(mScale, mMaxScale);
	}

	void RandomScaleEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mScale);
		archive.Write(&mMaxScale);
	}

	void RandomScaleEffect::Load(Archive& archive)
	{
		ScaleEffect::Load(archive);

		archive.Read(&mMaxScale);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(FadeToScaleEffect);
	DEFINE_SERIAL_CLONE(FadeToScaleEffect);

	FadeToScaleEffect::FadeToScaleEffect()
	{ }

	FadeToScaleEffect::FadeToScaleEffect(float startTime, float endTime, float scale) :
		ScaleEffect(startTime, Vector3(scale, scale, scale)),
		mEndTime(endTime)
	{
		_ASSERT(endTime > 0);
		_ASSERT(startTime < endTime);
	}

	FadeToScaleEffect::FadeToScaleEffect(float startTime, float endTime, const Vector3& scale) :
		ScaleEffect(startTime, scale),
		mEndTime(endTime)
	{
		_ASSERT(endTime > 0);
		_ASSERT(startTime < endTime);
	}

	void FadeToScaleEffect::Startup(Particle& particle)
	{
		_ASSERT(mEndTime > 0);
		_ASSERT(mStartTime < mEndTime);
	}

	void FadeToScaleEffect::Update(Particle& particle)
	{
		if (particle.mElapsedTime < mEndTime + Timing::DESIRED_FRAME_RATE)
			particle.mScale = LERP(particle.mScale, mScale, (particle.mElapsedTime / mEndTime));
	}

	void FadeToScaleEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mScale);
		archive.Write(&mEndTime);
	}

	void FadeToScaleEffect::Load(Archive& archive)
	{
		ScaleEffect::Load(archive);

		archive.Read(&mEndTime);
	}

	////////////////////////////////////////////////////////////////////////////
	// Velocity.
	//
	DEFINE_SERIAL_REGISTER(VelocityEffect);
	DEFINE_SERIAL_CLONE(VelocityEffect);

	VelocityEffect::VelocityEffect()
	{ }

	VelocityEffect::VelocityEffect(float startTime, const Vector3& velocity) :
		ParticleEffect(startTime),
		mVelocity(velocity)
	{ }

	void VelocityEffect::Startup(Particle& particle)
	{
		particle.mVelocity = mVelocity;
	}

	void VelocityEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mVelocity);
	}

	void VelocityEffect::Load(Archive& archive)
	{
		ParticleEffect::Load(archive);

		archive.Read(&mVelocity);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(RandomVelocityEffect);
	DEFINE_SERIAL_CLONE(RandomVelocityEffect);

	RandomVelocityEffect::RandomVelocityEffect()
	{ }

	RandomVelocityEffect::RandomVelocityEffect(float startTime, const Vector3& minVelocity, const Vector3& maxVelocity) :
		VelocityEffect(startTime, minVelocity),
		mMaxVelocity(maxVelocity)
	{ }

	void RandomVelocityEffect::Startup(Particle& particle)
	{
		particle.mVelocity = RandomValue(mVelocity, mMaxVelocity);
	}

	void RandomVelocityEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mVelocity);
		archive.Write(&mMaxVelocity);
	}

	void RandomVelocityEffect::Load(Archive& archive)
	{
		VelocityEffect::Load(archive);

		archive.Read(&mMaxVelocity);
	}

	////////////////////////////////////////////////////////////////////////////
	// Angular Velocity.
	//
	DEFINE_SERIAL_REGISTER(AngularVelocityEffect);
	DEFINE_SERIAL_CLONE(AngularVelocityEffect);

	AngularVelocityEffect::AngularVelocityEffect()
	{ }

	AngularVelocityEffect::AngularVelocityEffect(float startTime, const Vector3& angularVelocity) :
		ParticleEffect(startTime),
		mAngularVelocity(angularVelocity)
	{ }

	void AngularVelocityEffect::Startup(Particle& particle)
	{
		particle.mAngularVelocity = mAngularVelocity;
	}

	void AngularVelocityEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mAngularVelocity);
	}

	void AngularVelocityEffect::Load(Archive& archive)
	{
		ParticleEffect::Load(archive);

		archive.Read(&mAngularVelocity);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(RandomAngularVelocityEffect);
	DEFINE_SERIAL_CLONE(RandomAngularVelocityEffect);

	RandomAngularVelocityEffect::RandomAngularVelocityEffect()
	{ }

	RandomAngularVelocityEffect::RandomAngularVelocityEffect(float startTime, const Vector3& minAngularVelocity, const Vector3& maxAngularVelocity) :
		AngularVelocityEffect(startTime, minAngularVelocity),
		mMaxAngularVelocity(maxAngularVelocity)
	{ }

	void RandomAngularVelocityEffect::Startup(Particle& particle)
	{
		particle.mAngularVelocity = RandomValue(mAngularVelocity, mMaxAngularVelocity);
	}

	void RandomAngularVelocityEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mAngularVelocity);
		archive.Write(&mMaxAngularVelocity);
	}

	void RandomAngularVelocityEffect::Load(Archive& archive)
	{
		AngularVelocityEffect::Load(archive);

		archive.Read(&mMaxAngularVelocity);
	}

	////////////////////////////////////////////////////////////////////////////
	// Acceleration.
	//
	DEFINE_SERIAL_REGISTER(AccelEffect);
	DEFINE_SERIAL_CLONE(AccelEffect);

	AccelEffect::AccelEffect()
	{ }

	AccelEffect::AccelEffect(float startTime, const Vector3& accel) :
		ParticleEffect(startTime),
		mAccel(accel)
	{ }

	void AccelEffect::Startup(Particle& particle)
	{
		particle.mAccel = mAccel;
	}

	void AccelEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mAccel);
	}

	void AccelEffect::Load(Archive& archive)
	{
		ParticleEffect::Load(archive);

		archive.Read(&mAccel);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(RandomAccelEffect);
	DEFINE_SERIAL_CLONE(RandomAccelEffect);

	RandomAccelEffect::RandomAccelEffect()
	{ }

	RandomAccelEffect::RandomAccelEffect(float startTime, const Vector3& minAccel, const Vector3& maxAccel) :
		AccelEffect(startTime, minAccel),
		mMaxAccel(maxAccel)
	{ }

	void RandomAccelEffect::Startup(Particle& particle)
	{
		particle.mAccel = RandomValue(mAccel, mMaxAccel);
	}

	void RandomAccelEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mAccel);
		archive.Write(&mMaxAccel);
	}

	void RandomAccelEffect::Load(Archive& archive)
	{
		AccelEffect::Load(archive);

		archive.Read(&mMaxAccel);
	}

	////////////////////////////////////////////////////////////////////////////
	// Angular Acceleration.
	//
	DEFINE_SERIAL_REGISTER(AngularAccelEffect);
	DEFINE_SERIAL_CLONE(AngularAccelEffect);

	AngularAccelEffect::AngularAccelEffect()
	{ }

	AngularAccelEffect::AngularAccelEffect(float startTime, const Vector3& angularAccel) :
		ParticleEffect(startTime),
		mAngularAccel(angularAccel)
	{ }

	void AngularAccelEffect::Startup(Particle& particle)
	{
		particle.mAngularAccel = mAngularAccel;
	}

	void AngularAccelEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mAngularAccel);
	}

	void AngularAccelEffect::Load(Archive& archive)
	{
		ParticleEffect::Load(archive);

		archive.Read(&mAngularAccel);
	}

	////////////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_REGISTER(RandomAngularAccelEffect);
	DEFINE_SERIAL_CLONE(RandomAngularAccelEffect);

	RandomAngularAccelEffect::RandomAngularAccelEffect()
	{ }

	RandomAngularAccelEffect::RandomAngularAccelEffect(float startTime, const Vector3& minAngularAccel, const Vector3& maxAngularAccel) :
		AngularAccelEffect(startTime, minAngularAccel),
		mMaxAngularAccel(maxAngularAccel)
	{ }

	void RandomAngularAccelEffect::Startup(Particle& particle)
	{
		particle.mAngularAccel = RandomValue(mAngularAccel, mMaxAngularAccel);
	}

	void RandomAngularAccelEffect::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		ParticleEffect::Save(archive);

		archive.Write(&mAngularAccel);
		archive.Write(&mMaxAngularAccel);
	}

	void RandomAngularAccelEffect::Load(Archive& archive)
	{
		AngularAccelEffect::Load(archive);

		archive.Read(&mMaxAngularAccel);
	}
}
