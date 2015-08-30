#pragma once

#include <map>

#include "Sentinel.h"
#include "Types.h"
#include "RendererTypes.h"

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

		std::map<WORD, std::multimap<WORD, Component::Drawable*>> mBackground;
		std::map<WORD, std::multimap<WORD, Component::Drawable*>> mAlpha;
		std::map<WORD, std::multimap<WORD, Component::Drawable*>> mForeground;

	public:

		RenderManager();
		~RenderManager();

		////////////////////////////////////

		void Clear();

		// Place a Component::Drawable within the queue.
		// Draw is called on the component.
		//
		void Draw(Component::Drawable* drawable, WORD renderQueue, WORD layer = RenderLayer::LAYER0);

		// Render all geometry.
		//
		void Present(Component::Camera* camera);
	};
}
