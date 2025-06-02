#include "pch.h"
#include "Tank.h"

Tank::Tank()
{
}

Tank::~Tank()
{
}

void Tank::Initialize()
{

}

int Tank::Update(float deltaTime)
{

    // �߽� ��ǥ (��ũ �߰� ���̱��� �ø�)
	Matrix4x4& mat = TransformMatrix;

	// �߽� ��ǥ: ��ũ ��ġ + ������ ����
	_obbBox.center = Vec3(
		mat.m[3][0],             // X (world pos X)
		mat.m[3][1] + 185.f,     // Y (�߽��� ���� 370�� ���ݸ�ŭ ��)
		mat.m[3][2]              // Z (world pos Z)
	);

	// ȸ�� �� ���� ����
	_obbBox.axis[0] = Vec3(mat.m[0][0], mat.m[0][1], mat.m[0][2]); // Right (X��)
	_obbBox.axis[1] = Vec3(mat.m[1][0], mat.m[1][1], mat.m[1][2]); // Up (Y��)
	_obbBox.axis[2] = Vec3(mat.m[2][0], mat.m[2][1], mat.m[2][2]); // Forward (Z��)

	// �ݻ����� ���� (��ũ ũ��: �� 380, ���� 370, ���� 960)
	_obbBox.halfSize = Vec3(190.f, 185.f, 480.f); // �ݾ�

	return 0;
}

void Tank::Late_Update()
{
}

void Tank::Release()
{
}

void Tank::SetTankState(const Matrix4x4& mat, float PotapAngle, float PosinAngle)
{
	__super::SetTransform(mat);
	_potapAngle = PotapAngle;
	_posinAngle = PosinAngle;

}

Tank_INFO Tank::GetTankState()
{

	Tank_INFO myInfo = {

		TransformMatrix,
		_posinAngle,
		_potapAngle,
		_hp

	};

	return myInfo;
}
