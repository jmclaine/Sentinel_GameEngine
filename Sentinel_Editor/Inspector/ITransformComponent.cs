using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Media;
using System.ComponentModel;
using System.Collections.ObjectModel;

using Sentinel.Components;
using Sentinel.Math;

namespace Sentinel_Editor
{
	class ITransformComponent : Inspector
	{
		public WTransformComponent mData;

		public ITransformComponent( WTransformComponent component )
		{
			Header	= "Transform";
			mData	= component;

			Grid grid = Inspector.CreateEdit( "Name: ", mData.Name );
			Items.Add( grid );

			Items.Add( new IVector3f( mData.Position, "Position" ));

			IVector3f inspector = new IVector3f( mData.Rotation, "Rotation" );
			inspector.ChangedValue = UpdateRotation;
			Items.Add( inspector );

			Items.Add( new IVector3f( mData.Scale, "Scale" ));
		}

		private void UpdateRotation()
		{
			mData.Update();
		}
	}
}
