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

		MAX
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
	void BossStateChange();
	void SelectNextState();
private:
	BOSS_STATE bossState_;
	int stateChangeTimer;
	float rotateSpeed;
};