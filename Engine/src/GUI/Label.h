#pragma once

#include "Component/Drawable.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	class FontSystem;

namespace GUI
{
	class SENTINEL_DLL Label : public Component::Drawable
	{
		DECLARE_SERIAL();

	public:

		DECLARE_SERIAL_REGISTER_SAVE();

		FontSystem* mFontSystem;

		std::string mText;
		ColorRGBA mColor;

		//////////////////////////////////

		Label();
		Label(const std::string& text);
		virtual ~Label();

		//////////////////////////////////

		void Startup();

		void Update();

		void Shutdown();

		//////////////////////////////////

		void CalculateBounds();

		bool CheckVisible(Component::Camera* camera);

		void Draw();

		//////////////////////////////////

		GameComponent* Copy();
	};
}}
