#include "Bullet.h"
#include "Stage.h"


Bullet::Bullet(FileManager& fileMng, Stage* stage, float x, float y, float vx, float vy, float scale)
	: BulletBase(fileMng, stage, x, y, vx, vy, scale)
{
	SetImage("Resource/Image/Bullet/Fantagy_bullet.png");
	bulletType_ = BULLET_TYPE::B_TYPE_1;
	damage_ = 10; // 攻撃力を設定
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
	BulletBase::Update();
}