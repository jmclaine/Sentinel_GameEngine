﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Media;
using System.ComponentModel;
using System.Collections.ObjectModel;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	class ITransformComponent : Inspector
	{
		public WTransformComponent Data { get; set; }

		private IString mName;

		public ITransformComponent( WTransformComponent component )
		{
			Header	= "Transform";
			Data	= component;

			mName = new IString( "Name: ", Data.Name );
			mName.ChangedValue = ChangedName;
			Items.Add( mName );

			Items.Add( new IVector3f( "Position", Data.Position ));

			IVector3f inspector = new IVector3f( "Rotation", Data.Rotation );
			inspector.ChangedValue = UpdateRotation;
			Items.Add( inspector );

			Items.Add( new IVector3f( "Scale", Data.Scale ));
		}

		private void UpdateRotation()
		{
			Data.Update();
		}

		private void ChangedName()
		{
			Data.Name = mName.Data;
		}
	}
}
