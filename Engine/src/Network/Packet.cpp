#include "Network/Packet.h"
#include "Network/Socket.h"
#include "Debug.h"
#include "MathUtil.h"
#include "FileUtil.h"

namespace Sentinel {
namespace Network
{
	Packet::GuaranteedMessage::GuaranteedMessage(UINT size) :
		mSize(size),
		mData(new char[size])
	{
		memset(mData, 0, size);
	}

	Packet::GuaranteedMessage::~GuaranteedMessage()
	{
		delete mData;
	}

	/////////////////////////////////////////////////////////////////////

	Packet::GuaranteedSender::GuaranteedSender(UINT ack) :
		mACK(ack),
		mResendTimer(0),
		mTotalTime(0)
	{ }

	bool Packet::GuaranteedSender::operator < (GuaranteedSender* sender)
	{
		return mACK < sender->mACK;
	}

	bool Packet::GuaranteedSender::operator == (GuaranteedSender* sender)
	{
		return mACK == sender->mACK;
	}

	/////////////////////////////////////////////////////////////////////

	std::unordered_map<UINT, Packet::GuaranteedMessage*> Packet::mGuaranteedMessageMap;
	std::unordered_map<Socket::Connection*, std::vector<Packet::GuaranteedSender*>> Packet::mGuaranteedSenderMap;

	UINT Packet::mACK = 0;

	/////////////////////////////////////////////////////////////////////

	Packet::Packet(Socket* socket) :
		mSocket(socket)
	{
		Startup(PACKET_SIZE);
		Clear(PACKET_SIZE);
	}

	Packet::~Packet()
	{
		delete[] mData;
	}

	void Packet::Startup(UINT size)
	{
		mSize = size;
		mData = new char[mSize];
		mStart = mData;

		mACK = 0;
	}

	void Packet::Update(float DT)
	{
		TRAVERSE_LIST(it0, mGuaranteedSenderMap)
		{
			float resend = (float)it0->first->mAverageRTT * 2.5f;
			resend = max(resend, 1.0f);

			TRAVERSE_LIST(it1, it0->second)
			{
				(*it1)->mResendTimer += DT;
				(*it1)->mTotalTime += DT;

				if ((*it1)->mResendTimer > resend)
				{
					(*it1)->mResendTimer = 0;

					GuaranteedMessage* msg = mGuaranteedMessageMap[(*it1)->mACK];

					char* data = msg->mData + sizeof(Packet::Header);

					GuaranteedHeader* gHeader = (GuaranteedHeader*)data;
					gHeader->mAdvanceTimer = (*it1)->mTotalTime;

					mSocket->Send(msg->mData, msg->mSize, it0->first->mID);
				}
			}
		}
	}

	void Packet::Shutdown()
	{
		TRAVERSE_LIST(it, mGuaranteedMessageMap)
			delete it->second;
		mGuaranteedMessageMap.clear();

		TRAVERSE_LIST(it0, mGuaranteedSenderMap)
			TRAVERSE_LIST(it1, it0->second)
			delete (*it1);
		mGuaranteedSenderMap.clear();
	}

	void Packet::Clear(UINT size)
	{
		memset(mData, 0, size);
		mPosition = mData;
		mStart = mPosition;
	}

	///////////////////////////////////////////////////

	PacketSender::PacketSender(Socket* socket) :
		Packet(socket)
	{ }

	void PacketSender::AddHeader(UCHAR header, float advTimer)
	{
		mStart = mPosition;

		AddValue(Packet::Header(header, mSocket->mConnection[0]->mID, (WORD)0));

		if (header & HEADER_GUARANTEED)
		{
			AddValue(GuaranteedHeader(0, advTimer));
		}
	}

	void PacketSender::AddString(const char* v)
	{
		UINT len = (UINT)strlen(v);

		for (UINT y = 0; y < len; ++y)
			AddValue((char)v[y]);

		++mPosition;
	}

	void PacketSender::AddEnd()
	{
		AddValue((BYTE)0);

		int size = mPosition - mStart;

		_ASSERT(size > 0 && size <= (int)mSize);

		((Packet::Header*)mStart)->mSize = (WORD)size;
	}

	bool PacketSender::Send(int index)
	{
		bool result = false;

		int size = mPosition - mData;

		if ((char*)mData != mPosition)
		{
			_ASSERT(size <= (int)mSize);

			Packet::Header* pHeader = (Packet::Header*)mData;

			// Ensure valid packet.
			//
			if (pHeader->mType >= HEADER_UNKNOWN)
			{
				Debug::Log("Bad Header!");

				return false;
			}

			// Prepare Guaranteed Message.
			//
			if (pHeader->mType & HEADER_GUARANTEED)
			{
				// Check if this message is a resend.
				//
				GuaranteedHeader* gHeader = (GuaranteedHeader*)(mData + sizeof(Packet::Header));

				auto itMsg = mGuaranteedMessageMap.find(gHeader->mACK);
				if (itMsg == mGuaranteedMessageMap.end())
				{
					// Copy packet, and store within resend list.
					//
					if (size <= PACKET_SIZE)
					{
						gHeader->mACK = mACK;

						UINT counter = 0;
						bool isValid = false;

						if (index == -1)
						{
							isValid = true;
							TRAVERSE_LIST(it, mSocket->mConnection)
							{
								++counter;
								mGuaranteedSenderMap[it->second].push_back(new GuaranteedSender(mACK));
							}
						}
						else
						{
							auto itConn = mSocket->mConnection.find(index);
							if (itConn != mSocket->mConnection.end())
							{
								isValid = true;
								++counter;
								mGuaranteedSenderMap[itConn->second].push_back(new GuaranteedSender(mACK));
							}
						}

						if (!isValid)
							return false;

						GuaranteedMessage* msg = new GuaranteedMessage(size);
						msg->mCounter = counter;
						memcpy(msg->mData, mData, size);
						mGuaranteedMessageMap[mACK] = msg;

						++mACK;
					}
					else
					{
						Debug::Log(STREAM("Attempted GHeader Size: " << size));

						return false;
					}
				}
			}

			result = mSocket->Send(mData, (UINT)size, index);
		}

		Clear(size);

		return result;
	}

	///////////////////////////////////////////////////

	PacketReceiver::PacketReceiver(Socket* socket, PacketSender* sender) :
		Packet(socket), mSender(sender)
	{ }

	const Packet::Header& PacketReceiver::GetHeader()
	{
		mStart = mPosition;

		return GetValue<Packet::Header>();
	}

	const char* PacketReceiver::GetString()
	{
		const char* v = mPosition;
		mPosition += strlen(mPosition) + 1;
		return v;
	}

	void PacketReceiver::Ignore(UINT size)
	{
		mPosition = mStart + size;
	}

	bool PacketReceiver::Recv()
	{
		Clear(mSize);

		bool result = mSocket->Recv(mData, mSize);

		// Process the received packet.
		//
		if (result)
		{
			mStart = mData;

			while (*mStart != 0)
			{
				mPosition = mStart;

				Socket::Connection* connection = nullptr;
				Packet::Header* pHeader = (Packet::Header*)(mPosition);
				mPosition += sizeof(Packet::Header);

				if (pHeader->mType != HEADER_nullptr)
				{
					// Ensure the packet received is valid.
					//
					if (pHeader->mSenderID != 0)
					{
						connection = mSocket->mConnection[pHeader->mSenderID];
						connection->mConnectionTimer = 0;
					}
					else if (!(pHeader->mType & HEADER_CLIENT))
					{
						Debug::Log(STREAM("Received NetworkPacket from Invalid ID #" << pHeader->mSenderID));

						return false;
					}

					// Offset data for guaranteed message.
					//
					GuaranteedHeader* gHeader = nullptr;
					if (pHeader->mType & HEADER_GUARANTEED)
					{
						gHeader = (GuaranteedHeader*)(mPosition);
						mPosition += sizeof(GuaranteedHeader);
					}

					// Process this packet.
					//
					if (pHeader->mType & HEADER_PACKET)
					{ }
					else if (pHeader->mType & HEADER_KEEP_ALIVE)
					{ }
					else if (pHeader->mType & HEADER_ACK)
					{
						++connection->mIndexRTT;
						WRAP(connection->mIndexRTT, 0, (MAX_RTT_COUNT - 1));

						connection->mAverageRTT = 0;
						for (UINT x = 0; x < MAX_RTT_COUNT; ++x)
						{
							connection->mAverageRTT += connection->mEndRTT[x];
						}
						connection->mAverageRTT /= MAX_RTT_COUNT;

						// Check for acknowledgment.
						//
						UINT ack = *(UINT*)(mPosition);

						// Remove this message from the receiver's list.
						//
						mGuaranteedMessageMap[ack]->mCounter--;
						if (mGuaranteedMessageMap[ack]->mCounter == 0)
							mGuaranteedMessageMap.erase(ack);

						std::vector< GuaranteedSender* >& sender = mGuaranteedSenderMap[connection];
						auto it = binary_find(sender.begin(), sender.end(), &GuaranteedSender(ack));
						connection->mEndRTT[connection->mIndexRTT] = (*it)->mTotalTime;
						sender.erase(it);

						//Debug::Log( "ACK #" << ack << " from " << connection->mUserName );
					}
					// Setup a client on the server.
					//
					else if (pHeader->mType & HEADER_CLIENT)
					{
						// Check for existing user.
						//
						std::string name(mPosition);

						if (connection)
						{
							if (connection->mID != 0)
							{
								// TCP automatically generates a connection when a client
								// attempts to join the server.
								//
								connection->mConnectionTimer = 0;

								if (connection->mStatus == Socket::STATUS_FAIL)
								{
									Debug::Log(STREAM(connection->mUserName << " Reconnected."));
								}
								else
								{
									Debug::Log(STREAM(connection->mUserName << " Connected."));
								}

								connection->mStatus = Socket::STATUS_GOOD;
							}
							else
							{
								// UDP client logging into server.
								//
								Debug::ShowError(
									"This should not have happened.",
									"NetworkSocket Failure");

								connection->mID = mSocket->mNextID++;
								strcpy_s(connection->mUserName, name.c_str());

								Debug::Log(STREAM(connection->mUserName << " Logged In."));
							}
						}
						else
						{
							Socket::Connection* baseConnection = mSocket->mConnection[0];
							connection = new Socket::Connection(mSocket->mNextID++);

							connection->mNetworkSocket = baseConnection->mNetworkSocket;
							connection->mReceiver = mSocket->mReceiverInfo;
							strcpy_s(connection->mUserName, name.c_str());

							mSocket->mConnection[connection->mID] = connection;

							Debug::Log(STREAM(connection->mUserName << " Logged In."));
						}

						// Overwrite the client ID so that the server
						// knows who sent it in setting up the player.
						//
						pHeader->mSenderID = connection->mID;

						if (connection->mStatus == Socket::STATUS_FAIL)
						{
							connection->mStatus = Socket::STATUS_GOOD;
							connection->mConnectionTimer = 0;

							Debug::Log(STREAM("Assigned ID: " << pHeader->mSenderID));

							// Send the client its ID on the server.
							//
							mSender->AddHeader(HEADER_SERVER | HEADER_GUARANTEED);
							mSender->AddValue((UINT)pHeader->mSenderID);
							mSender->AddEnd();
							mSender->Send(pHeader->mSenderID);
						}
					}
					// Setup the client.
					//
					else if (pHeader->mType & HEADER_SERVER)
					{
						connection = mSocket->mConnection[0];

						Debug::Log(STREAM("Logged In --> " << connection->mUserName));

						// Set the current order number.
						//
						if (connection->mID == 0)
						{
							//connection->mNetworkSocket = client;
							connection->mReceiver = mSocket->mReceiverInfo;

							connection->mID = *(UINT*)mPosition;
						}

						Debug::Log(STREAM("Assigned ID: " << connection->mID));
					}
					else
					{
						Debug::Log(STREAM(
							"Invalid Header Received" << "; Type: " << pHeader->mType <<
							"; Sender: " << pHeader->mSenderID <<
							"; Size: " << pHeader->mSize));

						return false;
					}

					// Send ACK.
					//
					if (pHeader->mType & HEADER_GUARANTEED)
					{
						//Debug::Log( "Sending ACK #" << gHeader->ack << " --> " << mCurrReceiver->userName );

						mSender->AddHeader(HEADER_ACK);
						mSender->AddValue((UINT)gHeader->mACK);
						mSender->AddEnd();
						mSender->Send(pHeader->mSenderID);
					}

					mStart += pHeader->mSize;
				}
				else return false;
			}
		}

		return result;
	}
}}
