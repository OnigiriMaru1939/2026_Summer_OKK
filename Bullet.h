#pragma once


#include <DxLib.h>

class Bullet
{
public:
	Bullet();

	void Update();
	void Draw() const;
	void ApplyDamage(int damage);

	// 初期設定
	void SetPosition(float x, float y);
	void SetVelocity(float vx, float vy);
	void SetDamage(int damage);

	// 当たり判定
	RECT GetRect() const;

	// 生存状態
	bool IsAlive() const;
	void Destroy();

	// Getter
	float GetX() const { return x_; }
	float GetY() const { return y_; }
	int GetDamage() const { return damage_; }

private:
	float x_;
	float y_;

	float vx_;
	float vy_;

	int damage_;

	bool isAlive_;
};