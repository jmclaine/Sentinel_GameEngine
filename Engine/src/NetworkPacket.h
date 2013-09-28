#pragma once

#include "Common.h"
#include "NetworkSocket.h"
#include "Timing.h"

namespace Sentinel
{
	enum HeaderType
	{
		HEADER_NULL			= 0x00,		// undefined header

		HEADER_CLIENT		= 0x01,		// from client (for init)
		HEADER_SERVER		= 0x02,		// from server (for init)
		HEADER_PACKET		= 0x04,		// normal packet
		HEADER_ACK			= 0x08,		// acknowledgment

		HEADER_GUARANTEED	= 0x10,		// guaranteed message
		HEADER_KEEP_ALIVE	= 0x20,		// tag as keep alive packet

		HEADER_UNKNOWN		= 0x40,		// this must be an error
	};

	////////////////////////////////////////////////

	// Guaranteed messages are handled internally.
	// This class is a basis for sending and receiving packets.
	//
	class SENTINEL_DLL NetworkPacket
	{
	public:

		struct Header
		{
			UCHAR		mType;			// HeaderType
			UINT		mSenderID;		// ID of sender
			WORD		mSize;			// size of packet

			Header( UCHAR _type, UINT _senderID, WORD _size ) :
				mType( (UCHAR)_type ),
				mSenderID( _senderID ),
				mSize( _size )
				{}
		};

		// Begins after NetworkPacket::Header()
		//
		struct GuaranteedHeader
		{
			UINT		mACK;			// ACK number
			float		mAdvanceTimer;	// time to advance ahead when received

			GuaranteedHeader( UINT _ack, float _advTimer ) :
				mACK( _ack ),
				mAdvanceTimer( _advTimer )
				{}
		};

		struct SENTINEL_DLL GuaranteedMessage
		{
			UINT		mCounter;		// count of instances
			UINT		mSize;			// size of data
			char*		mData;			// guaranteed data packet

			GuaranteedMessage( UINT size );
			~GuaranteedMessage();
		};

		struct SENTINEL_DLL GuaranteedSender
		{
			UINT		mACK;			// ACK number of message to send
			float		mResendTimer;	// Timer to send message (based on mAverageRTT of connection)
			float		mTotalTime;		// Total amount of time elapsed since message was generated (used for mAdvanceTimer)

			GuaranteedSender( UINT ack );

			bool operator < ( GuaranteedSender* sender );
			bool operator == ( GuaranteedSender* sender );
		};

	protected:

		// UINT = ACK
		//
		static std::unordered_map< UINT, GuaranteedMessage* >									 mGuaranteedMessageMap;

		// Use connections as a basis for what messages should be sent.
		//
		static std::unordered_map< NetworkSocket::Connection*, std::vector< GuaranteedSender* >> mGuaranteedSenderMap;

		static UINT	mACK;		// next ACK number

		////////////////////////////////

		char*	mData;			// starting location of entire packet data
		char*	mStart;			// starting location of a packet (header)
		char*	mPosition;		// current memory location within packet

		UINT	mSize;			// size of packet

	public:

		NetworkPacket();

		virtual ~NetworkPacket();

	private:

		void		Startup( UINT size );

	public:

		static void Update();

		static void Shutdown();

		void		Clear( UINT size = PACKET_SIZE );
	};

	// Sends constructed packets over the network.
	//
	// Always begin each packet with a header,
	// so that NetworkPacket can process it correctly.
	//
	// Example:
	//
	// NetworkPacketSender::Inst()->AddHeader( HEADER_PACKET | HEADER_GUARANTEED );
	// NetworkPacketSender::Inst()->AddValue( (BYTE)0 );
	// NetworkPacketSender::Inst()->AddEnd();
	//
	// NetworkPacketSender::Inst()->Send();
	//
	class SENTINEL_DLL NetworkPacketSender : public NetworkPacket, public SingletonSafe< NetworkPacketSender >
	{
		friend class SingletonSafe< NetworkPacketSender >;

	private:

		NetworkPacketSender();

	public:

		void AddHeader( UCHAR header, float advTimer = 0.0f );

		template< typename Value >
		void AddValue( const Value& v )
		{
			*(Value*)mData = v;
			mData += sizeof( Value );
		}

		void AddString( const char* v );

		void AddEnd();

		bool Send( int index = -1 );	// -1 = send to all
	};

	// Receives packets for processing.
	// Packets should start with a header,
	// and end with NULL (0) termination.
	//
	// Example:
	//
	// if( NetworkPacketReceiver::Inst()->Recv() )
	// {
	//     NetworkPacket::Header header = NetworkPacketReceiver::Inst()->GetHeader();
	//
	//     BYTE value = NetworkPacketReceiver::Inst()->GetValue< BYTE >();
	// }
	//
	class SENTINEL_DLL NetworkPacketReceiver : public NetworkPacket, public SingletonSafe< NetworkPacketReceiver >
	{
		friend class SingletonSafe< NetworkPacketReceiver >;

	private:

		NetworkPacketReceiver();

	public:

		const NetworkPacket::Header& GetHeader();

		const char* GetString();

		template< typename Value >
		const Value& GetValue()
		{
			const Value& v = *(Value*)mData;
			mData += sizeof( Value );
			return v;
		}

		void Ignore( UINT size );

		bool Recv();
	};
}
