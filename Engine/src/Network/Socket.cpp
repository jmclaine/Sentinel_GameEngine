#include <iostream>
#include <sstream>

#include "Network/Socket.h"
#include "Timing.h"
#include "Debug.h"
#include "Memory.h"

namespace Sentinel {
namespace Network
{
#ifdef _DEBUG
	#define SIMULATE_PACKET_LOSS(chance);//	if( rand()%100 < chance ) return false;
#else
	#define SIMULATE_PACKET_LOSS(chance);
#endif

	#define EVENT_TIMER				1		// seconds
	#define CONNECTION_TIMEOUT		5.0f	// seconds

	///////////////////////////////////////////

	Socket::Connection::Connection(UINT _id)
	{
		memset(&mUserName, 0, 32);
		memset(&mReceiver, 0, sizeof(sockaddr_in));

		mID = _id;

		mIndexRTT = 0;

		mStatus = STATUS_FAIL;
		mConnectionTimer = 0;

		for (UINT x = 0; x < MAX_RTT_COUNT; ++x)
		{
			mEndRTT[x] = Timing::DESIRED_FRAME_RATE;
		}

		mAverageRTT = Timing::DESIRED_FRAME_RATE;
	}

	Socket::Connection::~Connection()
	{}

	///////////////////////////////////////////

	Socket::Socket()
	{
		mCurrConnection = NULL;

		mNextID = 1;

		mReceiverAddrSize = sizeof(struct sockaddr_in);

		mNetworkSocketSet.fd_count = 1;

		mTimeout.tv_sec = EVENT_TIMER;
		mTimeout.tv_usec = 0;
	}

	Socket::~Socket()
	{
		TRAVERSE_LIST(it, mConnection)
			delete it->second;

		Shutdown();

	#ifdef WIN32
		WSACleanup();
	#endif
	}

	// Initialize by connection type flags.
	// Must use NETWORK_UDP or NETWORK_TCP.
	// Defaults to Blocking; otherwise use | NETWORK_NONBLOCKING flag.
	//
	bool Socket::Startup(WORD flags)
	{
		_ASSERT(flags != NULL);

		Debug::Log("Initializing...");

	#ifdef WIN32
		WSADATA	m_wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &m_wsaData) != NO_ERROR)
		{
			printf("Error %d: %s\n", WSAGetLastError(), "WSAStartup Failure");
			return false;
		}
	#endif

		mCurrConnection = new Connection(0);
		mConnection[0] = mCurrConnection;

		mFlags = flags;
		if (mFlags & NETWORK_UDP)
		{
			if ((mCurrConnection->mNetworkSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
			{
				printf("Error %d: %s\n", WSAGetLastError(), "NetworkSocket Failure");
				return false;
			}
		}
		else if (mFlags & NETWORK_TCP)
		{
			if ((mCurrConnection->mNetworkSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			{
				printf("Error %d: %s\n", WSAGetLastError(), "NetworkSocket Failure");
				return false;
			}
		}

		u_long mode = (mFlags & NETWORK_NONBLOCKING) ? 1 : 0;
		if (ioctlsocket(mCurrConnection->mNetworkSocket, FIONBIO, &mode) != NO_ERROR)
		{
			printf("Error %d: %s\n", WSAGetLastError(), "NetworkSocket I/O Failure");
			return false;
		}

		return true;
	}

	// Server.
	//
	bool Socket::Startup(WORD flags, int port)
	{
		if (!Startup(flags))
		{
			int lastError = WSAGetLastError();
			Debug::LogError(STREAM(lastError << ": Initialization Failed."));
			Shutdown();

			return false;
		}

		mReceiverInfo.sin_family = AF_INET;
		mReceiverInfo.sin_addr.s_addr = INADDR_ANY;
		mReceiverInfo.sin_port = htons((u_short)port);

		if (bind(mCurrConnection->mNetworkSocket, (SOCKADDR*)&mReceiverInfo, mReceiverAddrSize) == SOCKET_ERROR)
		{
			int lastError = WSAGetLastError();
			Debug::LogError(STREAM(lastError << ": Host Failure."));
			Shutdown();

			return false;
		}

		if (mFlags & NETWORK_TCP)
		{
			if (listen(mCurrConnection->mNetworkSocket, SOMAXCONN) == SOCKET_ERROR)
			{
				int lastError = WSAGetLastError();
				Debug::LogError(STREAM(lastError << ": Listen Failure."));
				Shutdown();

				return false;
			}
		}

		mFlags |= NETWORK_SERVER;

		Debug::Log(STREAM("Hosting on Port " << port << " ..."));

		return true;
	}

	// Client.
	//
	bool Socket::Startup(WORD flags, const char* ipAddress, int port)
	{
		if (!Startup(flags))
		{
			int lastError = WSAGetLastError();
			Debug::LogError(STREAM(lastError << ": Initialization Failed."));
			Shutdown();

			return false;
		}

		Debug::Log(STREAM("Connecting to IP: " << ipAddress << " @ Port: " << port << " ..."));

		mCurrConnection->mReceiver.sin_family = AF_INET;
		mCurrConnection->mReceiver.sin_addr.s_addr = (!ipAddress) ? INADDR_ANY : inet_addr(ipAddress);
		mCurrConnection->mReceiver.sin_port = htons((u_short)port);

		strcpy_s(mCurrConnection->mUserName, "Server");

		SOCKET socket = mCurrConnection->mNetworkSocket;

		if (mFlags & NETWORK_TCP)
		{
			if (connect(socket, (SOCKADDR*)&mCurrConnection->mReceiver, mReceiverAddrSize) == SOCKET_ERROR)
			{
				if (mFlags & NETWORK_NONBLOCKING)
				{
					int connectResult = WSAGetLastError();
					if (connectResult == WSAEWOULDBLOCK)
					{
						fd_set socket_write, socket_error;
						FD_ZERO(&socket_write);
						FD_ZERO(&socket_error);
						FD_SET(socket, &socket_write);
						FD_SET(socket, &socket_error);

						TIMEVAL timeout;
						timeout.tv_sec = 5;
						timeout.tv_usec = 0;

						connectResult = select(0, NULL, &socket_write, &socket_error, &timeout);

						if (connectResult == 0)
						{
							Debug::Log("Connection Timeout.");

							Shutdown();

							return false;
						}
						else if (connectResult == SOCKET_ERROR)
						{
							connectResult = WSAGetLastError();

							Debug::LogError(STREAM(connectResult << ": Connection Failed (select)"));

							Shutdown();

							if (FD_ISSET(socket, &socket_write))
							{ }
							else if (FD_ISSET(socket, &socket_error))
							{
								Debug::LogWarning("Server Not Found.");
							}

							return false;
						}

						u_long iMode = 1;
						ioctlsocket(socket, FIONBIO, &iMode);
					}
					else
					{
						Debug::LogError(STREAM(connectResult << ": Connection Failed (nonblock)"));
						Shutdown();

						return false;
					}
				}
				else
				{
					int lastError = WSAGetLastError();
					Debug::LogError(STREAM(lastError << ": Connection Failed"));
					Shutdown();

					return false;
				}
			}
		}

		Debug::Log("Connected!");

		mCurrConnection->mStatus = STATUS_GOOD;

		return true;
	}

	bool Socket::Send(char* data, int size, int index)
	{
		_ASSERT(data != NULL);

		bool result = true;

		auto start = mConnection.begin();
		auto end = mConnection.end();

		if (index > -1)
		{
			start = mConnection.find(index);
			end = start++;
		}

		for (auto it = start; it != end; ++it)
		{
			Connection* connection = it->second;
			if (connection->mStatus == STATUS_GOOD)
			{
				// Ensure the socket is available to send.
				//
				mNetworkSocketSet.fd_array[0] = it->second->mNetworkSocket;
				int rv = select(0, NULL, &mNetworkSocketSet, NULL, &mTimeout);

				if (rv < 0)
				{
					Debug::LogError(STREAM(rv << ": Select Failure on Client #" << it->second->mID));
				}

				// Send packet.
				//
				if (mFlags & NETWORK_UDP)
				{
					if (::sendto(connection->mNetworkSocket, data, size, 0, (SOCKADDR*)&connection->mReceiver, mReceiverAddrSize) == SOCKET_ERROR)
					{
						int lastError = WSAGetLastError();
						Debug::LogError(STREAM(lastError << ": Send Failure"));

						if (lastError != WSAEWOULDBLOCK && (mFlags & NETWORK_NONBLOCKING))
						{
							connection->mNetworkSocket = INVALID_SOCKET;
							connection->mConnectionTimer = DBL_MAX;
						}

						result = false;
					}
				}
				else if (mFlags & NETWORK_TCP)
				{
					// Each connection has its own socket.
					//
					if (::send(connection->mNetworkSocket, data, size, 0) == SOCKET_ERROR)
					{
						int lastError = WSAGetLastError();
						Debug::LogError(STREAM(lastError << ": Send Failure"));

						if (lastError != WSAEWOULDBLOCK && (mFlags & NETWORK_NONBLOCKING))
						{
							connection->mNetworkSocket = INVALID_SOCKET;
							connection->mConnectionTimer = DBL_MAX;
						}

						result = false;
					}
				}
			}
			else result = false;
		}

		//Debug::Log( "Message Sent: %s\n", data );

		return result;
	}

	bool Socket::Recv(char* data, int size)
	{
		static SOCKET client = mConnection[0]->mNetworkSocket;

		u_long readSize = 0;
		memset(data, 0, size);

		if (mFlags & NETWORK_UDP)
		{
			if (mFlags & NETWORK_NONBLOCKING)
			{
				if (ioctlsocket(client, FIONREAD, &readSize) != NO_ERROR)
				{
					int lastError = WSAGetLastError();
					Debug::LogError(STREAM(lastError << ": Receive Non-Blocking Failure"));

					return false;
				}

				if (readSize > (u_long)size)
				{
					Debug::LogWarning(STREAM("Attempted to 'recv': " << readSize << " --> size: " << size));

					return false;
				}
			}
			else readSize = size;

			if (readSize > 0)
			{
				mNetworkSocketSet.fd_array[0] = client;
				int rv = select(0, &mNetworkSocketSet, NULL, NULL, &mTimeout);

				if (rv < 0)
				{
					Debug::LogError(STREAM(rv << ": Select Failure on 'recv'"));
				}

				if (::recvfrom(client, data, readSize, 0, (SOCKADDR*)&mReceiverInfo, (int*)&mReceiverAddrSize) == SOCKET_ERROR)
				{
				#ifdef WIN32
					int lastError = WSAGetLastError();
					if (lastError != WSAECONNRESET)	// some connection has been lost
					{
						Debug::LogError(STREAM(lastError << ": Receive Failure"));
					}
				#else
					printf( "Receive Failure\n" );
				#endif

					return false;
				}

				return true;
			}
		}
		else if (mFlags & NETWORK_TCP)
		{
			bool didRecv = false;

			TRAVERSE_LIST_OFFSET(it, mConnection, +)
			{
				mCurrConnection = it->second;
				client = mCurrConnection->mNetworkSocket;

				if (client != INVALID_SOCKET)
				{
					if (mFlags & NETWORK_NONBLOCKING)
					{
						if (ioctlsocket(client, FIONREAD, &readSize) != NO_ERROR)
						{
							int lastError = WSAGetLastError();
							Debug::LogError(STREAM(lastError << ": Receive Non-Blocking Failure"));

							mCurrConnection->mStatus = STATUS_FAIL;
						}

						if (readSize > PACKET_SIZE)
						{
							Debug::LogWarning(STREAM("Attempted to RECV " << readSize << " Bytes"));

							return false;
						}
					}
					else readSize = size;

					if (readSize > 0)
					{
						mNetworkSocketSet.fd_array[0] = client;
						int rv = select(0, &mNetworkSocketSet, NULL, NULL, &mTimeout);

						if (rv < 0)
						{
							Debug::LogError(STREAM(rv << ": Select Failure on 'recv' from " << mCurrConnection->mID));
						}

						if (::recv(client, data, readSize, 0) == SOCKET_ERROR)
						{
							int lastErr = WSAGetLastError();
							if (lastErr == WSAEWOULDBLOCK && (mFlags & NETWORK_NONBLOCKING))
							{ }
							else
							{
								Debug::LogError(STREAM(lastErr << ": Receive Failure"));

								mCurrConnection->mStatus = STATUS_FAIL;
							}
						}
						else
						{
							didRecv = true;
						}
					}
				}
			}

			return didRecv;
		}

		return false;	// no packet received
	}

	void Socket::Shutdown()
	{
	#ifdef WIN32
		TRAVERSE_LIST(it, mConnection)
			closesocket(it->second->mNetworkSocket);
	#else
		TRAVERSE_LIST( it, mConnection )
			close( it->second->mNetworkSocket );
	#endif
	}

	std::string Socket::Info()
	{
		std::stringstream str;

		TRAVERSE_LIST(it, mConnection)
		{
			mCurrConnection = it->second;

			str << "Name: " << mCurrConnection->mUserName << std::endl \
				<< "IPv4: " << inet_ntoa(mCurrConnection->mReceiver.sin_addr) << std::endl \
				<< "Port: " << ntohs(mCurrConnection->mReceiver.sin_port) << std::endl \
				<< "  ID: " << (int)mCurrConnection->mID << std::endl \
				<< "Stat: " << ((mCurrConnection->mStatus == STATUS_GOOD) ? "Good" : "Fail") << std::endl \
				<< " RTT: " << mCurrConnection->mAverageRTT * 1000.0 << " ms" << std::endl \
				<< std::endl;
		}

		return str.str();
	}

	void Socket::Update(float DT)
	{
		// Check if a client has connected to the server.
		//
		if ((mFlags & NETWORK_TCP) && (mFlags & NETWORK_SERVER))
		{
			SOCKET socket = accept(mConnection[0]->mNetworkSocket, (SOCKADDR*)&mReceiverInfo, (int*)&mReceiverAddrSize);

			if (socket != INVALID_SOCKET)
			{
				mCurrConnection = new Connection(mNextID++);
				mCurrConnection->mNetworkSocket = socket;
				mCurrConnection->mReceiver = mReceiverInfo;

				mConnection[mCurrConnection->mID] = mCurrConnection;
			}
		}

		// Update Connections.
		//
		TRAVERSE_LIST(it, mConnection)
		{
			mCurrConnection = it->second;

			if (mCurrConnection->mStatus == STATUS_GOOD)
			{
				mCurrConnection->mConnectionTimer += DT;

				if (mCurrConnection->mConnectionTimer > CONNECTION_TIMEOUT)
				{
					Debug::Log(STREAM(mCurrConnection->mUserName << " Logged Out."));

					mCurrConnection->mStatus = STATUS_FAIL;
				}
			}
		}
	}
}}
