#pragma once
#include "ObjectManager.h"

enum ROOM_STATE {

	ROOM_WAITTING, ROOM_INGAME, ROOM_END

};


class Room 
{
public:
	Room();
	Room(uint64 Max);
	~Room();

public:
	void Initialize();
	void Update(float deltaTime);
	void LateUpdate();
	void Release();

public:
	//for PlayerManagement
	void Accept_Player(PlayerRef Player);
	void Leave_Player(PlayerRef Player);

public:
	//for ReadyGame
	bool Check_ClientLoading();
	void Clinet_Loading_Finish();

	void SetInitObject();

	bool GetTerrainData();
	bool Wait_Full(uint16 MaxPlayer);
	void Entered_All();
	void Check_RoomMax();

public:

	bool CollisionTest();
	
	
public:
	//for data
	void Broadcast_PlayerData(Vec3 p1, Vec3 p2);
	void Broadcast_All_TankState(uint8 PlayersCnt);
	void Broadcast_Hit_Weapon(Vec3 Pos);
	void Broadcast(SendBufferRef sendBuffer);

public:
	//for Debug
	void ShowRoomData();
	void ShowTankState(uint8 Id);
	void ShowBulletCnt();

public:
	//for PlayerSet
	void SetTankState(int64 pID, const Matrix4x4& mat, const float& PosinAngle, const float& PotapAngl);
	Tank_INFO GetTankState(int64 pID);

public:
	//for GamePlay
	void CreateBullet(int8 playerID, WEAPON_ID WeaponID, Vec3 Dir, Vec3 Pos);
	void Check_Bullet_Collision();
	void Detect_Bullet_Tank_Collisions();
	bool Check_OBB_Collision(const Vec3& point, const OBB& obb);
public:
	
	ObjectManager Room_ObjectManager;
	

public:


	bool isStart = false;
	bool isMax = false;

private:
	uint8 my_RoomID;
	uint8 RoomMax;
	uint8 Wait_LoadingCnt;

	bool waitStartDelay = false;    // 2초 딜레이 시작 여부
	float waitStartTimer = 0.0f;    // 딜레이 타이머 (초)

	//queue<>
private:

	USE_LOCK;
	map<uint64, PlayerRef>	_Players;

public:

	ROOM_STATE CurState = ROOM_WAITTING;


};

extern Room GRoom;