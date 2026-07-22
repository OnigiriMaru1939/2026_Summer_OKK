#pragma once

#include "IBoss.h"
#include "EnemyBase.h"

class Boss4 : public EnemyBase, public IBoss
{
public:
	enum BOSS_STATE
	{
		NON,

		WAIT,
		MOVE,
		JUMP,
		DASH,
		CHARGEDASH,
		STUN,
		LASER,

		MAX
	};

	enum class CHARGEDASH_STATE
	{
		AIM,
		DASH
	};

	enum class LASER_STATE
	{
		AIM,
		LOCK_ON,
		FIRE
	};

	Boss4(FileManager& fileMng, Stage& stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng);
	~Boss4() override;
	void Update() override;
	void Draw() const override;
	void BossAppear();
	void Wait();
	void Move();
	void Jump();
	void Dash();
	void ChargeDash();
	void Stun();
	void LaserAttack();
	void BossStateChange();
	void SelectNextState();
private:
	BOSS_STATE bossState_;
	CHARGEDASH_STATE chargeDashState_;
	LASER_STATE laserState_;
	int stateChangeTimer;
	int chargeDashTimer;
	float rotateSpeed;
	float chargeDashVX_;
	float chargeDashVY_;
	bool stunFlag;
	int stunTimer;
	int laserTimer;
	float laserAngle;
	bool laserFlag;
	float laserDirX_;
	float laserDirY_;
};