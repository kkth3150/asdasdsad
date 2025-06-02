#include "pch.h"
#include "SGlobal.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "ClientSession.h"
#include "Player.h"
#include "Room.h"

void ServerPacketHandler::HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br.Peek(&header);

	switch (header.id)
	{
	case C_LOGIN:
		Handle_C_LOGIN(session,buffer, len);
		break;

	case C_KEYINPUT:
		Handle_C_KEYINPUT(session, buffer, len);
		break;

	case C_MOVEMENT:
		Handle_C_MOVEMENT(session, buffer, len);
		break;
	
	case C_FINISH_LOADING:
		Handle_C_FINISH_LOADING(session, buffer, len);
		break;

	case C_SHOT:
		Handle_C_SHOT(session, buffer, len);
		break;

	default:
		break;
	}
}

void ServerPacketHandler::Handle_C_LOGIN(PacketSessionRef& session,BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	
	static Atomic<uint64> ClientID = 0;
	uint64 newClientID = ClientID.fetch_add(1);

	PlayerRef playerRef = MakeShared<Player>();
	playerRef->playerID = newClientID;
	playerRef->OwenerSession = C_Session;
	C_Session->_players.push_back(playerRef);

	GRoom.Accept_Player(playerRef);
	cout << "Player" << playerRef->playerID << "EnterRoom" << endl;

	auto sendbuffer = Make_S_SUCCESS_LOGIN((uint16)newClientID);
	session->Send(sendbuffer);
		

	//�α���


}


// -> �� ť => �븶�� ���� �������� �����͸� �׾Ƶд�.
//���� �����ӵ��� ù �����ӿ� => ť�� ��� ������ ó���� ��ü�� ������Ʈ -> ��ε�ĳ��Ʈ
//Ű��ǲ->

void ServerPacketHandler::Handle_C_KEYINPUT(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	
	int64 id;
	if (!C_Session->_players.empty())
		id = C_Session->_players[0]->playerID;

	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	uint8 key;

	br >> key;

	cout << "Player(" << id << ") Moved" << endl;
	
	
	//GRoom.MovePlayer(id);


	// �ӵ� ����.
	// �������� ���Ͻô°� ��ũ �˵�.. ���� -> �������� �����Ѵ� ?
	//
	// ���� ������ �ϳ� ���� -> firstDown

}

void ServerPacketHandler::Handle_C_MOVEMENT(PacketSessionRef& session, BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);

	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	Matrix4x4 mat;
	float potapRotation;
	float posinRotation;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			br >> mat.m[i][j];
		}
	}

	br >> potapRotation >> posinRotation;

	if (!C_Session->_players.empty()) {
		uint64 ID = C_Session->_players[0]->playerID;
		GRoom.SetTankState(ID, mat, potapRotation, posinRotation);
	}

}

void ServerPacketHandler::Handle_C_SHOT(PacketSessionRef& session, BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;
	WEAPON_ID WeaponID = WEAPON_NPOTAN;
	Vec3 InitPos;
	Vec3 Normalized_Dir;

	br >>InitPos.X >> InitPos.Y >> InitPos.Z 
		>> Normalized_Dir.X >> Normalized_Dir.Y >> Normalized_Dir.Z;

	uint8 ID = C_Session->_players[0]->playerID;
	GRoom.CreateBullet(ID, WeaponID, Normalized_Dir, InitPos);

}

void ServerPacketHandler::Handle_C_FINISH_LOADING(PacketSessionRef& session, BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;
	GRoom.Clinet_Loading_Finish();

}

SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();
	
	bw << id << hp << attack;



	header->size = bw.WriteSize();
	header->id = S_TEST; // 1 : Test Msg

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}


SendBufferRef ServerPacketHandler::Make_S_SUCCESS_LOGIN(uint16 id)
{

	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id;						// ���� �����͸� ����
	header->size = bw.WriteSize();	// �״��� ��Ȯ�� ��ü ũ�⸦ �����Ѵ�
	header->id = S_SUCCESS_LOGIN;



	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_SUCCESS_ENTER_ROOM(uint16 id)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id;

	header->size = bw.WriteSize();
	header->id = S_SUCCESS_ENTER_ROOM;

	cout << "SEND_SUCCESS_ENTER_MESSAGE" << endl;
	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_GAME_START(uint16 id)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id;

	header->size = bw.WriteSize();
	header->id = S_GAME_START;

	cout << "SEND_GAME_START_MESSAGE" << endl;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_WEAPON_HIT(float x, float y, float z)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << x << y << z;

	header->size = bw.WriteSize();
	header->id = S_WEAPON_HIT;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}


SendBufferRef ServerPacketHandler::Make_S_PLAYER_MOVED(uint8 Id, Matrix4x4 mat, float PotapAngle, float PosinAngle)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << Id;

	for (int i = 0; i < 4; ++i)
	{
		bw << mat.m[i][0];  // row i, col 0
		bw << mat.m[i][1];  // row i, col 1
		bw << mat.m[i][2];  // row i, col 2
		bw << mat.m[i][3];  // row i, col 3
	}
	bw << PotapAngle;
	bw << PosinAngle;

	header->size = bw.WriteSize();
	header->id = S_PLAYER_MOVE;

	sendBuffer->Close(bw.WriteSize());
	return sendBuffer;
}
