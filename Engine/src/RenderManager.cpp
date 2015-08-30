#include "RenderManager.h"
#include "Component/Drawable.h"
#include "RendererTypes.h"

namespace Sentinel
{
	RenderManager::RenderManager()
	{
		for (WORD x = 0; x < (int)RenderLayer::LENGTH; ++x)
		{
			mBackground[(1 << x)] = std::multimap<WORD, Component::Drawable*>();
		}
	}

	RenderManager::~RenderManager()
	{
		Clear();
	}

	////////////////////////////////////

	void RenderManager::Clear()
	{
		WORD currLayer;

		for (WORD x = 0; x < (int)RenderLayer::LENGTH; ++x)
		{
			currLayer = 1 << x;

			mBackground[currLayer].clear();
			mAlpha[currLayer].clear();
			mForeground[currLayer].clear();
		}
	}

	void RenderManager::Draw(Component::Drawable* drawable, WORD renderQueue, WORD layer)
	{
		WORD currLayer;

		for (WORD x = 0; x < (int)RenderLayer::LENGTH; ++x)
		{
			currLayer = 1 << x;

			if (layer & currLayer)
			{
				if (renderQueue < (WORD)(RenderQueue::ALPHA_BLEND))
				{
					mBackground[currLayer].insert(std::pair<WORD, Component::Drawable*>(renderQueue, drawable));
				}
				else if (renderQueue < (WORD)(RenderQueue::FOREGROUND))
				{
					mAlpha[currLayer].insert(std::pair<WORD, Component::Drawable*>(renderQueue, drawable));
				}
				else
				{
					mForeground[currLayer].insert(std::pair<WORD, Component::Drawable*>(renderQueue, drawable));
				}
			}
		}
	}

	void RenderManager::Present(Component::Camera* camera)
	{
		WORD layer = camera->mRenderLayer;
		WORD currLayer = 0;
		std::multimap<WORD, Component::Drawable*>* drawable;

		for (WORD x = 0; x < (int)RenderLayer::LENGTH; ++x)
		{
			currLayer = 1 << x;

			if (layer & currLayer)
			{
				drawable = &mBackground[currLayer];
				for (auto it = drawable->begin(); it != drawable->end(); ++it)
					if (it->second->CheckVisible(camera))
						it->second->Draw();

				drawable = &mAlpha[currLayer];
				for (auto it = drawable->begin(); it != drawable->end(); ++it)
					if (it->second->CheckVisible(camera))
						it->second->Draw();

				drawable = &mForeground[currLayer];
				for (auto it = drawable->begin(); it != drawable->end(); ++it)
					if (it->second->CheckVisible(camera))
						it->second->Draw();
			}
		}
	}
}
