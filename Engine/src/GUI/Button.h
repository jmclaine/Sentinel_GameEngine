#pragma once

#include "Common.h"
#include "Event.h"

namespace Sentinel
{
	class Archive;

namespace GUI
{
	class SENTINEL_DLL Button
	{
	public:

		enum State
		{
			UP,
			OVER,
			DOWN,
		};

	protected:

		State mState;

	public:

		Event mActionUp;
		Event mActionDown;
		Event mActionClick;

	protected:

		Button();

	public:

		virtual ~Button();

		////////////////////////////////

		virtual void Update(bool isOver);

		////////////////////////////////

		virtual void Save(Archive& archive);
		virtual void Load(Archive& archive);
	};
}}
