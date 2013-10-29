using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	class IPhysicsComponent : Inspector
	{
		public WPhysicsComponent Data;

		private WRigidBody	mRigidBody;

		private IString		mName;
		private IVector3f	mPosition;
		private IVector3f	mScale;
		private IFloat		mMass;
		private IVector3f	mGravity;
		private IVector3f	mAngularFactor;
		private IFloat		mFriction;
		private IFloat		mRestitution;

		public IPhysicsComponent( WPhysicsComponent component )
		{
			Header	= "Physics";
			Data	= component;

			mName = new IString( "Name: ", Data.Name );
			mName.ChangedValue = ChangedName;
			Items.Add( mName );

			mRigidBody = Data.GetRigidBody();

			String[] items = new String[]{ "Invalid", "Sphere", "Box", "Cylinder", "Mesh" };
			Items.Add( new IEnum( "Shape Type: ", items, (int)mRigidBody.ShapeType, ChangedShapeType ));

			mPosition = new IVector3f( "Position", mRigidBody.Position );
			mPosition.ChangedValue = ChangedPosition;
			Items.Add( mPosition );

			//IVector3f inspector = new IVector3f( Data.Rotation, "Rotation" );
			//inspector.ChangedValue = UpdateRotation;
			//Items.Add( inspector );

			mScale = new IVector3f( "Scale", mRigidBody.Scale );
			mScale.ChangedValue = ChangedScale;
			Items.Add( mScale );

			///////////////////////////

			items = new String[]{ "None", "Disable Gravity", "Enable Gyroscopic Force" };
			Items.Add( new IEnum( "Flags: ", items, (int)mRigidBody.Flags, ChangedFlags, 200 ));
			
			mMass = new IFloat( "Mass: ", mRigidBody.Mass );
			mMass.ChangedValue = ChangedMass;
			Items.Add( mMass );

			mGravity = new IVector3f( "Gravity", mRigidBody.Gravity );
			mGravity.ChangedValue = ChangedGravity;
			Items.Add( mGravity );

			mAngularFactor = new IVector3f( "Angular Factor", mRigidBody.AngularFactor );
			mAngularFactor.ChangedValue = ChangedAngularFactor;
			Items.Add( mAngularFactor );

			mFriction = new IFloat( "Friction: ", mRigidBody.Friction );
			mFriction.ChangedValue = ChangedFriction;
			Items.Add( mFriction );

			mRestitution = new IFloat( "Restitution: ", mRigidBody.Restitution );
			mRestitution.ChangedValue = ChangedResitution;
			Items.Add( mRestitution );
		}

		private void ChangedName()
		{
			Data.Name = mName.Data;
		}

		private void ChangedPosition()
		{
			mRigidBody.Position = mPosition.Data;
		}
		
		private void ChangedScale()
		{
			mRigidBody.Scale = mScale.Data;
		}

		private void ChangedShapeType( Object sender, SelectionChangedEventArgs e )
		{
			mRigidBody.ShapeType = (PhysicsShapeType)(sender as ComboBox).SelectedIndex;
		}

		private void ChangedFlags( Object sender, SelectionChangedEventArgs e )
		{
			mRigidBody.Flags = (sender as ComboBox).SelectedIndex;
		}

		private void ChangedMass()
		{
			mRigidBody.Mass = mMass.Data;
		}

		private void ChangedGravity()
		{
			mRigidBody.Gravity = mGravity.Data;
		}

		private void ChangedAngularFactor()
		{
			mRigidBody.AngularFactor = mAngularFactor.Data;
		}

		private void ChangedFriction()
		{
			mRigidBody.Friction = mFriction.Data;
		}

		private void ChangedResitution()
		{
			mRigidBody.Restitution = mRestitution.Data;
		}
	}
}
