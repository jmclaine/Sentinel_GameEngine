#include "Component/SpriteParticleEmitter.h"
#include "Component/Transform.h"
#include "ParticleEmitter.h"
#include "Archive.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Renderer.h"
#include "Timing.h"
#include "RenderManager.h"
#include "MeshBuilder.h"
#include "Material.h"
#include "Mesh.h"
#include "Point.h"
#include "Sprite.h"
#include "Particle.h"
#include "Shader.h"
#include "Buffer.h"
#include "MaterialManager.h"
#include "SpriteManager.h"

namespace Sentinel
{
	namespace Component
	{
		SerialRegister SpriteParticleEmitter::SERIAL_REGISTER("SpriteParticleEmitter", SpriteParticleEmitter::Clone);
		Serializable* SpriteParticleEmitter::Clone() { return new SpriteParticleEmitter(); }

		SpriteParticleEmitter::SpriteParticleEmitter()
		{ }

		SpriteParticleEmitter::SpriteParticleEmitter(UINT maxParticles) :
			ParticleEmitter(maxParticles)
		{ }

		SpriteParticleEmitter::~SpriteParticleEmitter()
		{
			delete[] mParticle;
		}

		/////////////////////////////////

		void SpriteParticleEmitter::SetMaxParticles(UINT maxParticles)
		{
			mMaxParticles = maxParticles;
		}

		/////////////////////////////////

		void SpriteParticleEmitter::Startup()
		{
			_ASSERT(mMaxParticles > 0);

			ParticleEmitter::Startup();

			auto world = mOwner->GetWorld();
			auto material = mMaterial.lock();

			mParticle = new SpriteParticle[mMaxParticles];

			auto shader = material->mShader.lock();

			MeshBuilder builder;
			builder.mLayout = shader->Layout();

			for (UINT x = 0; x < mMaxParticles; ++x)
			{
				builder.mVertex.push_back(MeshBuilder::Vertex(Vector3(0.0f, 0.0f, 0.0f)));
			}

			builder.mPrimitive = PrimitiveFormat::POINTS;

			mMesh = std::unique_ptr<Mesh>(builder.BuildMesh(world->mRenderer, false));

			mMesh.get()->mMaterial = material;
			mMesh.get()->mMatrixWorld.Identity();
		}

		void SpriteParticleEmitter::Update()
		{
			ParticleEmitter::Update();
		}

		void SpriteParticleEmitter::Shutdown()
		{
			ParticleEmitter::Shutdown();
		}

		/////////////////////////////////

		void SpriteParticleEmitter::Draw(Camera* camera)
		{
			const auto transform = camera->GetTransform();

			auto world = mOwner->GetWorld();
			auto renderer = world->mRenderer;
			auto mesh = mMesh.get();
			auto sprite = mSprite.lock();

			mesh->mMatrixWorld = mTransform->GetMatrixWorld();

			BYTE* verts = (BYTE*)mesh->mVertexBuffer->Lock();

			for (UINT x = 0; x < (UINT)mNumParticles; ++x)
			{
				SpriteParticle& particle = mParticle[x];

				*(Quad*)verts = sprite->GetFrame(particle.mFrame);
				verts += sizeof(Quad);

				*(UINT*)verts = particle.mColor.ToUINT();
				verts += sizeof(UINT);

				static Matrix4x4 matrixParticle;
				matrixParticle.World(particle.mPosition, Quaternion(particle.mRotation), particle.mScale);

				static Matrix4x4 matrixBillboard;
				matrixBillboard.BillboardAxis(mMesh->mMatrixWorld.Transform(particle.mPosition), transform->mPosition, Vector3(0, 1, 0));

				*(Matrix4x4*)verts = world->mCurrentCamera->GetMatrixWVP() * mMesh->mMatrixWorld * matrixParticle * matrixBillboard;
				verts += sizeof(Matrix4x4);
			}

			mesh->mVertexBuffer->Unlock();

			mMesh->Draw(renderer, world, camera, mNumParticles);
		}

		/////////////////////////////////

		void SpriteParticleEmitter::Save(Archive& archive)
		{
			SERIAL_REGISTER.Save(archive);

			Drawable::Save(archive);

			auto world = mOwner->GetWorld();

			archive.Write(&world->mMaterialManager->Get(mMaterial));
			archive.Write(&world->mSpriteManager->Get(mSprite));
		}

		void SpriteParticleEmitter::Load(Archive& archive)
		{
			Drawable::Load(archive);

			std::string material;
			archive.Read(&material);

			std::string sprite;
			archive.Read(&sprite);
		}

		///////////////////////////////////

		GameComponent* SpriteParticleEmitter::Copy()
		{
			SpriteParticleEmitter* emitter = new SpriteParticleEmitter();

			Copy(emitter);

			return emitter;
		}

		void SpriteParticleEmitter::Copy(GameComponent* component)
		{
			ParticleEmitter::Copy(component);
		}

		///////////////////////////////////

		Particle& SpriteParticleEmitter::GetParticle(UINT index)
		{
			_ASSERT(index < mMaxParticles);

			return mParticle[index];
		}
	}
}
