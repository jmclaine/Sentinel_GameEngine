#include "Mesh.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/PointLight.h"
#include "Archive.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "Timing.h"
#include "VertexLayout.h"
#include "Material.h"

namespace Sentinel
{
	Mesh::Mesh()
	{
		mPrimitive = PrimitiveFormat::TRIANGLES;

		mVertexBuffer = nullptr;
		mIndexBuffer = nullptr;

		mMatrixWorld.Identity();
	}

	Mesh::~Mesh()
	{
		SAFE_DELETE(mVertexBuffer);
		SAFE_DELETE(mIndexBuffer);
	}

	void Mesh::Draw(Renderer* renderer, GameWorld* world, Component::Camera* camera, UINT count)
	{
		if (count == 0)
			return;

		if (mVertexBuffer != nullptr || mIndexBuffer != nullptr)
		{
			_ASSERT(renderer);

			auto material = mMaterial.lock();
			auto layout = mLayout.lock();

			_ASSERT(layout);
			_ASSERT(material);

			material->Apply(renderer);

			auto shader = renderer->GetShader();

			_ASSERT(shader);

			UINT texCount = 0;
			UINT lightCount = 0;
			UINT uniformIndex = 0;

			const std::vector<ShaderUniform>& uniform = shader->Uniforms();

			for (UINT i = 0; i < uniform.size(); ++i)
			{
				switch (uniform[i])
				{
				case ShaderUniform::WORLD_VIEW_PROJ:
					_ASSERT(camera);

					shader->SetMatrix(uniformIndex, (camera->GetMatrixWVP() * mMatrixWorld).Ptr());
					break;

				case ShaderUniform::WORLD_VIEW:
					_ASSERT(camera);

					shader->SetMatrix(uniformIndex, (camera->GetMatrixView() * mMatrixWorld).Ptr());
					break;

				case ShaderUniform::WORLD:
					shader->SetMatrix(uniformIndex, mMatrixWorld.Ptr());
					break;

				case ShaderUniform::INV_WORLD:
					shader->SetMatrix(uniformIndex, mMatrixWorld.Inverse().Ptr());
					break;

				case ShaderUniform::VIEW:
					_ASSERT(camera);

					shader->SetMatrix(uniformIndex, const_cast<Matrix4x4&>(camera->GetMatrixView()).Ptr());
					break;

				case ShaderUniform::INV_VIEW:
					_ASSERT(camera);

					shader->SetMatrix(uniformIndex, camera->GetMatrixView().Inverse().Ptr());
					break;

				case ShaderUniform::PROJ:
					_ASSERT(camera);

					shader->SetMatrix(uniformIndex, const_cast<Matrix4x4&>(camera->GetMatrixProjection()).Ptr());
					break;

				case ShaderUniform::INV_PROJ:
					_ASSERT(camera);

					shader->SetMatrix(uniformIndex, camera->GetMatrixProjection().Inverse().Ptr());
					break;

				case ShaderUniform::TEXTURE:
					_ASSERT(material.get());
					_ASSERT(material->mTexture[texCount].lock()); // no texture loaded

					shader->SetTexture(uniformIndex, material->mTexture[texCount].lock().get());
					++texCount;
					break;

				case ShaderUniform::AMBIENT:
					shader->SetFloat4(uniformIndex, mMaterial.lock()->mAmbient.Ptr());
					break;

				case ShaderUniform::DIFFUSE:
					shader->SetFloat4(uniformIndex, material->mDiffuse.Ptr());
					break;

				case ShaderUniform::SPECULAR:
					shader->SetFloat4(uniformIndex, material->mSpecular.Ptr());
					break;

				case ShaderUniform::SPEC_COMP:
					shader->SetFloat(uniformIndex, &material->mSpecularComponent);
					break;

				case ShaderUniform::CAMERA_POS:
				{
					_ASSERT(camera);

					Vector3 pos = camera->GetTransform()->mPosition;

					shader->SetFloat3(uniformIndex, pos.Ptr());
				}
				break;

				case ShaderUniform::LIGHT_POS:
				{
					_ASSERT(world);
					_ASSERT(world->GetLight(lightCount));

					Vector3 pos = world->GetLight(lightCount)->GetTransform()->GetMatrixWorld().Transform(Vector3(0, 0, 0));

					shader->SetFloat3(uniformIndex, pos.Ptr());
				}
				break;

				case ShaderUniform::LIGHT_DIR:
					_ASSERT(world);
					_ASSERT(world->GetLight(lightCount));

					shader->SetFloat3(uniformIndex, world->GetLight(lightCount)->GetTransform()->mOrientation.ToEuler().Ptr());
					break;

				case ShaderUniform::LIGHT_COLOR:
					_ASSERT(world);
					_ASSERT(world->GetLight(lightCount));

					shader->SetFloat3(uniformIndex, world->GetLight(lightCount)->mColor.Ptr());
					break;

				case ShaderUniform::LIGHT_ATTN:
					_ASSERT(world);
					_ASSERT(world->GetLight(lightCount));

					shader->SetFloat4(uniformIndex, world->GetLight(lightCount)->mAttenuation.Ptr());
					break;

				case ShaderUniform::LIGHT_TEXTURE_CUBE:
					_ASSERT(world);
					_ASSERT(world->GetLight(lightCount));

					shader->SetTextureCube(uniformIndex, static_cast<Component::PointLight*>(world->GetLight(lightCount))->GetTexture());
					break;

				case ShaderUniform::LIGHT_MATRIX:
					_ASSERT(world);
					_ASSERT(world->GetLight(lightCount));

					//shader->SetMatrix(uniformIndex, static_cast<DirectionalLightComponent*>(world->GetLight(lightCount))->PtrMatrixFinal());
					break;

				case ShaderUniform::LIGHT_CUBE_MATRIX:
					_ASSERT(world);
					_ASSERT(world->GetLight(lightCount));

					shader->SetMatrix(uniformIndex, static_cast<Component::PointLight*>(world->GetLight(lightCount))->PtrMatrixFinal(), 6);
					break;

				case ShaderUniform::DELTA_TIME:
				{
					_ASSERT(world);
					_ASSERT(world->mTiming);

					float DT = world->mTiming->DeltaTime();
					shader->SetFloat(uniformIndex, &DT);
				}
				break;

				default:
					_ASSERT(0);	// Mesh attempted to Draw with invalid Uniform!
					break;
				}

				++uniformIndex;
			}

			renderer->SetVertexBuffer(mVertexBuffer);
			renderer->SetVertexLayout(layout.get());

			if (mIndexBuffer)
			{
				renderer->SetIndexBuffer(mIndexBuffer);
				renderer->DrawIndexed(mPrimitive, (count == UINT_MAX) ? mIndexBuffer->Count() : count, 0, 0);
			}
			else
			{
				switch (mPrimitive)
				{
				case PrimitiveFormat::POINTS:
					renderer->Draw(mPrimitive, (count == UINT_MAX) ? mVertexBuffer->Count() : count, 0);
					break;

				case PrimitiveFormat::LINES:
					renderer->Draw(mPrimitive, ((count == UINT_MAX) ? mVertexBuffer->Count() : count) >> 1, 0);
					break;

				case PrimitiveFormat::TRIANGLES:
					renderer->Draw(mPrimitive, ((count == UINT_MAX) ? mVertexBuffer->Count() : count) / 3, 0);
					break;

				default:
					_ASSERT(0);	// Mesh attempted to Draw with invalid PrimitiveType!
					break;
				}
			}
		}
	}

	void Mesh::CalculateBounds()
	{
		mBounds.Set((BYTE*)mVertexBuffer->Lock(), mVertexBuffer->Count(), mVertexBuffer->Stride());

		mVertexBuffer->Unlock();
	}

	//////////////////////////////

	void Mesh::Save(
		Archive& archive,
		Mesh* mesh,
		Renderer* renderer,
		MaterialManager* materialManager)
	{
		BYTE primitive = (BYTE)mesh->mPrimitive;
		archive.Write(&primitive);

		const auto& layout = mesh->mLayout.lock()->Layout();

		UINT count = (UINT)layout.size();
		archive.Write(&count);

		BYTE data;
		for (const auto& x : layout)
		{
			data = (BYTE)x;
			archive.Write(&data);
		}

		Buffer::Save(archive, mesh->mVertexBuffer);
		Buffer::Save(archive, mesh->mIndexBuffer);

		std::string material = materialManager->Get(mesh->mMaterial);
		archive.Write(&material);
	}

	Mesh* Mesh::Load(
		Archive& archive,
		Renderer* renderer,
		MaterialManager* materialManager)
	{
		Mesh* mesh = new Mesh();

		BYTE primitive;
		archive.Read(&primitive);
		mesh->mPrimitive = (PrimitiveFormat)primitive;

		std::vector<VertexAttribute> layout;

		UINT count;
		archive.Read(&count);

		BYTE data;
		for (UINT x = 0; x < count; ++x)
		{
			archive.Read(&data);
			layout.push_back((VertexAttribute)data);
		}

		mesh->mLayout = std::shared_ptr<VertexLayout>(renderer->CreateVertexLayout(layout));

		mesh->mVertexBuffer = Buffer::Load(archive, renderer);
		mesh->mIndexBuffer = Buffer::Load(archive, renderer);

		std::string material;
		archive.Read(&material);
		mesh->mMaterial = materialManager->Get(material);

		return mesh;
	}
}
