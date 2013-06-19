#include "NetworkPacket.h"

namespace Sentinel
{
	/////////////////////////////////////////////////////////////////////

	NetworkPacket::GuaranteedMessage::GuaranteedMessage( UINT size ) :
		mSize( size ),
		mData( new char[ size ] )
	{
		memset( mData, 0, size );
	}

	NetworkPacket::GuaranteedMessage::~GuaranteedMessage()
	{
		delete mData;
	}

	/////////////////////////////////////////////////////////////////////

	NetworkPacket::GuaranteedSender::GuaranteedSender( UINT ack ) :
		mACK( ack ),
		mResendTimer( 0 ),
		mTotalTime( 0 )
	{}

	bool NetworkPacket::GuaranteedSender::operator < ( GuaranteedSender* sender )
	{
		return mACK < sender->mACK;
	}

	bool NetworkPacket::GuaranteedSender::operator == ( GuaranteedSender* sender )
	{
		return mACK == sender->mACK;
	}

	/////////////////////////////////////////////////////////////////////

	std::unordered_map< UINT, NetworkPacket::GuaranteedMessage* >									 NetworkPacket::mGuaranteedMessageMap;
	std::unordered_map< NetworkSocket::Connection*, std::vector< NetworkPacket::GuaranteedSender* >> NetworkPacket::mGuaranteedSenderMap;

	UINT NetworkPacket::mACK = 0;

	/////////////////////////////////////////////////////////////////////

	NetworkPacket::NetworkPacket()
	{
		Startup( PACKET_SIZE );
		Clear( PACKET_SIZE );
	}

	NetworkPacket::~NetworkPacket()
	{
		delete[] mData;
	}

	void NetworkPacket::Startup( UINT size )
	{
		mSize  = size;
		mData  = new char[ mSize ];
		mStart = mData;

		mACK = 0;
	}

	void NetworkPacket::Update()
	{
		TRAVERSE_LIST( it0, mGuaranteedSenderMap )
		{
			float resend = (float)it0->first->mAverageRTT * 2.5f;
			resend = max( resend, 1.0f );

			TRAVERSE_LIST( it1, it0->second )
			{
				(*it1)->mResendTimer += Timing::Inst()->DeltaTime();
				(*it1)->mTotalTime   += Timing::Inst()->DeltaTime();

				if( (*it1)->mResendTimer > resend )
				{
					(*it1)->mResendTimer = 0;

					GuaranteedMessage* msg = mGuaranteedMessageMap[ (*it1)->mACK ];

					char* data = msg->mData + sizeof( NetworkPacket::Header );

					GuaranteedHeader* gHeader = (GuaranteedHeader*)data;
					gHeader->mAdvanceTimer = (*it1)->mTotalTime;

					NetworkSocket::Inst()->Send( msg->mData, msg->mSize, it0->first->mID );
				}
			}
		}
	}

	void NetworkPacket::Shutdown()
	{
		TRAVERSE_LIST( it, mGuaranteedMessageMap )
			delete it->second;
		mGuaranteedMessageMap.clear();

		TRAVERSE_LIST( it0, mGuaranteedSenderMap )
			TRAVERSE_LIST( it1, it0->second )
				delete (*it1);
		mGuaranteedSenderMap.clear();
	}

	void NetworkPacket::Clear( UINT size )
	{
		memset( mData, 0, size );
		mPosition = mData;
		mStart = mPosition;
	}

	///////////////////////////////////////////////////

	NetworkPacketSender::NetworkPacketSender()
	{}

	void NetworkPacketSender::AddHeader( UCHAR header, float advTimer )
	{
		mStart = mPosition;

		AddValue( NetworkPacket::Header( header, NetworkSocket::Inst()->mConnection[ 0 ]->mID, (WORD)0 ));

		if( header & HEADER_GUARANTEED )
		{
			AddValue( GuaranteedHeader( 0, advTimer ));
		}
	}

	void NetworkPacketSender::AddString( const char* v )
	{
		UINT len = (UINT)strlen( v );

		for( UINT y = 0; y < len; ++y )
			AddValue( (char)v[ y ] );

		++mPosition;
	}

	void NetworkPacketSender::AddEnd()
	{
		AddValue( (BYTE)0 );

		int size = mPosition - mStart;
			
		_ASSERT( size > 0 && size <= (int)mSize );
			
		((NetworkPacket::Header*)mStart)->mSize = (WORD)size;
	}

	bool NetworkPacketSender::Send( int index )
	{
		bool result = false;

		int size = mPosition - mData;

		if( (char*)mData != mPosition )
		{
			_ASSERT( size <= (int)mSize );

			NetworkPacket::Header* pHeader = (NetworkPacket::Header*)mData;
			
			// Ensure valid packet.
			//
			if( pHeader->mType >= HEADER_UNKNOWN )
			{
				TRACE( "Bad Header!" );

				return false;
			}

			// Prepare Guaranteed Message.
			//
			if( pHeader->mType & HEADER_GUARANTEED )
			{
				// Check if this message is a resend.
				//
				GuaranteedHeader* gHeader = (GuaranteedHeader*)(mData + sizeof( NetworkPacket::Header ));
				
				auto itMsg = mGuaranteedMessageMap.find( gHeader->mACK );
				if( itMsg == mGuaranteedMessageMap.end() )
				{
					// Copy packet, and store within resend list.
					//
					if( size <= PACKET_SIZE )
					{
						gHeader->mACK = mACK;

						UINT counter = 0;
						bool isValid = false;

						if( index == -1 )
						{
							isValid = true;
							TRAVERSE_LIST( it, NetworkSocket::Inst()->mConnection )
							{
								++counter;
								mGuaranteedSenderMap[ it->second ].push_back( new GuaranteedSender( mACK ));
							}
						}
						else
						{
							auto itConn = NetworkSocket::Inst()->mConnection.find( index );
							if( itConn != NetworkSocket::Inst()->mConnection.end() )
							{
								isValid = true;
								++counter;
								mGuaranteedSenderMap[ itConn->second ].push_back( new GuaranteedSender( mACK ));
							}
						}

						if( !isValid )
							return false;

						GuaranteedMessage* msg = new GuaranteedMessage( size );
						msg->mCounter = counter;
						memcpy( msg->mData, mData, size );
						mGuaranteedMessageMap[ mACK ] = msg;

						++mACK;
					}
					else
					{
						TRACE( "Attempted GHeader Size: " << size );

						return false;
					}
				}
			}
				
			result = NetworkSocket::Inst()->Send( mData, (UINT)size, index );
		}

		Clear( size );

		return result;
	}

	///////////////////////////////////////////////////

	NetworkPacketReceiver::NetworkPacketReceiver()
	{}

	const NetworkPacket::Header& NetworkPacketReceiver::GetHeader()
	{
		mStart = mPosition;

		return GetValue< NetworkPacket::Header >();
	}

	const char* NetworkPacketReceiver::GetString()
	{
		const char* v = mPosition;
		mPosition += strlen( mPosition ) + 1;
		return v;
	}

	void NetworkPacketReceiver::Ignore( UINT size )
	{
		mPosition = mStart + size;
	}

	bool NetworkPacketReceiver::Recv()
	{
		Clear( mSize );

		bool result = NetworkSocket::Inst()->Recv( mData, mSize );

		// Process the received packet.
		//
		if( result )
		{
			mStart = mData;

			while( *mStart != NULL )
			{
				mPosition = mStart;

				NetworkSocket::Connection* connection = NULL;
				NetworkPacket::Header* pHeader = (NetworkPacket::Header*)(mPosition);
				mPosition += sizeof( NetworkPacket::Header );

				if( pHeader->mType != HEADER_NULL )
				{
					// Ensure the packet received is valid.
					//
					if( pHeader->mSenderID != 0 )
					{
						connection = NetworkSocket::Inst()->mConnection[ pHeader->mSenderID ];
						connection->mConnectionTimer = 0;
					}
					else
					if( !(pHeader->mType & HEADER_CLIENT) )
					{
						TRACE( "Received NetworkPacket from Invalid ID #" << pHeader->mSenderID );

						return false;
					}

					// Offset data for guaranteed message.
					//
					GuaranteedHeader* gHeader = NULL;
					if( pHeader->mType & HEADER_GUARANTEED )
					{
						gHeader = (GuaranteedHeader*)(mPosition);
						mPosition += sizeof( GuaranteedHeader );
					}

					// Process this packet.
					//
					if( pHeader->mType & HEADER_PACKET )
					{}
					else
					if( pHeader->mType & HEADER_KEEP_ALIVE )
					{}
					else
					if( pHeader->mType & HEADER_ACK )
					{
						++connection->mIndexRTT;
						wrap( connection->mIndexRTT, 0, MAX_RTT_COUNT-1 );
						
						connection->mAverageRTT = 0;
						for( UINT x = 0; x < MAX_RTT_COUNT; ++x )
						{
							connection->mAverageRTT += connection->mEndRTT[ x ];
						}
						connection->mAverageRTT /= MAX_RTT_COUNT;

						// Check for acknowledgment.
						//
						UINT ack = *(UINT*)(mPosition);

						// Remove this message from the receiver's list.
						//
						mGuaranteedMessageMap[ ack ]->mCounter--;
						if( mGuaranteedMessageMap[ ack ]->mCounter == 0 )
							mGuaranteedMessageMap.erase( ack );

						std::vector< GuaranteedSender* >& sender = mGuaranteedSenderMap[ connection ];
						auto it = binary_find( sender.begin(), sender.end(), &GuaranteedSender( ack ));
						connection->mEndRTT[ connection->mIndexRTT ] = (*it)->mTotalTime;
						sender.erase( it );
					
						//TRACE( "ACK #" << ack << " from " << connection->mUserName );
					}
					// Setup a client on the server.
					//
					else
					if( pHeader->mType & HEADER_CLIENT )
					{
						// Check for existing user.
						//
						std::string name( mPosition );

						if( connection )
						{
							if( connection->mID != 0 )
							{
								// TCP automatically generates a connection when a client
								// attempts to join the server.
								//
								connection->mConnectionTimer = 0;
							
								if( connection->mStatus == STATUS_FAIL )
								{
									TRACE( connection->mUserName << " Reconnected." );
								}
								else
								{
									TRACE( connection->mUserName << " Connected." );
								}

								connection->mStatus = STATUS_GOOD;
							}
							else
							{
								// UDP client logging into server.
								//
								REPORT_ERROR( "This should not have happened.", "NetworkSocket Failure" );

								connection->mID = NetworkSocket::Inst()->mNextID++;
								strcpy_s( connection->mUserName, name.c_str() );
							
								TRACE( connection->mUserName << " Logged In." );
							}
						}
						else
						{
							NetworkSocket::Connection* baseConnection = NetworkSocket::Inst()->mConnection[ 0 ];
							connection = new NetworkSocket::Connection( NetworkSocket::Inst()->mNextID++ );

							connection->mNetworkSocket   = baseConnection->mNetworkSocket;
							connection->mReceiver = NetworkSocket::Inst()->mReceiverInfo;
							strcpy_s( connection->mUserName, name.c_str() );

							NetworkSocket::Inst()->mConnection[ connection->mID ] = connection;

							TRACE( connection->mUserName << " Logged In." );
						}

						// Overwrite the client ID so that the server
						// knows who sent it in setting up the player.
						//
						pHeader->mSenderID = connection->mID;

						if( connection->mStatus == STATUS_FAIL )
						{
							connection->mStatus		= STATUS_GOOD;
							connection->mConnectionTimer = 0;

							TRACE( "Assigned ID: " << pHeader->mSenderID );

							// Send the client its ID on the server.
							//
							NetworkPacketSender::Inst()->AddHeader( HEADER_SERVER | HEADER_GUARANTEED );
							NetworkPacketSender::Inst()->AddValue( (UINT)pHeader->mSenderID );
							NetworkPacketSender::Inst()->AddEnd();
							NetworkPacketSender::Inst()->Send( pHeader->mSenderID );
						}
					}
					// Setup the client.
					//
					else
					if( pHeader->mType & HEADER_SERVER )
					{
						connection = NetworkSocket::Inst()->mConnection[ 0 ];

						TRACE( "Logged In --> " << connection->mUserName );

						// Set the current order number.
						//
						if( connection->mID == 0 )
						{
							//connection->mNetworkSocket		= client;
							connection->mReceiver	= NetworkSocket::Inst()->mReceiverInfo;
						
							connection->mID			= *(UINT*)mPosition;
						}

						TRACE( "Assigned ID: " << connection->mID );
					}
					else
					{
						TRACE( "Invalid Header Received" << "; Type: " << pHeader->mType << 
															"; Sender: " << pHeader->mSenderID << 
															"; Size: " << pHeader->mSize );

						return false;
					}

					// Send ACK.
					//
					if( pHeader->mType & HEADER_GUARANTEED )
					{
						//TRACE( "Sending ACK #" << gHeader->ack << " --> " << mCurrReceiver->userName );

						NetworkPacketSender::Inst()->AddHeader( HEADER_ACK );
						NetworkPacketSender::Inst()->AddValue( (UINT)gHeader->mACK );
						NetworkPacketSender::Inst()->AddEnd();
						NetworkPacketSender::Inst()->Send( pHeader->mSenderID );
					}

					mStart += pHeader->mSize;
				}
				else
				{
					return false;
				}
			}
		}

		return result;
	}
}
