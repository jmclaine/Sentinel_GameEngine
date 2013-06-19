// NetworkSocket is compatible with Windows and Linux.
//
#pragma once

#include <string>
#include <unordered_map>

#ifndef WIN32
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <netinet/in.h>
    #include <netinet/ip.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <cerrno>

    #define INVALID_SOCKET  -1
    #define NO_ERROR         0
    #define SOCKET_ERROR    -1

    typedef int SOCKET;
    typedef sockaddr SOCKADDR;

    #define WSAGetLastError() 0
    #define ioctlsocket ioctl
    #define strcpy_s strcpy
#else
    #pragma comment( lib, "ws2_32.lib" )

    #include <Winsock2.h>
    #include <ws2tcpip.h>
    #include <IPHlpApi.h>
#endif

#include "Singleton.h"
#include "Util.h"

#define MAX_RTT_COUNT		5
#define PACKET_SIZE			1048576

namespace Sentinel
{
	enum NetworkType
	{
		NETWORK_UDP			= 0x0001,
		NETWORK_TCP			= 0x0002,

		NETWORK_NONBLOCKING = 0x0010,
		
		NETWORK_SERVER		= 0x1000,		// internal usage only
	};

	enum ConnectionStatus
	{
		STATUS_FAIL,
		STATUS_GOOD,
	};

	////////////////////////////////////////////////
	
	class NetworkSocket : public Singleton< NetworkSocket >
	{
	public:

		struct Connection
		{
			char		mUserName[ 32 ];
			sockaddr_in mReceiver;
			SOCKET		mNetworkSocket;

			UINT		mID;

			double		mEndRTT[ MAX_RTT_COUNT ];
			double		mAverageRTT;
			int			mIndexRTT;

			UCHAR		mStatus;
			double		mConnectionTimer;

			//////////////////////////////////

			Connection( UINT _id );
			~Connection();
		};

		// UINT = user ID
		//
		std::unordered_map< UINT, Connection* >	mConnection;

		UINT					mNextID;		// the server generates IDs for each client

		// Gathers information about connections.
		//
		sockaddr_in				mReceiverInfo;
		socklen_t				mReceiverAddrSize;

	private:

		Connection*				mCurrConnection;

		// Ensures packets can send() or recv().
		//
		struct fd_set			mNetworkSocketSet;
		struct timeval			mTimeout;

		WORD					mFlags;			// NetworkType

	public:

		NetworkSocket();
		~NetworkSocket();

		// Initialize by connection type flags.
		// Must use NETWORK_UDP or NETWORK_TCP.
		// Defaults to Blocking; otherwise use | NETWORK_NONBLOCKING flag.
		//
		bool Startup( WORD flags );
		bool Startup( WORD flags, int port );							// Server
		bool Startup( WORD flags, const char* ipAddress, int port );	// Client

		void Update();

		void Shutdown();

		///////////////////////////////////////

		bool Send( char* data, int size, int index = -1 );
		bool Recv( char* data, int size );

		const std::string Info();
	};
};
