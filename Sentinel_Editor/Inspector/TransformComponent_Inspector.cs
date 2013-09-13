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
	class TransformComponent_Inspector : Inspector_TreeViewItem
	{
		public WTransformComponent mData;

		public TransformComponent_Inspector( WTransformComponent component )
		{
			Header	= "Transform";
			mData	= component;

			Items.Add( new Vector3f_Inspector( mData.Position, "Position" ));

			Vector3f_Inspector inspector = new Vector3f_Inspector( mData.Rotation, "Rotation" );
			inspector.ChangedValue = Update;
			Items.Add( inspector );

			Items.Add( new Vector3f_Inspector( mData.Scale, "Scale" ));
		}

		private void Update()
		{
			mData.Update();
		}
	}
}
