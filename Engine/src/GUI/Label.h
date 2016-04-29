#pragma once

#include "Component/Drawable.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	class Font;
	class FontSystem;

namespace GUI
{
	class SENTINEL_DLL Label : public Component::Drawable
	{
	private:
		static SerialRegister SERIAL_REGISTER;
		static Serializable* Clone();

	public:
		std::weak_ptr<Font> mFont;

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
		void Draw(Component::Camera* camera);

		//////////////////////////////////

		GameComponent* Copy();

		//////////////////////////////////

		void SerialSave(Archive& archive);

		void Save(Archive& archive);
		void Load(Archive& archive);
	};
}}
