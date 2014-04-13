#pragma once

#include <map>

#include "Common.h"

namespace Sentinel
{
	class DrawableComponent;
	
	class SENTINEL_DLL RenderManager
	{
	private:

		std::multimap< WORD, DrawableComponent* > mBackground;
		std::multimap< WORD, DrawableComponent* > mAlpha;
		std::multimap< WORD, DrawableComponent* > mForeground;

	public:

		RenderManager();
		~RenderManager();

		////////////////////////////////////

		void	Clear();

		// Place a DrawableComponent within the queue.
		// Draw is called on the component.
		//
		void	Draw( WORD renderQueue, DrawableComponent* drawable );

		// Render all geometry.
		//
		void	Present();
	};
}
