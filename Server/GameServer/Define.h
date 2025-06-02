#pragma once


#define PURE	= 0

/*----------------
	For Object
----------------*/

enum ObjectID {

	OBJ_TANK, OBJ_WEAPON ,OBJ_END

};


struct Vec3 {
	float X;
	float Y;
	float Z;

	Vec3() : X(0), Y(0), Z(0) {}
	Vec3(float x, float y, float z) : X(x), Y(y), Z(z) {}

	Vec3 operator-(const Vec3& other) const {
		return Vec3(X - other.X, Y - other.Y, Z - other.Z);
	}

	static float Dot(const Vec3& a, const Vec3& b) {
		return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
	}
};

struct Size {
	float length_X;
	float length_Y;
	float length_Z;
};


struct Matrix4x4
{
	float m[4][4];
};


struct OBB
{
	Vec3 center;     
	Vec3 axis[3];    
	Vec3 halfSize;   
};

/*-----------------
	For Tank
-----------------*/


struct Tank_INFO {

	Matrix4x4	TankTransform;
	float		PosinAngle;
	float		PotapAngle;
	uint8		TankHP;

};



/*-----------------
	For Weapon
-----------------*/

enum WEAPON_ID {

	WEAPON_NPOTAN, WEAPON_NBULLET, WEAPON_END

};


/*-----------------
	For Delete
-----------------*/

template<typename T>
void Safe_Delete(T& Temp)
{
	if (Temp)
	{
		delete Temp;
		Temp = nullptr;
	}
}




/*------------------
	For Protocol
------------------*/


