#include "RenderManager.h"
#include "DrawableComponent.h"
#include "RendererTypes.h"

namespace Sentinel
{
	RenderManager::RenderManager()
	{}

	RenderManager::~RenderManager()
	{
		Clear();
	}

	////////////////////////////////////

	void RenderManager::Clear()
	{
		mBackground.clear();
		mAlpha.clear();
		mForeground.clear();
	}

	void RenderManager::Draw( WORD renderQueue, DrawableComponent* drawable )
	{
		if( renderQueue < (WORD)(RenderQueue::ALPHA_BLEND) )
		{
			mBackground.insert( std::pair< WORD, DrawableComponent* >( renderQueue, drawable ));
		}
		else
		if( renderQueue < (WORD)(RenderQueue::FOREGROUND) )
		{
			mAlpha.insert( std::pair< WORD, DrawableComponent* >( renderQueue, drawable ));
		}
		else
		{
			mForeground.insert( std::pair< WORD, DrawableComponent* >( renderQueue, drawable ));
		}
	}

	void RenderManager::Present()
	{
		for( auto it = mBackground.begin(); it != mBackground.end(); ++it )
			if( it->second->mIsVisible )
				it->second->Draw();

		for( auto it = mAlpha.begin(); it != mAlpha.end(); ++it )
			if( it->second->mIsVisible )
				it->second->Draw();

		for( auto it = mForeground.begin(); it != mForeground.end(); ++it )
			if( it->second->mIsVisible )
				it->second->Draw();
	}
}
