#include "RenderManager.h"
#include "Component/Drawable.h"
#include "RenderLayer.h"
#include "GameObject.h"
#include <algorithm>

namespace Sentinel
{
	RenderManager::RenderManager()
	{ }

	////////////////////////////////////

	void RenderManager::Clear()
	{
		mDrawables.clear();
		mDebugDraw.clear();
	}

	void RenderManager::Add(Component::Drawable* drawable, WORD queue)
	{
		WORD layer = (WORD)drawable->GetOwner()->mLayer;

		auto render_it = mDrawables.find(layer);

		if (render_it == mDrawables.end())
		{
			mDrawables[layer] = std::map<WORD, std::multimap<UINT, Component::Drawable*>>();
		}
		
		auto& renderLayer = mDrawables[layer];
		auto queue_it = renderLayer.find(queue);

		if (queue_it == renderLayer.end())
		{
			renderLayer[queue] = std::multimap<UINT, Component::Drawable*>();
		}

		renderLayer[queue].insert(std::pair<UINT, Component::Drawable*>(drawable->ID(), drawable));
	}

	void RenderManager::Remove(Component::Drawable* drawable, WORD queue)
	{
		auto& renderLayer = mDrawables[(WORD)drawable->GetOwner()->mLayer];
		renderLayer[queue].erase(drawable->ID());
	}

	void RenderManager::Present(Component::Camera* camera)
	{
		WORD layer = camera->mRenderLayer;
		WORD currLayer;

		for (WORD x = 0; x < (int)RenderLayer::LENGTH; ++x)
		{
			currLayer = 1 << x;

			if (layer & currLayer)
			{
				auto drawable = mDrawables[currLayer];

				std::for_each(drawable.begin(), drawable.end(), [camera](const std::pair<WORD, std::multimap<UINT, Component::Drawable*>>& drawQueue)
				{
					auto queue = drawQueue.second;

					std::for_each(queue.begin(), queue.end(), [camera](const std::pair<UINT, Component::Drawable*>& item)
					{
						auto component = item.second;

						if (component->mEnabled && component->CheckVisible(camera))
							component->Draw(camera);
					});
				});
			}
		}
	}
}
