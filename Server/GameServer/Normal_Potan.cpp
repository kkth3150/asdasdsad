#include "pch.h"
#include "Normal_Potan.h"
#include "Terrain_Manager.h"


void Normal_Potan::Initialize()
{
    myWeaponID = WEAPON_NPOTAN;
    _isDead = false;
 
}

int Normal_Potan::Update(float deltaTime)
{
    if (_isDead) {

        return 1;
    }

    Process_Move(deltaTime);

    return 0;

}

void Normal_Potan::Late_Update()
{

    if (Check_Collision()) {
        _isDead = true;
    }

}

void Normal_Potan::Release()
{
}

void Normal_Potan::SetInitData(Vec3& normalizedDirection, Vec3& startPos,uint8 ID)
{

    OwnerID = ID;
    _myPos = startPos;
    direction = normalizedDirection;   // ����ȭ�� ���⺤�� ��� ������ ����                    // �ʱ� �ӵ� ��� ������ ����
    velocity = {
        direction.X * speed,
        direction.Y * speed,
        direction.Z * speed
    };
}

void Normal_Potan::Process_Move(float deltaTime)
{
    Vec3 gravity = { 0.f, -6.8f, 0.f };

    // �߷� ���ӵ��� velocity�� ���� (v = v + a * dt)
    velocity.X += gravity.X * deltaTime;
    velocity.Y += gravity.Y * deltaTime;
    velocity.Z += gravity.Z * deltaTime;

    // ��ġ�� velocity �������� deltaTime ��ŭ �̵� (p = p + v * dt)
    _myPos.X += velocity.X * deltaTime;
    _myPos.Y += velocity.Y * deltaTime;
    _myPos.Z += velocity.Z * deltaTime;

}

bool Normal_Potan::Check_Collision()
{

    if (!_isDead) {
        if (Collision_Terrain()) {
            return 1;
        }
    }


    return 0;
}

bool Normal_Potan::Collision_Terrain()
{
    float terrainHeight = Terrain_Manager::GetInstance().Get_Height(_myPos.X, _myPos.Z);

    return (_myPos.Y <= terrainHeight);
}


