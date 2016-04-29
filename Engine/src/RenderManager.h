#pragma once

#include <map>
#include <vector>

#include "Sentinel.h"

namespace Sentinel
{
	namespace Component
	{
		class Drawable;
		class Camera;
	}

	class SENTINEL_DLL RenderManager
	{
	private:
		// RenderLayer, RenderQueue, ID
		std::map<WORD, std::map<WORD, std::multimap<UINT, Component::Drawable*>>> mDrawables;
		std::vector<Component::Drawable*> mDebugDraw;

	public:
		RenderManager();
		RenderManager(const RenderManager&) = delete;
		RenderManager& operator = (const RenderManager&) = delete;

		////////////////////////////////////

		void Clear();

		// set queue manually for now
		// model may contain multiple meshes with different materials

		void Add(Component::Drawable* drawable, WORD queue);
		void Remove(Component::Drawable* drawable, WORD queue);

		void Present(Component::Camera* camera);
	};
}
