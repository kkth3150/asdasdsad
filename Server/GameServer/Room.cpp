#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "ClientSession.h"
#include "ServerPacketHandler.h"
#include "AbstractFactory.h"
#include "Tank.h"
#include "Normal_Potan.h"
#include "ObjectManager.h"
#include "Terrain_Manager.h"
#include "SGlobal.h"

Room GRoom;

Room::Room() 
{
	
}

Room::Room(uint64 Max)
{

}

Room::~Room()
{
}

void Room::Initialize()
{
	RoomMax = 2;
	WRITE_LOCK;
	Room_ObjectManager.Add_Object(OBJ_TANK, CAbstractFactory<Tank>::Create(0, 0, 0));
	Room_ObjectManager.Add_Object(OBJ_TANK, CAbstractFactory<Tank>::Create(10, 10, 10));

}

void Room::Update(float deltaTime)
{
	system("cls");

	if (!isMax) {
		if (Wait_Full(RoomMax)) {
			Entered_All();
		}
	}

	if (!isStart) {
		if (Check_ClientLoading()) {

			isStart = true;
		}
	}
	else {
		ShowRoomData();
		ShowTankState(0);
		ShowTankState(1);
		ShowBulletCnt();
		Broadcast_All_TankState(2);
		Room_ObjectManager.Update(deltaTime);
		Detect_Bullet_Tank_Collisions();
		
	}
}

void Room::LateUpdate()
{

	if (isStart) {
		Room_ObjectManager.Late_Update();
		Check_Bullet_Collision();
	}
}

void Room::Release()
{
}

void Room::Accept_Player(PlayerRef Player)
{
	WRITE_LOCK;
	_Players[Player->playerID] = Player;
}

void Room::Leave_Player(PlayerRef Player)
{
	WRITE_LOCK;
	_Players.erase(Player->playerID);
}

bool Room::Check_ClientLoading()
{
	if (Wait_LoadingCnt >= RoomMax) {
		return true;
	}
	return false;
}

void Room::Clinet_Loading_Finish()
{

	Wait_LoadingCnt++;

}

void Room::Entered_All()
{

	isMax = true;
	auto sendBuffer = ServerPacketHandler::Make_S_GAME_START(100);
	Broadcast(sendBuffer);


}

bool Room::CollisionTest()
{
	WRITE_LOCK;
	

	return false;
}

void Room::Broadcast_PlayerData(Vec3 p1, Vec3 p2)
{

}

void Room::Broadcast_All_TankState(uint8 PlayersCnt)
{
	std::vector<GameObject*> tankListCopy;
	{
		READ_LOCK;
		auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);
		if (tankList)
			tankListCopy = *tankList;
	}

	WRITE_LOCK;
	for (const auto& iter : _Players)
	{
		uint64 playerID = iter.first;
		PlayerRef player = iter.second;

		if (player == nullptr || player->OwenerSession == nullptr)
			continue;

		uint64 otherID = (playerID == 0) ? 1 : 0;

		if (otherID >= tankListCopy.size())
			continue;

		Tank* otherTank = dynamic_cast<Tank*>(tankListCopy[otherID]);
		if (otherTank == nullptr)
			continue;

		Tank_INFO info = otherTank->GetTankState();

		auto sendBuffer = ServerPacketHandler::Make_S_PLAYER_MOVED(
			(uint8)otherID,
			info.TankTransform,
			info.PotapAngle,
			info.PosinAngle
		);

		player->OwenerSession->Send(sendBuffer);
	}

}

void Room::Broadcast_Hit_Weapon(Vec3 Pos)
{
	for (const auto& iter : _Players)
	{
		uint64 playerID = iter.first;
		PlayerRef player = iter.second;

		auto sendBuffer = ServerPacketHandler::Make_S_WEAPON_HIT(Pos.X, Pos.Y, Pos.Z);
		player->OwenerSession->Send(sendBuffer);
	}
}



void Room::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (const auto& iter : _Players)
	{
		const PlayerRef& player = iter.second;
		if (player && player->OwenerSession)
			player->OwenerSession->Send(sendBuffer);
	}
}

void Room::ShowRoomData()
{

	std::cout << "���� �÷��� ��" << std::endl;

}



void Room::ShowTankState(uint8 Id)
{
	READ_LOCK;
	Tank_INFO Tank0state = GetTankState(Id);
	std::cout << "Tank  "<<  Id  <<"  ����" << std::endl;
	std::cout << "X: " << Tank0state.TankTransform.m[3][0] << std::endl;  // _41
	std::cout << "Y: " << Tank0state.TankTransform.m[3][1] << std::endl;  // _42
	std::cout << "Z: " << Tank0state.TankTransform.m[3][2] << std::endl;  // _43
	std::cout << "��ž ����: " << Tank0state.PotapAngle << std::endl;
	std::cout << "���� ����: " << Tank0state.PosinAngle << std::endl;
	std::cout << "HP: " << static_cast<int>(Tank0state.TankHP) << std::endl;
}

void Room::ShowBulletCnt()
{

	int BulletCnt = 0;
	if (Room_ObjectManager.Get_List(OBJ_WEAPON)) {
		READ_LOCK;
		BulletCnt = Room_ObjectManager.Get_List(OBJ_WEAPON)->size();
	}
	cout << "������ �Ѿ� : " << BulletCnt << " ��" << endl;
}

bool Room::Wait_Full(uint16 MaxPlayer)
{
	READ_LOCK;
	int playerCnt = (int)_Players.size();

	std::cout << "�÷��̾� ���� ��� ��" << std::endl;
	std::cout << "���� �÷��̾� (" << playerCnt << ") ��" << std::endl;
	std::cout << "�ִ� �÷��̾� (" << MaxPlayer << ") ��" << std::endl;

	
	if (playerCnt >= MaxPlayer)
		return true;

	return false;
		
}



void Room::SetTankState(int64 pID, const Matrix4x4& mat, const float& PotapAngle, const float& PosinAngle)
{
	WRITE_LOCK;
	dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))[pID])->SetTankState(mat, PotapAngle, PosinAngle);

}

Tank_INFO Room::GetTankState(int64 pID)
{
	READ_LOCK;
	return dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))[pID])->GetTankState();
	
}

void Room::CreateBullet(int8 pID, WEAPON_ID ID, Vec3 Dir,Vec3 Pos)
{
	WRITE_LOCK;
	switch (ID) {

	case WEAPON_NPOTAN:
	{
		GameObject* TempBullet = CAbstractFactory<Normal_Potan>::Create();
		dynamic_cast<Normal_Potan*>(TempBullet)->SetInitData(Dir, Pos, pID);
		Room_ObjectManager.Add_Object(OBJ_WEAPON, TempBullet);
	}
		break;

	case WEAPON_NBULLET:
		break;

	default:
		break;


	}
}

void Room::Detect_Bullet_Tank_Collisions()
{
	auto bulletList = Room_ObjectManager.Get_List(OBJ_WEAPON);
	auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);

	if (bulletList == nullptr || tankList == nullptr)
		return;

	WRITE_LOCK;

	for (GameObject* objBullet : *bulletList)
	{
		if (!objBullet) continue;

		Normal_Potan* bullet = dynamic_cast<Normal_Potan*>(objBullet);
		if (!bullet) continue;

		if (bullet->isHit())  
			continue;

		Vec3 bulletPos = bullet->GetPos();
		int bulletOwnerID = bullet->GetOwnerID();

		for (size_t tankIndex = 0; tankIndex < tankList->size(); ++tankIndex)
		{
			if (!(*tankList)[tankIndex]) continue;

			if ((int)tankIndex == bulletOwnerID)
				continue;

			Tank* tank = dynamic_cast<Tank*>((*tankList)[tankIndex]);
			if (!tank) continue;

			const OBB& tankOBB = tank->Get_OBB();

			if (Check_OBB_Collision(bulletPos, tankOBB))
			{
				int a = tankIndex;

				bullet->SetDead();
				break; // �� �Ѿ��� �� ��ũ�� �浹ó��
			}
		}
	}
}

bool Room::Check_OBB_Collision(const Vec3& point, const OBB& obb)
{
	Vec3 dir = { point.X - obb.center.X, point.Y - obb.center.Y, point.Z - obb.center.Z };

	for (int i = 0; i < 3; ++i)
	{
		float dist = dir.X * obb.axis[i].X + dir.Y * obb.axis[i].Y + dir.Z * obb.axis[i].Z;

		float halfSize = (i == 0) ? obb.halfSize.X : (i == 1) ? obb.halfSize.Y : obb.halfSize.Z;

		if (fabs(dist) > halfSize)
			return false;
	}

	return true;
}

void Room::Check_Bullet_Collision()
{

	auto bulletList = Room_ObjectManager.Get_List(OBJ_WEAPON);
	if (bulletList == nullptr)
		return;

	READ_LOCK;

	for (GameObject* obj : *bulletList)
	{
		if (!obj) continue;

		Normal_Potan* bullet = dynamic_cast<Normal_Potan*>(obj);
		if (bullet && bullet->isHit())
		{
			Vec3 hitPos = bullet->GetPos();
			Broadcast_Hit_Weapon(hitPos);
		}
	}

}


