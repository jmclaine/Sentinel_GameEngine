using System;
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

		public ITransformComponent( WTransformComponent component )
		{
			Header	= "Transform";
			Data	= component;

			Items.Add( new Inspector( Inspector.CreateEdit( "Name: ", Data.Name )));

			Items.Add( new IVector3f( Data.Position, "Position" ));

			IVector3f inspector = new IVector3f( Data.Rotation, "Rotation" );
			inspector.ChangedValue = UpdateRotation;
			Items.Add( inspector );

			Items.Add( new IVector3f( Data.Scale, "Scale" ));
		}

		private void UpdateRotation()
		{
			Data.Update();
		}
	}
}
