#pragma once
#

struct Matrix4x4;
enum
{
	S_TEST = 1,//for Dummy
	S_SUCCESS_LOGIN = 2,
	S_SUCCESS_ENTER_ROOM = 3,
	S_GAME_START = 4,
	S_PLAYER_MOVE = 5,
	S_WEAPON_HIT = 6,
	C_LOGIN = 1001,
	C_FINISH_LOADING = 1002,
	C_KEYINPUT = 1003,
	C_MOVEMENT = 1004,
	C_SHOT = 1005
};

struct BuffData
{
	uint64 buffId;
	float remainTime;
};

class ServerPacketHandler
{
public:
	static void HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len);

	//For Recv
	static void Handle_C_LOGIN(PacketSessionRef& session,BYTE* buffer, int32 len);
	static void Handle_C_KEYINPUT(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_MOVEMENT(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_SHOT(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_FINISH_LOADING(PacketSessionRef& session, BYTE* buffer, int32 len);
	//For Send
	static SendBufferRef Make_S_TEST(uint64 id, uint32 hp, uint16 attack);
	static SendBufferRef Make_S_SUCCESS_LOGIN(uint16 id);
	static SendBufferRef Make_S_SUCCESS_ENTER_ROOM(uint16 id);
	static SendBufferRef Make_S_GAME_START(uint16 id);
	static SendBufferRef Make_S_WEAPON_HIT(float x, float y, float z);
	static SendBufferRef Make_S_PLAYER_MOVED(uint8 id, Matrix4x4 mat, float PotapAngle, float PosinAngle);
};

