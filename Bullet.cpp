#include "Bullet.h"
#include <DxLib.h>


Bullet::Bullet()
{
	x_ = 0.0f;
	y_ = 0.0f;

	vx_ = 0.0f;
	vy_ = 0.0f;

	damage_ = 0;

	isAlive_ = true;
}

void Bullet::Update()
{
	x_ += vx_;
	y_ += vy_;
}

void Bullet::Draw() const
{
	if (!isAlive_) return;

	DrawCircle(
		(int)x_,
		(int)y_,
		5,
		GetColor(255, 255, 0),
		TRUE
	);
}

void Bullet::ApplyDamage(int damage)
{
	damage_ = damage;
}

void Bullet::SetPosition(float x, float y)
{
	x_ = x;
	y_ = y;
}

void Bullet::SetVelocity(float vx, float vy)
{
	vx_ = vx;
	vy_ = vy;
}

void Bullet::SetDamage(int damage)
{
	damage_ = damage;
}

void Bullet::Destroy()
{
	isAlive_ = false;
}

RECT Bullet::GetRect() const
{
	RECT rc;

	rc.left = (LONG)x_ - 5;
	rc.right = (LONG)x_ + 5;
	rc.top = (LONG)y_ - 5;
	rc.bottom = (LONG)y_ + 5;

	return rc;
}

bool Bullet::IsAlive() const
{
	return isAlive_;
}